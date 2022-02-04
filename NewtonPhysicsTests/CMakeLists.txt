if (NOT URHO3D_WIN32_CONSOLE)
    set (TARGET_TYPE WIN32)
endif ()



file (GLOB_RECURSE SOURCE_FILES *.cpp *.h)
group_sources()



if (NOT URHO3D_WIN32_CONSOLE)
    set (TARGET_TYPE WIN32)
endif ()

# Define executable name.
add_executable(NewtonPhysicsTests ${TARGET_TYPE} ${SOURCE_FILES})
target_link_libraries(NewtonPhysicsTests PUBLIC rbfx-newton rbfx-visualdebugger rbfx-mathextras Urho3D)






