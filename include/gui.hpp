#pragma once

#include "imgui.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// SELF HEADERS
#include "serial_port.hpp"
#include "graph.hpp"
#include "log.hpp"
#include "lidar_drawer.hpp"

const unsigned int WIN_WIDTH = 800;
const unsigned int WIN_HEIGHT = 750;

// struct to pass to all functions
struct GUIData {
    float prev_time = 0.0f;
    float delta_time;

    int ADC_value = 0;

    // slider values
    float s_target_voltage = 0.0f;
    float s_kP = 0.0f;
    float s_kI = 0.0f;
    float s_kD = 0.0f;

    // actual set values
    // only change after receiving il matto confirmations
    float target_voltage = 0.0f;
    float kP = 0.0f;
    float kI = 0.0f;
    float kD = 0.0f;

    GUILog gui_log;
    SerialPort sp;

    // drawer stuff
    LidarDrawer lidar_drawer;
    float drawer_zoom = 8.5f;

    // testing stuff
    uint64_t frame_count = 0;

    GLFWwindow* window;
};

const float HEADER_FONT_SIZE = 20.0f;
const ImVec4 HEADER_COLOR = ImVec4(0.5f, 0.5f, 1.0f, 1.0f);

void run_gui();

// returns true if word processed or not a command
// returns false if command is not complete
bool process_word(std::string& input);

void make_header(const char* msg);

// make sure actual value is not changed, only change slider value (s_val)
void make_PID_slider(const char* label, float* s_val, const float val,
                     DacOption dac_opt, SerialPort& sp);

void port_select(GUIData& gui_data);