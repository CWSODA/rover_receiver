cmake_minimum_required(VERSION 3.10)

project(ROVER_RECEIVER)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include ImGui as subdir
add_subdirectory(imgui)

# Include ImPlot as subdir
add_subdirectory(implot)

# Add glad as lib
add_library(glad src/third_party/glad.c)

# Find GLFW
find_package(glfw3 REQUIRED)

set(TARGET_NAME rover_receiver)

# file(GLOB_RECURSE SRC CONFIGURE_DEPENDS
#     src/*.cpp
# )

# app is the executable
add_executable(${TARGET_NAME} 
    src/main.cpp
    src/gui.cpp
    src/lidar_drawer.cpp
    src/parser.cpp
    src/serial_port.cpp
    src/log.cpp
    src/lidar.cpp
)

# adds run function
add_custom_target(run
    COMMAND ${TARGET_NAME} 
    DEPENDS ${TARGET_NAME} 
)

include_directories(${TARGET_NAME}  PRIVATE include)

# Link libs
target_link_libraries(${TARGET_NAME}  
    PRIVATE
    glad 
    glfw 
    imgui
    implot
)

target_include_directories(imgui PRIVATE /opt/homebrew/include)
target_include_directories(implot PRIVATE /opt/homebrew/include)