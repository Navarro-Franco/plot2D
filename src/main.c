/*******************************************************************************************
*
*   raylib [textures] example - Texture loading and drawing
*
*   Example originally created with raylib 1.0, last time updated with raylib 1.0
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2014-2024 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

// Headers para la camara
#include "rlgl.h"
#include "raymath.h"

#include <stdio.h>

typedef struct referencias
{
    Vector2 pos;
    Color color;
} ref;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int myMonitor = GetCurrentMonitor();
    const int screenWidth = GetMonitorWidth(myMonitor);
    const int screenHeight = GetMonitorHeight(myMonitor);

    InitWindow(screenWidth, screenHeight, "Plot2D --- Experimental");

    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
    // Definicion de la imagen a analizar
    Image curva = LoadImage("RAD14_CURVA.png");
    int ImageWidth = curva.width;
    int ImageHeight = curva.height;
    ImageResize(&curva, ImageWidth, ImageHeight);
    Texture2D texture = LoadTextureFromImage(curva);        // Texture loading
    
    //---------------------------------------------------------------------------------------
    Camera2D camera = { 0 };
    camera.zoom = 0.875f;

    // Mis valores
    ref splot[100];
    int cplot = 0;

    // Datos para los colores
    float brushSize = 10.0f;

    // Main game loop
    SetTargetFPS(60);
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // Translate based on mouse right click
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f/camera.zoom);

            camera.target = Vector2Add(camera.target, delta);
        }

        // Zoom based on mouse wheel
        float wheel = GetMouseWheelMove();
        if (wheel != 0)
        {
            // Get the world point that is under the mouse
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
            
            // Set the offset to where the mouse is
            camera.offset = GetMousePosition();

            // Set the target to match, so that the camera maps the world space point 
            // under the cursor to the screen space point under the cursor at any zoom
            camera.target = mouseWorldPos;

            // Zoom increment
            const float zoomIncrement = 0.125f;

            camera.zoom += (wheel*zoomIncrement);
            if (camera.zoom < zoomIncrement) camera.zoom = zoomIncrement;
        }

        // Pongo el punto de muestro
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            if (cplot < 100)
            {
                splot[cplot].pos = GetScreenToWorld2D(GetMousePosition(), camera);
                cplot++; // Incrementa para un nuevo punto
                // Por ahora es rojo, esto se deberia de cambiar dependiendo
                // de si es otra energia, si es un punto de referencia, etc.
                splot[cplot].color = RED;
            }
        }

        if (IsKeyReleased(KEY_D))
        {
            if (cplot > 0) cplot--; // Decrementa para borrar el anterior
            splot[cplot].pos = (Vector2){0, 0};
            splot[cplot].color = BLANK;
            printf("\nBorrando punto %d", cplot);
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        {
            ClearBackground(WHITE);
            // NOTE: Render texture must be y-flipped due to default OpenGL coordinates (left-bottom)
            // Objetos que se mueven con la camara
            BeginMode2D(camera);
            {
                DrawTexture(texture, screenWidth/2, screenHeight/2, WHITE);
                for (int i = 0; i <= cplot; i++)
                {
                    DrawCircle((int)splot[i].pos.x, (int)splot[i].pos.y, brushSize, splot[cplot].color);
                }
            }
            EndMode2D();
        }
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texture);     // Texture unloading
    UnloadImage(curva);         // Unload image from RAM
    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
