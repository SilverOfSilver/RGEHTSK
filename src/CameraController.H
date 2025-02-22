#pragma once

#include "raylib.h"
#include "raymath.h"
#include "CollisionSphere.H"

class CameraController {

    

public:
    Camera camera;
    CollisionSphere sphere;
    float rotationSpeed;
    Vector2 lastMousePosition = GetMousePosition();


    CameraController(){}

    CameraController(Camera c, CollisionSphere s, float sens){
        camera = c;
        sphere = s;
        rotationSpeed = sens;
    }
    

    void update(){
        camera.target = (sphere.position+Vector3{0.0,1.5,0.0})-(camera.position-camera.target);
        camera.position = sphere.position;
        camera.position += {0.0,1.5,0.0};

        // Update
        Vector2 mouseDelta = Vector2Subtract(GetMousePosition(), lastMousePosition);

        // Calculate the rotation based on mouse movement
        float yaw = -mouseDelta.x * rotationSpeed; // Rotate around the Y-axis
        float pitch = mouseDelta.y * rotationSpeed; // Rotate around the X-axis

        // Apply the rotation to the camera target
        // Rotate around the camera position with respect to yaw and pitch
        Vector3 direction = Vector3Subtract(camera.target, camera.position);  // Get direction vector

        // Rotate around the Y-axis (yaw)
        direction = Vector3RotateByAxisAngle(direction, camera.up, yaw);

        // Rotate around the X-axis (pitch)
        Vector3 right = Vector3CrossProduct(camera.up, direction);
        direction = Vector3RotateByAxisAngle(direction, right, pitch);

        // Normalize direction and set camera target
        direction = Vector3Normalize(direction);
        camera.target = Vector3Add(camera.position, direction);  // Set the target 1 unit away

        // Update last mouse position
        lastMousePosition = GetMousePosition();

        UpdateCamera(&camera, CAMERA_CUSTOM);
    }

    Vector3 forwardV(){

        // Calculate the vector between the points (camera position and target)
        Vector3 vectorBetween = Vector3Subtract(camera.position, camera.target);

        // Normalize the vector to get the unit vector
        Vector3 unitVector = Vector3Normalize(vectorBetween);

        return unitVector;
    }

    Vector3 rightV(){
        //Up vector
        Vector3 upV = {0.0f, 1.0f, 0.0f};

        // Normalize the vector to get the unit vector
        Vector3 unitVector = forwardV();

        // Calculate the right vector using the cross product
        Vector3 rightV = Vector3CrossProduct(upV, unitVector);

        return Vector3Normalize(rightV);
    }
    
    void Update();
    void HandleInput();
    void ApplyRotation();
};
