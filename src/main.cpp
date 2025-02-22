#include "raylib.h"
#include "raymath.h"
#include "CameraController.H"
#include "CollisionSphere.H"
#include "Triangle.H"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>


std::vector<Triangle> LoadObjAsTriangles(const std::string& filename, Color color) {
    std::vector<Vector3> vertices;
    std::vector<Triangle> triangles;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << filename << std::endl;
        return triangles;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {  // Vertex
            Vector3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        }
        else if (type == "f") {  // Face
            int v1, v2, v3;
            iss >> v1 >> v2 >> v3;
            triangles.push_back(Triangle(vertices[v3 - 1], vertices[v2 - 1], vertices[v1 - 1], color));
        }
    }

    file.close();
    return triangles;
}

Vector3 flatten(Vector3 in){
    return Vector3{in.x, 0.0, in.z};
}

void DrawCubeFromBoundingBox(BoundingBox box, Color color) {
    Vector3 size = Vector3Subtract(box.max, box.min);
    Vector3 center = Vector3Scale(Vector3Add(box.min, box.max), 0.5f);
    DrawCubeV(center, size, color);
    DrawCubeWiresV(center, size, BLACK);
}

int main() {
    int screenWidth = 1000;
    int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "3D Box Collision with Sphere Sliding");
    bool jump = false;

    Camera3D camera = { 0 };
    camera.position = (Vector3){ 5.0f, 5.0f, 5.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 80.0f;
    camera.projection = CAMERA_CUSTOM;

    DisableCursor();

    // Time-related variables
    Vector2 lastMousePosition = GetMousePosition();
    float rotationSpeed = 0.002f;

    // Define bounding boxes
    BoundingBox box1 = {
        (Vector3){ -1.0f, -100.0f, -10.0f },
        (Vector3){ 1.0f, 10.0f, 1.0f }
    };


    // Create a collision sphere
    CollisionSphere sphere((Vector3){ 3.5f, 10.5f, 0.5f }, 1.0f);

    CameraController camy = CameraController(camera, sphere, 0.005);

    //terrain collision
    std::vector<Triangle> TerrainCollision = LoadObjAsTriangles("./src/LS1C.obj", GREEN);

    // Load FBX model
    Model model = LoadModel("./src/LS1.glb");  // Ensure the file exists in the working directory
    Texture2D texture = LoadTexture("./src/terrain.png");  // Load texture if available
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;  // Apply texture
    Vector3 modelPosition = { 0.0f, 0.0f, 0.0f };


    // Main loop
    while (!WindowShouldClose()) {

        camy.update();

        Vector3 projectedVector = {0, 0, 0};
        
        if (IsKeyDown(KEY_W)) projectedVector -= flatten(camy.forwardV());
        if (IsKeyDown(KEY_S)) projectedVector += flatten(camy.forwardV());
        if (IsKeyDown(KEY_A)) projectedVector -= flatten(camy.rightV());
        if (IsKeyDown(KEY_D)) projectedVector += flatten(camy.rightV());
        

        if (IsKeyDown(KEY_SPACE)){
            if(!jump){
                jump = true;
                camy.sphere.jump(19.5);
            }
        }else{
            jump = false;
        };

        if (IsKeyPressed(KEY_LEFT_SHIFT)){
            camy.sphere.swoosh(camy.forwardV()*(-8.5f));
            printf("Swoosh");
        }

        // Move sphere and check collision only if there's movement
        if (Vector3Length(projectedVector) > 0) {
            camy.sphere.update(Vector3Normalize(projectedVector)*0.17);
        } else {
            camy.sphere.update({0,0,0});
        }
        

        // Collision checks: less frequent or simplified collision checks
        camy.sphere.coliding = false;
        camy.sphere.standing = false;
        camy.sphere.collisionColor = BLACK;
        camy.sphere.checkCollision(box1);  // Check and resolve bounding box collisions
        for (auto& tri : TerrainCollision) {
            camy.sphere.checkCollision(tri);
        }
        
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camy.camera);

        // Draw static objects (not recalculated each frame)
        DrawCubeFromBoundingBox(box1, BLUE);

        // Draw the moving sphere
        camy.sphere.draw();
        /*
        T1.draw(); // Static triangle
        for (auto& tri : model) {
            tri.draw();
        }
        */
        DrawModel(model, modelPosition, 1.0f, WHITE);  // Draw model at position

        EndMode3D();

        if (model.meshCount == 0) {
            std::cerr << "Failed to load model!" << std::endl;
        }
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
