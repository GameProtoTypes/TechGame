
cmake_minimum_required(VERSION 3.4.3)


# Set project name
project (TechGameSolution)

# Set binary output directories. Changing these paths will require you adjusting your resource path configuration!
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory("./rbfx")
add_subdirectory("./rbfx-newton")
add_subdirectory("./rbfx-mathextras")
add_subdirectory("./rbfx-visualdebugger")
add_subdirectory("./TechGame")


#make the game project the main one for visual studio.
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TechGame)


