#!/bin/sh

mkdir -p "_Compiler"

cd "_Compiler/Debug"
cmake --build . --config Debug
cd "../Release"
cmake --build . --config Release
cd ../..
