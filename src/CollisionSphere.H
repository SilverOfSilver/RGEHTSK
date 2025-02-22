#pragma once
#include "raylib.h"
#include "raymath.h"
#include "Triangle.H"
#include <any>

// CollisionSphere class
class CollisionSphere {
public:
    Vector3 position;
    Vector3 velocity;  // Velocity of the sphere
    Vector3 acceleration;  // Acceleration of the sphere
    float radius;
    Color collisionColor = BLACK;
    bool coliding = false;
    bool standing = false;
    bool wasStanding = false;

    float gravity = -21.0f;  // Gravity constant
    float friction = 0.99f;  // Friction to slow down the sphere a bit

    CollisionSphere() {}

    CollisionSphere(Vector3 pos, float r) : position(pos), radius(r) {
        velocity = { 0.0f, 0.0f, 0.0f }; // Initially stationary
        acceleration = { 0.0f, gravity, 0.0f };  // Initial gravity acting on the sphere
    }

    void update(Vector3 inputVelocity) {
        wasStanding = standing;

        // If the sphere is not colliding with anything, maintain the velocity
        if (!standing) {
            // Apply the current velocity and acceleration (including gravity)
            velocity.y += gravity * GetFrameTime();
            velocity = Vector3Add(velocity, Vector3Scale(acceleration, GetFrameTime()));
        } else {
            // If collision occurs, apply friction and adjust velocity based on input
            velocity = Vector3Scale(velocity, friction);
            velocity = Vector3Add(velocity, inputVelocity);
        }
    
        // Update the sphere's position based on its velocity
        position = Vector3Add(position, Vector3Scale(velocity, GetFrameTime()));
    }

    void jump(float force){
        if(wasStanding || standing){
        velocity.y += force;
        }
    }

    void swoosh(Vector3 projected) {
        // Normalize the projected vector
        projected = Vector3Normalize(projected);
        
        // Get the current velocity magnitude
        float currentSpeed = Vector3Length(velocity);
        
        // Normalize the velocity
        Vector3 velocityNormalized = Vector3Normalize(velocity);
        
        // Calculate the dot product between velocity and projected
        float dotProduct = Vector3DotProduct(velocityNormalized, projected);
        
        // Compute the angle (in radians) between the two vectors
        float angle = acos(dotProduct);
        
        // Calculate the axis of rotation (cross product of velocity and projected vectors)
        Vector3 rotationAxis = Vector3CrossProduct(velocityNormalized, projected);
        
        if (wasStanding || standing) {
            // Normalize the axis of rotation
            rotationAxis = Vector3Normalize(rotationAxis);
            
            // Apply Rodrigues' rotation formula
            Vector3 rotatedVelocity = Vector3Scale(velocityNormalized, cos(angle)); // v * cos(θ)
            Vector3 crossTerm = Vector3CrossProduct(rotationAxis, velocityNormalized); // (k × v)
            crossTerm = Vector3Scale(crossTerm, sin(angle)); // (k × v) * sin(θ)
            
            Vector3 dotTerm = Vector3Scale(rotationAxis, dotProduct); // k * v
            dotTerm = Vector3Scale(dotTerm, (1 - cos(angle))); // (k * v) * (1 - cos(θ))
            
            // Final rotated velocity
            velocityNormalized = Vector3Add(rotatedVelocity, crossTerm);
            velocityNormalized = Vector3Add(velocityNormalized, dotTerm);
            
            // Restore original velocity magnitude
            velocity = Vector3Scale(Vector3Normalize(velocityNormalized), currentSpeed);
        }
    }


    Vector3 GetClosestPointOnBox(Vector3 point, BoundingBox box) {
        return (Vector3){
            Clamp(point.x, box.min.x, box.max.x),
            Clamp(point.y, box.min.y, box.max.y),
            Clamp(point.z, box.min.z, box.max.z)
        };
    }

