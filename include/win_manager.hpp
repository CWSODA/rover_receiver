#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "gui.hpp"

#define ZOOM_MIN 0.1f
#define ZOOM_MAX 20.0f
#define ZOOM_SCALER 0.05f

void process_input(GLFWwindow* window, GUIData& gui_data) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
    // change drawer size
    auto* gui_data = static_cast<GUIData*>(glfwGetWindowUserPointer(window));
    gui_data->drawer_zoom += -ZOOM_SCALER * y_offset;
    gui_data->drawer_zoom = std::max(gui_data->drawer_zoom, ZOOM_MIN);
    gui_data->drawer_zoom = std::min(gui_data->drawer_zoom, ZOOM_MAX);
}