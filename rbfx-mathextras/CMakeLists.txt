cmake_minimum_required (VERSION 3.4.3)

add_library(rbfx-mathextras MathExtras.h MathExtras.cpp MathExtras_DSP/MathExtras_DSP.h MathExtras_DSP/MathExtras_DSP_Generators.h MathExtras_DSP/MathExtras_Curves.h)

target_compile_definitions(rbfx-mathextras PUBLIC URHOMATHEXTRAS_EXPORTS)

target_include_directories(rbfx-mathextras PUBLIC ThirdParty/Eigen)

target_include_directories(rbfx-mathextras PUBLIC . )

target_link_libraries(rbfx-mathextras Urho3D)
