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
-DNEWTON_ENABLE_AVX2_SOLVER=ON \
-DNEWTON_DOUBLE_PRECISION=ON \
-DNEWTON_BUILD_SHARED_LIBS=ON \
-DNEWTON_BUILD_SANDBOX_DEMOS=ON \
-DNEWTON_BUILD_TEST=ON \
-DNEWTON_BUILD_CREATE_SUB_PROJECTS=OFF
