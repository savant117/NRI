@echo off

cd "_Compiler\Android\x86_64\Debug"
C:\Android\Sdk\cmake\3.22.1\bin\ninja.exe
cd "..\Release"
C:\Android\Sdk\cmake\3.22.1\bin\ninja.exe
cd "..\..\arm64-v8a\Debug"
C:\Android\Sdk\cmake\3.22.1\bin\ninja.exe
cd "..\Release"
C:\Android\Sdk\cmake\3.22.1\bin\ninja.exe
cd "..\..\..\.."