    // Compute closest point on a triangle to a sphere center
    Vector3 GetClosestPointOnTriangle(Vector3 p, Vector3 a, Vector3 b, Vector3 c) {
        // Compute triangle normal
        Vector3 ab = Vector3Subtract(b, a);
        Vector3 ac = Vector3Subtract(c, a);
        Vector3 normal = Vector3CrossProduct(ab, ac);
        normal = Vector3Normalize(normal);

        // Project point onto triangle plane
        float d = Vector3DotProduct(normal, a);
        float dist = Vector3DotProduct(normal, p) - d;
        Vector3 projected = Vector3Subtract(p, Vector3Scale(normal, dist));

        // Check if the projected point is inside the triangle
        Vector3 edge0 = Vector3Subtract(b, a);
        Vector3 edge1 = Vector3Subtract(c, b);
        Vector3 edge2 = Vector3Subtract(a, c);

        Vector3 C0 = Vector3Subtract(projected, a);
        Vector3 C1 = Vector3Subtract(projected, b);
        Vector3 C2 = Vector3Subtract(projected, c);

        if (Vector3DotProduct(Vector3CrossProduct(edge0, C0), normal) > 0 &&
            Vector3DotProduct(Vector3CrossProduct(edge1, C1), normal) > 0 &&
            Vector3DotProduct(Vector3CrossProduct(edge2, C2), normal) > 0) {
            return projected; // Inside the triangle
        }

        // Otherwise, get the closest point on the triangle edges
        Vector3 closestAB = GetClosestPointOnSegment(p, a, b);
        Vector3 closestBC = GetClosestPointOnSegment(p, b, c);
        Vector3 closestCA = GetClosestPointOnSegment(p, c, a);

        float dAB = Vector3LengthSqr(Vector3Subtract(p, closestAB));
        float dBC = Vector3LengthSqr(Vector3Subtract(p, closestBC));
        float dCA = Vector3LengthSqr(Vector3Subtract(p, closestCA));

        if (dAB < dBC && dAB < dCA) return closestAB;
        else if (dBC < dCA) return closestBC;
        return closestCA;
    }

    // Get the closest point on a segment (edge of a triangle)
    Vector3 GetClosestPointOnSegment(Vector3 p, Vector3 a, Vector3 b) {
        Vector3 ab = Vector3Subtract(b, a);
        float t = Vector3DotProduct(Vector3Subtract(p, a), ab) / Vector3DotProduct(ab, ab);
        t = Clamp(t, 0.0f, 1.0f);
        return Vector3Add(a, Vector3Scale(ab, t));
    }


    // Check collision with a triangle
    void checkCollision(Triangle T) {
        Vector3 closestPoint = GetClosestPointOnTriangle(position, T.p1, T.p2, T.p3);
        Vector3 collisionNormal = Vector3Subtract(position, closestPoint);

        float distance = Vector3Length(collisionNormal);
        if (distance < radius) {
            coliding= true;
            collisionColor = RED;
            if (distance > 0.0f) {
                collisionNormal = Vector3Scale(collisionNormal, radius / distance);
                position = Vector3Add(closestPoint, collisionNormal);
                if(closestPoint.y < position.y){
                    standing = true;
                }
            }
        }
    }

    void checkCollision(BoundingBox box) {
        if (CheckCollisionBoxSphere(box, position, radius)) {
            coliding = true;
            collisionColor = RED;
            Vector3 closestPoint = GetClosestPointOnBox(position, box);
            Vector3 collisionNormal = Vector3Subtract(position, closestPoint);
            float distance = Vector3Length(collisionNormal);
            if (distance > 0.0f) {
                collisionNormal = Vector3Scale(collisionNormal, radius / distance);
            }
            position = Vector3Add(closestPoint, collisionNormal);
            if(closestPoint.y < position.y){
                standing = true;
            }
        }
    }

    void draw() {
        DrawSphereWires(position, radius, 12, 12, collisionColor);
    }
};
