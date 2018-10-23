cd C:\sd2\visual_studio\projects\sd2

cmake %1 %2 %3 CMakeLists.txt && msbuild Project.sln /p:Configuration=Debug /p:Platform="Win32"