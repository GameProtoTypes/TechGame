
cmake_minimum_required(VERSION 3.4.3)


# Set project name
project (TechGameSolution)

# Minimal supported C++ version.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set binary output directories. Changing these paths will require you adjusting your resource path configuration!
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


set(URHO3D_PHYSICS2D OFF)

add_subdirectory("./rbfx")
add_subdirectory("./rbfx-newton")
add_subdirectory("./rbfx-mathextras")
add_subdirectory("./rbfx-visualdebugger")



#add_subdirectory("./rbfx-torch")
add_subdirectory("./TechGame")
add_subdirectory("./NewtonPhysicsTests")


#make the game project the main one for visual studio.
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT NewtonPhysicsTests)



