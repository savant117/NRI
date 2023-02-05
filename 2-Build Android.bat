@echo off

cd "_Compiler\Debug"
ninja
cd "..\Release"
ninja
cd ..\..
