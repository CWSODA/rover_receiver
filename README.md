# Brief
A real-time OpenGL-based GUI debugging tool for visualising rover sensor data over UART. \
Designed to accept connection with C232HM cable but any UART cable should work. \
Requires compatile Pico microcontroller flashed with program from https://github.com/CWSODA/rover_w 

# OS Notes
The program was developed on Apple Silicon and not tested on other operating systems. \
UART baudrate needs to be set higher than 115,200 (currently only implemented for macOS). 

# Features
* Displays distance data graphically
* Snapshotting of rendered data
* Slider to set strength threshold of signal
* Displays debugging values: sample and frame rate
* Shows UART log from microcontroller
* Automatic port detection and select

# Dependencies
* GLFW3
* glad (included)
* glm (included)
* ImGUI + ImPlot (included)
* OpenGL (core 3.3)

# Build Instructions
Install GLFW3 and CMake. \
Configure CMake and build. \
OR optionally, use the commands in the makefile to build and run faster
