#!/bin/sh

PROGRAM_NAME="plot2D"

C_FLASGS="-Wall"
RAYLIB_FLASGS="-lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -std=c99"

CC=gcc

SRC="src/main.c"

rm -rf build
mkdir build

$CC $C_FLASGS $SRC $RAYLIB_FLASGS -o ./build/$PROGRAM_NAME

./build/$PROGRAM_NAME
