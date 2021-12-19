cmake_minimum_required (VERSION 3.4.3)

# Name your project. Can be anything.
project (TechGame)


# Minimal supported C++ version.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Set binary output directories. Changing these paths will require you adjusting your resource path configuration!
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)



file (GLOB_RECURSE SOURCE_FILES *.cpp *.h)

group_sources()


if (NOT URHO3D_WIN32_CONSOLE)
    set (TARGET_TYPE WIN32)
endif ()

# Define executable name.
add_executable(TechGame ${TARGET_TYPE} ${SOURCE_FILES})




# Link to libraries.
target_link_libraries(TechGame Urho3D rbfx-mathextras rbfx-newton rbfx-visualdebugger)

