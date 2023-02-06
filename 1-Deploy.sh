#!/bin/sh

git submodule update --init --recursive

chmod +x "External/Packman/packman"
chmod +x "2-Build.sh"
chmod +x "3-Prepare NRI SDK.sh"
chmod +x "4-Clean.sh"

mkdir -p "_Compiler"
mkdir -p "_Compiler/Debug"
mkdir -p "_Compiler/Release"

cmake . -G "Ninja" -DCMAKE_CXX_COMPILER=clang -DCMAKE_BUILD_TYPE=Debug -B _Compiler/Debug/ 

cmake . -G "Ninja" -DCMAKE_CXX_COMPILER=clang -DCMAKE_BUILD_TYPE=Release -B _Compiler/Release/ 

