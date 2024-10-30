#!/bin/bash

mkdir -p build
cd build 
rm -f -r *


cmake .. -G "Visual Studio 17 2022" \
-DURHO3D_CSHARP=OFF \
-DURHO3D_MONOLITHIC_HEADER=ON \
-DURHO3D_SAMPLES=ON \
-DURHO3D_WIN32_CONSOLE=ON \
-DURHO3D_WEBP=OFF \
-DURHO3D_NAVIGATION=OFF \
-DURHO3D_URHO2D=OFF \
-DURHO3D_NETWORK=ON \
-DURHO3D_PROFILING=OFF \
-DNEWTON_WITH_AVX2_PLUGIN=ON \
-DNEWTON_DOUBLE_PRECISION=ON \
-DOPENVR_BUILD_SHARED=ON
