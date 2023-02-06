@echo off

set NRI_DIR=.

rd /q /s "_NRI_SDK"

mkdir "_NRI_SDK\Include\Extensions"
mkdir "_NRI_SDK\Lib\Android\x86_64\Debug"
mkdir "_NRI_SDK\Lib\Android\x86_64\Release"
mkdir "_NRI_SDK\Lib\Android\arm64-v8a\Debug"
mkdir "_NRI_SDK\Lib\Android\arm64-v8a\Release"

cd "_NRI_SDK"

copy "..\%NRI_DIR%\Include\*" "Include"
copy "..\%NRI_DIR%\Include\Extensions\*" "Include\Extensions"
copy "..\_Build\Android\x86_64\Debug\libNRI.so" "Lib\Android\x86_64\Debug"
copy "..\_Build\Android\x86_64\Release\libNRI.so" "Lib\Android\x86_64\Release"
copy "..\_Build\Android\arm64-v8a\Debug\libNRI.so" "Lib\Android\arm64-v8a\Debug"
copy "..\_Build\Android\arm64-v8a\Release\libNRI.so" "Lib\Android\arm64-v8a\Release"
copy "..\%NRI_DIR%\LICENSE.txt" "."

cd ..
