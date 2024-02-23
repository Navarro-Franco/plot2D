#ifndef MAIN_H
#define MAIN_H

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PUNTOS 100

typedef struct
{
    Vector2 curva[MAX_PUNTOS];  // Pos x e y
    int cont;                   // Contador de puntos
    Color acolor;
} dato;

typedef enum
{
    REFERENCIA = 0,
    CURVA1,
    CURVA2,
    CURVA3,
    CURVA4,
    CURVA5,
    CURVA6,
    CURVA7,
    CURVA8,
    CURVA9,
}action;

Color getColor(action accion);
#endif
