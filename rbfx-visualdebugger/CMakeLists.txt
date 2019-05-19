cmake_minimum_required (VERSION 3.4.3)

add_library(rbfx-visualdebugger SHARED VisualDebugger.h VisualDebugger.cpp)

target_compile_definitions(rbfx-visualdebugger PUBLIC URHOVISUALDEBUGGER_EXPORTS)

target_include_directories(rbfx-visualdebugger PUBLIC . )

target_link_libraries(rbfx-visualdebugger Urho3D)
