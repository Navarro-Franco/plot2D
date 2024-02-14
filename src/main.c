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
dato curva1;
dato curva2;
dato curva3;
dato curva4;
dato curva5;
dato curva6;

//-------------------------------------------------------------------------
//  Variables de la camara
//-------------------------------------------------------------------------
Camera2D camera = { 0 };

//-------------------------------------------------------------------------
//  size del punto a tomar 
//-------------------------------------------------------------------------
float brushSize = 5.0f;

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
    Image curva = LoadImage("images/RAD14_CURVA.png");
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
    curva1.cont = 0;
    curva2.cont = 0;
    curva3.cont = 0;
    curva4.cont = 0;
    curva5.cont = 0;
    curva6.cont = 0;

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
            const float zoomIncrement = 0.125f;

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

                // > 50 para que no se dibuje encima de la barra del menu
                if (mousePos.y > 50)
                {
                    // Dibujo todos los puntos que se tomaron hasta ahora
                    for (action i = REFERENCIA; i < CURVA6; i++)
                    {
                        switch (i)
                        {
                            case REFERENCIA:
                                dibujarPuntos(ref, i);
                                break;
                            case CURVA1:
                                dibujarPuntos(curva1, i);
                                break;
                            case CURVA2:
                                dibujarPuntos(curva2, i);
                                break;
                            case CURVA3:
                                dibujarPuntos(curva3, i);
                                break;
                            case CURVA4:
                                dibujarPuntos(curva4, i);
                                break;
                            case CURVA5:
                                dibujarPuntos(curva5, i);
                                break;
                            case CURVA6:
                                dibujarPuntos(curva6, i);
                                break;
                        };
                    }
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
    fprintf(csvFile, "Minutos, Enfriamiento, Potencia 1, Potencia 2, Potencia 3, Potencia 4, Potencia 5, Referencia");
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
        case CURVA1:
            return curva1.cont;
            break;
        case CURVA2:
            return curva2.cont;
            break;
        case CURVA3:
            return curva3.cont;
            break;
        case CURVA4:
            return curva4.cont;
            break;
        case CURVA5:
            return curva5.cont;
            break;
        case CURVA6:
            return curva6.cont;
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
        case CURVA1:
            curva1.curva[curva1.cont] = posActual;
            curva1.cont++;
            break;
        case CURVA2:
            curva2.curva[curva2.cont] = posActual;
            curva2.cont++;
            break;
        case CURVA3:
            curva3.curva[curva3.cont] = posActual;
            curva3.cont++;
            break;
        case CURVA4:
            curva4.curva[curva4.cont] = posActual;
            curva4.cont++;
            break;
        case CURVA5:
            curva5.curva[curva5.cont] = posActual;
            curva5.cont++;
            break;
        case CURVA6:
            curva6.curva[curva6.cont] = posActual;
            curva6.cont++;
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

        case CURVA1:
            if (curva1.cont <= 0)
                return;

            // Decrementa para borrar el anterior
            curva1.cont--;

            // Borro x e y
            curva1.curva[curva1.cont].x = '\0';
            curva1.curva[curva1.cont].y = '\0';
            break;

        case CURVA2:
            if (curva2.cont <= 0)
                return;

            // Decrementa para borrar el anterior
            curva2.cont--;

            // Borro x e y
            curva2.curva[curva2.cont].x = '\0';
            curva2.curva[curva2.cont].y = '\0';
            break;

        case CURVA3:
            if (curva3.cont <= 0)
                return;

            // Decrementa para borrar el anterior
            curva3.cont--;

            // Borro x e y
            curva3.curva[curva3.cont].x = '\0';
            curva3.curva[curva3.cont].y = '\0';
            break;

        case CURVA4:
            if (curva4.cont <= 0)
                return;

            // Decrementa para borrar el anterior
            curva4.cont--;

            // Borro x e y
            curva4.curva[curva4.cont].x = '\0';
            curva4.curva[curva4.cont].y = '\0';
            break;

        case CURVA5:
            if (curva5.cont <= 0)
                return;

            // Decrementa para borrar el anterior
            curva5.cont--;

            // Borro x e y
            curva5.curva[curva5.cont].x = '\0';
            curva5.curva[curva5.cont].y = '\0';
            break;

        case CURVA6:
            if (curva6.cont <= 0)
                return;

            // Decrementa para borrar el anterior
            curva6.cont--;

            // Borro x e y
            curva6.curva[curva6.cont].x = '\0';
            curva6.curva[curva6.cont].y = '\0';
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
