@echo off

set NRI_DIR=.

rd /q /s "_NRI_SDK"

mkdir "_NRI_SDK\Include\Extensions"
mkdir "_NRI_SDK\Lib\Debug"
mkdir "_NRI_SDK\Lib\Release"

cd "_NRI_SDK"

copy "..\%NRI_DIR%\Include\*" "Include"
copy "..\%NRI_DIR%\Include\Extensions\*" "Include\Extensions"
copy "..\_Build\Debug\libNRI.a" "Lib\Debug"
copy "..\_Build\Release\libNRI.a" "Lib\Release"
copy "..\%NRI_DIR%\LICENSE.txt" "."

cd ..
