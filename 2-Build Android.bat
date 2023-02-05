@echo off

cd "_Compiler\Debug"
C:\Android\Sdk\cmake\3.22.1\bin\ninja.exe
cd "..\Release"
C:\Android\Sdk\cmake\3.22.1\bin\ninja.exe
cd ..\..
