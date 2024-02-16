/******************************************************************************
 *  Plot2D:     
 *
 *
 *****************************************************************************/
#include "main.h"

//-------------------------------------------------------------------------
//  Variables para tomar puntos
//-------------------------------------------------------------------------
dato ref;
dato enfriamiento;
dato potencia1;
dato potencia2;
dato potencia3;
dato potencia4;
dato potencia5;
dato potencia6;
dato potencia7;

//-------------------------------------------------------------------------
//  Variables de la camara
//-------------------------------------------------------------------------
Camera2D camera = { 0 };

//-------------------------------------------------------------------------
//  size del punto a tomar 
//-------------------------------------------------------------------------
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
    Image curva = LoadImage("images/spotRAD14.png");
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
    action accionActual = REFERENCIA;

    ref.cont = 0;
    enfriamiento.cont = 0;
    potencia1.cont = 0;
    potencia2.cont = 0;
    potencia3.cont = 0;
    potencia4.cont = 0;
    potencia5.cont = 0;
    potencia6.cont = 0;

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
                tomarPunto(accionActual);
            }
        }

        //---------------------------------------------------------------------
        // Borro el punto anterior
        //---------------------------------------------------------------------
        if (IsKeyReleased(KEY_D))
        {
            removerPunto(accionActual);
        }

        //---------------------------------------------------------------------
        // Cambio de color
        //---------------------------------------------------------------------
        if (IsKeyReleased(KEY_N))
        {
            accionActual > CURVA6 ? accionActual = REFERENCIA : accionActual++;
        }

        //---------------------------------------------------------------------
        // Draw
        //---------------------------------------------------------------------
        BeginDrawing();
        {
            ClearBackground(WHITE);

            BeginMode2D(camera);
            {
                // NOTE: Render texture must be y-flipped due to default OpenGL coordinates (left-bottom)
                // Objetos que se mueven con la camara
                DrawTexture(texture, screenWidth/2, screenHeight/2 + 50, WHITE);

                // Dibujo todos los puntos que se tomaron hasta ahora
                for (action i = REFERENCIA; i <= CURVA7; i++)
                {
                    switch (i)
                    {
                        case REFERENCIA:
                            dibujarPuntos(ref, i);
                            break;
                        case ENFRIAMIENTO:
                            dibujarPuntos(enfriamiento, i);
                            break;
                        case CURVA1:
                            dibujarPuntos(potencia1, i);
                            break;
                        case CURVA2:
                            dibujarPuntos(potencia2, i);
                            break;
                        case CURVA3:
                            dibujarPuntos(potencia3, i);
                            break;
                        case CURVA4:
                            dibujarPuntos(potencia4, i);
                            break;
                        case CURVA5:
                            dibujarPuntos(potencia5, i);
                            break;
                        case CURVA6:
                            dibujarPuntos(potencia6, i);
                            break;
                        case CURVA7:
                            dibujarPuntos(potencia7, i);
                            break;
                    };
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
    fprintf(csvFile, "Minutos, Enfriamiento, Potencia 1, Potencia 2, Potencia 3, Potencia 4, Potencia 5, Potencia 6, Potencia 7, Referencia");
    fprintf(csvFile, "\n");

    // tiempo 0
    fprintf(csvFile, "0,"); // Minutos
    fprintf(csvFile, "0,");
    fprintf(csvFile, "0,");
    fprintf(csvFile, "0,");
    fprintf(csvFile, "0,");
    fprintf(csvFile, "0,");
    fprintf(csvFile, "0,");
    fprintf(csvFile, "0,");
    fprintf(csvFile, "0,");
    fprintf(csvFile, "%f, MAX=,", ref.curva[0].y);
    fprintf(csvFile, "%f, MIN=", ref.curva[1].y);
    fprintf(csvFile, "\n");

    // de 0.1 a 0.9 min
    for (size_t i = 1; i <= 9; i++)
    {
        fprintf(csvFile, "0.%zu,", i); // Minutos
        fprintf(csvFile, "%f,", enfriamiento.curva[i-1].y);
        fprintf(csvFile, "%f,", potencia1.curva[i-1].y);
        fprintf(csvFile, "%f,", potencia2.curva[i-1].y);
        fprintf(csvFile, "%f,", potencia3.curva[i-1].y);
        fprintf(csvFile, "%f,", potencia4.curva[i-1].y);
        fprintf(csvFile, "%f,", potencia5.curva[i-1].y);
        fprintf(csvFile, "%f,", potencia6.curva[i-1].y);
        fprintf(csvFile, "%f,", potencia7.curva[i-1].y);
        fprintf(csvFile, "\n");
    }

    // de 1 a 9 min
    for (size_t i = 1; i <= 9; i++)
    {
        fprintf(csvFile, "%zu,", i); // Minutos
        fprintf(csvFile, "%f,", enfriamiento.curva[i+8].y);
        fprintf(csvFile, "%f,", potencia1.curva[i+8].y);
        fprintf(csvFile, "%f,", potencia2.curva[i+8].y);
        fprintf(csvFile, "%f,", potencia3.curva[i+8].y);
        fprintf(csvFile, "%f,", potencia4.curva[i+8].y);
        fprintf(csvFile, "%f,", potencia5.curva[i+8].y);
        fprintf(csvFile, "%f,", potencia6.curva[i+8].y);
        fprintf(csvFile, "%f,", potencia7.curva[i+8].y);
        fprintf(csvFile, "\n");
    }

    // de 10 a 100 min
    for (size_t i = 1; i <= 10; i++)
    {
        fprintf(csvFile, "%zu" "0,", i); // Minutos
        fprintf(csvFile, "%f,", enfriamiento.curva[i+17].y);
        fprintf(csvFile, "%f,", potencia1.curva[i+17].y);
        fprintf(csvFile, "%f,", potencia2.curva[i+17].y);
        fprintf(csvFile, "%f,", potencia3.curva[i+17].y);
        fprintf(csvFile, "%f,", potencia4.curva[i+17].y);
        fprintf(csvFile, "%f,", potencia5.curva[i+17].y);
        fprintf(csvFile, "%f,", potencia6.curva[i+17].y);
        fprintf(csvFile, "%f,", potencia7.curva[i+17].y);
        fprintf(csvFile, "\n");
    }

    fprintf(csvFile, "\n");
    fprintf(csvFile, "\n");

    fprintf(csvFile, "Minutos, Enfriamiento, Potencia 1, Potencia 2, Potencia 3, Potencia 4, Potencia 5, Potencia 6, Potencia 7");
    fprintf(csvFile, "\n");

    // de 0.1 a 0.9 min
    for (size_t i = 0; i <= 9; i++)
    {
        fprintf(csvFile, "0.%zu,", i); // Minutos
        fprintf(csvFile, "\n");
    }

    // de 1 a 9 min
    for (size_t i = 1; i <= 9; i++)
    {
        fprintf(csvFile, "%zu,", i); // Minutos
        fprintf(csvFile, "\n");
    }

    // de 10 a 100 min
    for (size_t i = 1; i <= 10; i++)
    {
        fprintf(csvFile, "%zu" "0,", i); // Minutos
        fprintf(csvFile, "\n");
    }

    // Cierra el archivo CSV
    fclose(csvFile);
    return 0;
}

Color getColor(action accion)
{
    switch (accion)
    {
        case REFERENCIA:
            return GREEN;
            break;
        case ENFRIAMIENTO:
            return GRAY;
            break;
        case CURVA1:
            return BLUE;
            break;
        case CURVA2:
            return RED;
            break;
        case CURVA3:
            return YELLOW;
            break;
        case CURVA4:
            return ORANGE;
            break;
        case CURVA5:
            return PURPLE;
            break;
        case CURVA6:
            return PINK;
            break;
        case CURVA7:
            return BROWN;
            break;
        default:
            return GREEN;
            break;
    }
}

int getCount(action accion)
{
    switch (accion)
    {
        case REFERENCIA:
            return ref.cont;
            break;
        case ENFRIAMIENTO:
            return enfriamiento.cont;
            break;
        case CURVA1:
            return potencia1.cont;
            break;
        case CURVA2:
            return potencia2.cont;
            break;
        case CURVA3:
            return potencia3.cont;
            break;
        case CURVA4:
            return potencia4.cont;
            break;
        case CURVA5:
            return potencia5.cont;
            break;
        case CURVA6:
            return potencia6.cont;
            break;
        default:
            return ref.cont;
            break;
    }
}

void tomarPunto(action accion)
{
    Vector2 posActual = GetScreenToWorld2D(GetMousePosition(), camera);

    switch (accion)
    {
        case REFERENCIA:
            ref.curva[ref.cont] = posActual;
            ref.cont++;
            break;

        case ENFRIAMIENTO:
            enfriamiento.curva[enfriamiento.cont] = posActual;
            enfriamiento.cont++;
            break;

        case CURVA1:
            potencia1.curva[potencia1.cont] = posActual;
            potencia1.cont++;
            break;

        case CURVA2:
            potencia2.curva[potencia2.cont] = posActual;
            potencia2.cont++;
            break;

        case CURVA3:
            potencia3.curva[potencia3.cont] = posActual;
            potencia3.cont++;
            break;

        case CURVA4:
            potencia4.curva[potencia4.cont] = posActual;
            potencia4.cont++;
            break;

        case CURVA5:
            potencia5.curva[potencia5.cont] = posActual;
            potencia5.cont++;
            break;

        case CURVA6:
            potencia6.curva[potencia6.cont] = posActual;
            potencia6.cont++;
            break;

        case CURVA7:
            potencia7.curva[potencia7.cont] = posActual;
            potencia7.cont++;
            break;
    }
}

void removerPunto(action accion)
{
    switch (accion)
    {
        case REFERENCIA:
            if (ref.cont <= 0)
                return;

            // Decrementa para borrar el anterior
            ref.cont--;

            // Borro x e y
            ref.curva[ref.cont].x = '\0';
            ref.curva[ref.cont].y = '\0';
            break;

        case ENFRIAMIENTO:
            if (enfriamiento.cont <= 0)
                return;

            // Decrementa para borrar el anterior
            enfriamiento.cont--;

            // Borro x e y
            enfriamiento.curva[enfriamiento.cont].x = '\0';
            enfriamiento.curva[enfriamiento.cont].y = '\0';
            break;

        case CURVA1:
            if (potencia1.cont <= 0)
                return;

            // Decrementa para borrar el anterior
            potencia1.cont--;

            // Borro x e y
            potencia1.curva[potencia1.cont].x = '\0';
            potencia1.curva[potencia1.cont].y = '\0';
            break;

        case CURVA2:
            if (potencia2.cont <= 0)
                return;

            // Decrementa para borrar el anterior
            potencia2.cont--;

            // Borro x e y
            potencia2.curva[potencia2.cont].x = '\0';
            potencia2.curva[potencia2.cont].y = '\0';
            break;

        case CURVA3:
            if (potencia3.cont <= 0)
                return;

            // Decrementa para borrar el anterior
            potencia3.cont--;

            // Borro x e y
            potencia3.curva[potencia3.cont].x = '\0';
            potencia3.curva[potencia3.cont].y = '\0';
            break;

        case CURVA4:
            if (potencia4.cont <= 0)
                return;

            // Decrementa para borrar el anterior
            potencia4.cont--;

            // Borro x e y
            potencia4.curva[potencia4.cont].x = '\0';
            potencia4.curva[potencia4.cont].y = '\0';
            break;

        case CURVA5:
            if (potencia5.cont <= 0)
                return;

            // Decrementa para borrar el anterior
            potencia5.cont--;

            // Borro x e y
            potencia5.curva[potencia5.cont].x = '\0';
            potencia5.curva[potencia5.cont].y = '\0';
            break;

        case CURVA6:
            if (potencia6.cont <= 0)
                return;

            // Decrementa para borrar el anterior
            potencia6.cont--;

            // Borro x e y
            potencia6.curva[potencia6.cont].x = '\0';
            potencia6.curva[potencia6.cont].y = '\0';
            break;

        case CURVA7:
            if (potencia7.cont <= 0)
                return;

            // Decrementa para borrar el anterior
            potencia7.cont--;

            // Borro x e y
            potencia7.curva[potencia7.cont].x = '\0';
            potencia7.curva[potencia7.cont].y = '\0';
            break;
    }
}

void dibujarPuntos(dato pos, action accion)
{
    for(int p = 0; p < MAX_PUNTOS; p++)
    {
        if ((int)pos.curva[p].x == '\0')
            break;
        DrawCircle((int)pos.curva[p].x, (int)pos.curva[p].y, brushSize, getColor(accion));
    }
}
