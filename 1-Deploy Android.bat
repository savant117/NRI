@echo off

C:\Android\Sdk\cmake\3.22.1\bin\cmake.exe -DCMAKE_TOOLCHAIN_FILE="C:\Android\Sdk\ndk\25.2.9519653\build\cmake\android.toolchain.cmake" -DCMAKE_BUILD_TYPE=Debug -DANDROID_ABI=x86_64 -DANDROID_PLATFORM=android-24 -DCMAKE_MAKE_PROGRAM="C:\Android\Sdk\cmake\3.22.1\bin\ninja.exe" -DCMAKE_SYSTEM_NAME=Android -S . -B _Compiler\Android\x86_64\Debug\ -G Ninja

C:\Android\Sdk\cmake\3.22.1\bin\cmake.exe -DCMAKE_TOOLCHAIN_FILE="C:\Android\Sdk\ndk\25.2.9519653\build\cmake\android.toolchain.cmake" -DCMAKE_BUILD_TYPE=Release -DANDROID_ABI=x86_64 -DANDROID_PLATFORM=android-24 -DCMAKE_MAKE_PROGRAM="C:\Android\Sdk\cmake\3.22.1\bin\ninja.exe" -DCMAKE_SYSTEM_NAME=Android -S . -B _Compiler\Android\x86_64\Release\ -G Ninja

C:\Android\Sdk\cmake\3.22.1\bin\cmake.exe -DCMAKE_TOOLCHAIN_FILE="C:\Android\Sdk\ndk\25.2.9519653\build\cmake\android.toolchain.cmake" -DCMAKE_BUILD_TYPE=Debug -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-24 -DCMAKE_MAKE_PROGRAM="C:\Android\Sdk\cmake\3.22.1\bin\ninja.exe" -DCMAKE_SYSTEM_NAME=Android -S . -B _Compiler\Android\arm64-v8a\Debug\ -G Ninja

C:\Android\Sdk\cmake\3.22.1\bin\cmake.exe -DCMAKE_TOOLCHAIN_FILE="C:\Android\Sdk\ndk\25.2.9519653\build\cmake\android.toolchain.cmake" -DCMAKE_BUILD_TYPE=Release -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-24 -DCMAKE_MAKE_PROGRAM="C:\Android\Sdk\cmake\3.22.1\bin\ninja.exe" -DCMAKE_SYSTEM_NAME=Android -S . -B _Compiler\Android\arm64-v8a\Release\ -G Ninja
