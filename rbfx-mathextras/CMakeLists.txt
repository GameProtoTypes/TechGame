cmake_minimum_required (VERSION 3.4.3)

add_library(rbfx-mathextras SHARED MathExtras.h MathExtras.cpp)

target_compile_definitions(rbfx-mathextras PUBLIC URHOMATHEXTRAS_EXPORTS)

target_include_directories(rbfx-mathextras PRIVATE ThirdParty/Eigen)

target_include_directories(rbfx-mathextras PUBLIC . )

target_link_libraries(rbfx-mathextras Urho3D)
