#include "raylib.h"
#include "rlgl.h"  // Needed for 3D transformations

int main() {
    // Initialize the window
    int screenWidth = 800;
    int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Spinning Pyramid with Camera Control");

    // Camera setup
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 5.0f, 5.0f, 5.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;  

	DisableCursor();


    float rotationAngle = 0.0f; // Rotation angle of pyramid

    while (!WindowShouldClose()) {
        // Update rotation angle of pyramid
        rotationAngle += 0.01f;  // Rotation per frame

        BeginDrawing();
        ClearBackground(RAYWHITE);

        UpdateCamera(&camera, CAMERA_FREE);
		BeginMode3D(camera);

        // Apply rotation
        rlPushMatrix();
        rlRotatef(rotationAngle, 0.99f, 1.0f, 0.2f);  // Rotate around the Y-axis

        // Define the pyramid vertices
        Vector3 top = { 0.0f, 1.0f, 0.0f };
        Vector3 base1 = { -1.0f, 0.0f, -1.0f };
        Vector3 base2 = { 1.0f, 0.0f, -1.0f };
        Vector3 base3 = { 1.0f, 0.0f, 1.0f };
        Vector3 base4 = { -1.0f, 0.0f, 1.0f };

		//lines drawn all at once to avoid extra drawing steps
		DrawLine3D(top, base1, BLACK);
		DrawLine3D(top, base2, BLACK);
		DrawLine3D(top, base3, BLACK);
		DrawLine3D(top, base4, BLACK);
		DrawLine3D(base1, base2, BLACK);
		DrawLine3D(base3, base2, BLACK);
		DrawLine3D(base1, base4, BLACK);
		DrawLine3D(base3, base4, BLACK);


        // Draw pyramid faces
        DrawTriangle3D(top, base2, base1, RED);       // Front face
        DrawTriangle3D(top, base3, base2, GREEN);     // Right face
        DrawTriangle3D(top, base4, base3, BLUE);      // Back face
        DrawTriangle3D(top, base1, base4, YELLOW);    // Left face

		//lines drawn all at once to avoid extra drawing steps

        rlPopMatrix();

        EndMode3D();

        DrawText("Spinning Pyramid with Camera Control", 10, 10, 20, DARKGRAY);

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();

    return 0;
}
