#!/bin/sh

PROGRAM_NAME="plot2D"

C_FLASGS="-Wall"
RAYLIB_FLASGS="-lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -std=c99"

CC=gcc

SRC="src/main.c"

rm -f $PROGRAM_NAME

$CC $C_FLASGS $SRC $RAYLIB_FLASGS -o $PROGRAM_NAME

./$PROGRAM_NAME
