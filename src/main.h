#ifndef MAIN_H
#define MAIN_H

#include "raylib.h"

// Headers para la camara
#include "rlgl.h"
#include "raymath.h"

// Headers para debug
#include <stdio.h>
#include <string.h>

#define MAX_PUNTOS 100

typedef struct datos
{
    Vector2 curva[MAX_PUNTOS];  // Pos x e y
    int cont;                   // Contador de puntos
}dato;

typedef enum
{
    REFERENCIA = 0,
    CURVA1,
    CURVA2,
    CURVA3,
    CURVA4,
    CURVA5,
    CURVA6,
}action;

Color getColor(action accion);
int getCount(action accion);
void tomarPunto(action accion);
void removerPunto(action accion);
void removerPuntoAnterior(dato pos);
void dibujarPuntos(dato pos, action accion);
#endif
