@echo off

C:\Android\Sdk\cmake\3.22.1\bin\cmake.exe -DCMAKE_TOOLCHAIN_FILE="C:\Android\Sdk\ndk\25.2.9519653\build\cmake\android.toolchain.cmake" -DCMAKE_BUILD_TYPE=Debug -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-24 -DCMAKE_MAKE_PROGRAM="C:\Android\Sdk\cmake\3.22.1\bin\ninja.exe" -DCMAKE_SYSTEM_NAME=Android -S . -B _Compiler\Debug\ -G Ninja

C:\Android\Sdk\cmake\3.22.1\bin\cmake.exe -DCMAKE_TOOLCHAIN_FILE="C:\Android\Sdk\ndk\25.2.9519653\build\cmake\android.toolchain.cmake" -DCMAKE_BUILD_TYPE=Release -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-24 -DCMAKE_MAKE_PROGRAM="C:\Android\Sdk\cmake\3.22.1\bin\ninja.exe" -DCMAKE_SYSTEM_NAME=Android -S . -B _Compiler\Release\ -G Ninja