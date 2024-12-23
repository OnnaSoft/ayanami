#!/bin/bash

BUILD_DIR="build"

if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
    cd "$BUILD_DIR"
    cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
    cd ..
fi

if [ -z "$1" ]; then
    echo "No se especificó un ejecutable. Compilando todos los targets..."
    cd "$BUILD_DIR"
    make
    cd ..

    if [ -f "$BUILD_DIR/compile_commands.json" ]; then
        cp "$BUILD_DIR/compile_commands.json" .
    fi

    exit 0
fi

EXECUTABLE=$1

if [ -f "$BUILD_DIR/$EXECUTABLE" ]; then
    echo "Eliminando ejecutable existente: $EXECUTABLE"
    rm "$BUILD_DIR/$EXECUTABLE"
fi

cd "$BUILD_DIR"
make "$EXECUTABLE"

if [ -f "compile_commands.json" ]; then
    cp compile_commands.json ../
fi

cd ..
