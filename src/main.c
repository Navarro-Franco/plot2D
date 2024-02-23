/******************************************************************************
 *  Plot2D:     
 *
 *
 *****************************************************************************/
#include "main.h"

//-----------------------------------------------------------------------------
//  Variables de la camara
//-----------------------------------------------------------------------------
Camera2D camera = { 0 };

//-----------------------------------------------------------------------------
//  size del punto a tomar 
//-----------------------------------------------------------------------------
float brushSize = 2.0f;

//-----------------------------------------------------------------------------
// Program main entry point
//-----------------------------------------------------------------------------
int main(void)
{
    //-------------------------------------------------------------------------
    // Initialization
    //-------------------------------------------------------------------------
    int myMonitor = GetCurrentMonitor();
    const int screenWidth = GetMonitorWidth(myMonitor);
    const int screenHeight = GetMonitorHeight(myMonitor);

    InitWindow(screenWidth, screenHeight, "Plot2D --- Experimental");

    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
    // Definicion de la imagen a analizar
    Image curva = LoadImage("images/X39_FF_50Hz.png");
    int ImageWidth = curva.width;
    int ImageHeight = curva.height;
    ImageResize(&curva, ImageWidth, ImageHeight);
    Texture2D texture = LoadTextureFromImage(curva);        // Texture loading

    //-------------------------------------------------------------------------
    //  Variables de la camara
    //-------------------------------------------------------------------------
    camera.zoom = 0.875f;

    //-------------------------------------------------------------------------
    // Inicia en referencia
    //-------------------------------------------------------------------------

    // Esto esta hardcodeado, se deberia de poner en la pantalla
    int potencias = 10;

    // Creo las curvas
    dato *curvas = (dato *)malloc(potencias * sizeof(dato));

    // Limpio los contadores 
    for (int i = 0; i < potencias; i++)
        curvas[i].cont = 0;

    // Asigno los colores
    for (int i = 0; i < potencias; i++)
        curvas[i].acolor = getColor((action)i);

    action accionActual = REFERENCIA;

    // Boton para el guardado
    Rectangle btnSaveRec = { 750, 10, 40, 30 };
    bool btnSaveMouseHover = false;

    // Main game loop
    SetTargetFPS(60);
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        //---------------------------------------------------------------------
        // Actualizacion de la posicion para el menu
        //---------------------------------------------------------------------
        Vector2 mousePos = GetMousePosition();

        //---------------------------------------------------------------------
        // Trastradando la camara
        //---------------------------------------------------------------------
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f/camera.zoom);

            camera.target = Vector2Add(camera.target, delta);
        }

        //---------------------------------------------------------------------
        // Zoom basado en la rueda del mouse
        //---------------------------------------------------------------------
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
            const float zoomIncrement = 0.5f;

            camera.zoom += (wheel*zoomIncrement);
            if (camera.zoom < zoomIncrement) camera.zoom = zoomIncrement;
        }

        //---------------------------------------------------------------------
        // Marco un nuevo punto
        //---------------------------------------------------------------------
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            if (mousePos.y > 50)
            {
                // Obtengo la pos del click
                Vector2 posActual = GetScreenToWorld2D(GetMousePosition(), camera);

                // Lo guardo en su respectiva curva
                curvas[accionActual].curva[curvas[accionActual].cont] = posActual;
                curvas[accionActual].cont++;
            }
        }

        //---------------------------------------------------------------------
        // Borro el punto anterior
        //---------------------------------------------------------------------
        if (IsKeyReleased(KEY_D))
        {
            if (curvas[accionActual].cont > 0)
            {
                // Decrementa para borrar el anterior
                curvas[accionActual].cont--;

                // Borro x e y
                curvas[accionActual].curva[curvas[accionActual].cont].x = '\0';
                curvas[accionActual].curva[curvas[accionActual].cont].y = '\0';
            }
        }

        //---------------------------------------------------------------------
        // Cambio de color
        //---------------------------------------------------------------------
        if (IsKeyReleased(KEY_N))
        {
            if (++accionActual > potencias)
                accionActual = REFERENCIA;
        }

        //---------------------------------------------------------------------
        // Draw
        //---------------------------------------------------------------------
        BeginDrawing();
        {
            ClearBackground(WHITE);

            // Objetos que se mueven con la camara
            BeginMode2D(camera);
            {
                // NOTE: Render texture must be y-flipped due to default OpenGL coordinates (left-bottom)
                DrawTexture(texture, screenWidth/2, screenHeight/2 + 50, WHITE);

                // Dibujo todos los puntos que se tomaron hasta ahora
                for (int p = 0; p < potencias; p++)
                {
                    for (int i = 0; i < MAX_PUNTOS; i++)
                    {
                        if ((int)curvas[p].curva[i].x == '\0')
                            break;
                        DrawCircle(
                                (int)curvas[p].curva[i].x,
                                (int)curvas[p].curva[i].y,
                                brushSize,
                                curvas[p].acolor
                                );
                    }
                }

                // > 50 para que no se dibuje encima de la barra del menu
                if (mousePos.y > 50)
                {
                    // Dibujo un circulo para indicar el color en que esta
                    Vector2 brushPos = GetScreenToWorld2D(GetMousePosition(), camera);
                    DrawCircle(brushPos.x, brushPos.y, brushSize, getColor(accionActual));
                }
            }
            EndMode2D();
        }

        // Draw top panel
        DrawRectangle(0, 0, GetScreenWidth(), 50, RAYWHITE);
        DrawLine(0, 50, GetScreenWidth(), 50, LIGHTGRAY);

        // Draw save image button
        DrawRectangleLinesEx(btnSaveRec, 2, btnSaveMouseHover ? RED : BLACK);
        DrawText("SAVE!", 755, 20, 10, btnSaveMouseHover ? RED : BLACK);
        EndDrawing();
    }

    //-------------------------------------------------------------------------
    // De-Initialization
    //-------------------------------------------------------------------------
    UnloadTexture(texture);     // Texture unloading
    UnloadImage(curva);         // Unload image from RAM
    CloseWindow();              // Close window and OpenGL context

    //-------------------------------------------------------------------------
    // Guardo los datos en csv
    //-------------------------------------------------------------------------

    FILE *csvFile = fopen("build/csvTest.csv", "w");
    if (!csvFile) {
        printf("Error al abrir el archivo CSV.\n");
        return 1;
    }

    // Escribo el encabezado
    fprintf(csvFile, "Minutos,");

    for (int i = 0; i < potencias; i++)
        fprintf(csvFile, "Potencia %d,", i+1);

    fprintf(csvFile, "referencia,");
    fprintf(csvFile, "\n");

    // tiempo 0
    fprintf(csvFile, "0,"); // Minutos

    for (int i = 0; i < potencias; i++)
        fprintf(csvFile, "0,");

    fprintf(csvFile, "%f, MAX=,", curvas[0].curva[0].y);
    fprintf(csvFile, "%f, MIN=,", curvas[0].curva[1].y);

    fprintf(csvFile, "\n");

    // de 0.1 a 0.9
    for (size_t t = 1; t < 10; t++)
    {
        fprintf(csvFile, "0.%zu,", t);

        for (size_t p = 1; p < potencias; p++)
            fprintf(csvFile, "%f,", curvas[p].curva[t-1].y);

        fprintf(csvFile, "\n");
    }

    // de 1 a 9
    for (size_t t = 1; t < 10; t++)
    {
        fprintf(csvFile, "%zu,", t);

        for (size_t p = 1; p < potencias; p++)
            fprintf(csvFile, "%f,", curvas[p].curva[t+8].y);

        fprintf(csvFile, "\n");
    }

    // de 10 a 100
    for (size_t t = 1; t <= 10; t++)
    {
        fprintf(csvFile, "%zu" "0,", t);

        for (size_t p = 0; p < potencias; p++)
            fprintf(csvFile, "%f,", curvas[p].curva[t+17].y);

        fprintf(csvFile, "\n");
    }

    fprintf(csvFile, "\n");
    fprintf(csvFile, "\n");

    // Escribo el encabezado
    fprintf(csvFile, "Minutos,");

    for (int i = 0; i < potencias; i++)
        fprintf(csvFile, "Potencia %d,", i+1);

    fprintf(csvFile, "\n");

    // de 0.1 a 0.9
    for (size_t t = 1; t < 10; t++)
    {
        fprintf(csvFile, "0.%zu,", t);
        fprintf(csvFile, "\n");
    }

    // de 1 a 9
    for (size_t t = 1; t < 10; t++)
    {
        fprintf(csvFile, "%zu,", t);
        fprintf(csvFile, "\n");
    }

    // de 10 a 100
    for (size_t i = 1; i <= 10; i++)
    {
        fprintf(csvFile, "%zu" "0,", i);
        fprintf(csvFile, "\n");
    }

    // Cierra el archivo CSV
    fclose(csvFile);

    // Limpio las curvas
    free(curvas);
    return 0;
}

Color getColor(action accion)
{
    switch (accion)
    {
        case REFERENCIA:
            return GREEN;
            break;

        case CURVA1:
            return GRAY;
            break;

        case CURVA2:
            return YELLOW;
            break;

        case CURVA3:
            return ORANGE;
            break;

        case CURVA4:
            return PINK;
            break;

        case CURVA5:
            return RED;
            break;

        case CURVA6:
            return MAGENTA;
            break;

        case CURVA7:
            return LIME;
            break;

        case CURVA8:
            return SKYBLUE;
            break;

        case CURVA9:
            return PURPLE;
            break;
    }
}
