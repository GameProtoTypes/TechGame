#!/bin/bash

mkdir -p build
cd build 
#rm -f -r *

#cmake .. -G "Visual Studio 16 2019" -A x64  \
cmake .. -G "Visual Studio 15 2017 Win64"  \
-DBUILD_SHARED_LIBS=ON \
-DURHO3D_CSHARP=OFF \
-DURHO3D_MONOLITHIC_HEADER=ON \
-DURHO3D_SAMPLES=ON \
-DURHO3D_WIN32_CONSOLE=ON \
-DURHO3D_WEBP=OFF \
-DURHO3D_NAVIGATION=OFF \
-DURHO3D_URHO2D=OFF \
-DURHO3D_NETWORK=ON \
-DURHO3D_PROFILING=ON \
-DNEWTON_WITH_AVX2_PLUGIN=ON \
-DNEWTON_DOUBLE_PRECISION=ON \
-DCMAKE_PREFIX_PATH=C:/Users/casht/repos/TechGame/rbfx-torch/libtorch