#pragma once

/*
Draws lidar code to a texture that gets passed to ImGui
Store data points and draws them as points
Discard points after cooldown, dimming them over delta
*/

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

// IMGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <vector>

#include "shader.hpp"

constexpr float PI = 3.1415926535897932384f;

// drawer settings
constexpr int DRAWER_WIDTH = 550;
constexpr int DRAWER_HEIGHT = 550;
constexpr float LIFETIME_SECONDS = 1.0f;
constexpr size_t GRID_CIRCLE_DIVS = 64;

struct GUIData;

struct LidarPoint {
    uint8_t strength;
    glm::vec2 pos;
    float point_angle;

    // dictates point opacity and deletion
    float lifetime;

    // constructor calculates xy position from distance+angle
    // angle in degrees
    LidarPoint(uint8_t strength, float distance, float angle, float lifetime) {
        this->strength = strength;
        float angle_in_rad = angle * PI / 180.0f;
        pos.x = cosf(angle_in_rad) * distance;
        pos.y = sinf(angle_in_rad) * distance;
        // normalize angle
        point_angle = angle - 360.0f * (int)(angle / 360.0f);

        this->lifetime = lifetime;
    }
};

class LidarDrawer {
   public:
    int strength_threshold = 0;  // must be within one byte
    float distance_threshold = 12.0f;
    bool is_snapshot = false;  // set true to disable decay and intake
    LidarDrawer();

    // render loop
    void render(GUIData& gui_data);

    // clears texture
    void reset() {
        glBindFramebuffer(GL_FRAMEBUFFER, ld_fbo);
        glViewport(0, 0, DRAWER_WIDTH, DRAWER_HEIGHT);
        float greyscale_val = 0.0f;
        glClearColor(greyscale_val, greyscale_val, greyscale_val, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    GLuint get_texture() const { return ld_color_tex; }

    // adds points to be rendered, distance in m, angle in degrees
    void add_point(uint8_t strength, float distance, float angle,
                   float lifetime = LIFETIME_SECONDS) {
        ++this->sample_count;
        if (is_snapshot) return;
        if (strength < strength_threshold) return;  // ignore weak signals
        if (distance > distance_threshold) return;  // ignore invalid dist
        points.emplace_back(LidarPoint(strength, distance, angle, lifetime));
    }

    uint64_t get_sample_count() const { return sample_count; }

    void gen_test_points() {
        // generate spiral data
        add_point(1, 0, 0);
        for (int x = 1; x <= 10; x++) {
            add_point(1, x, x * 45.0f, LIFETIME_SECONDS + x);
        }
    }

    void print_points() {
        for (const auto& point : points) {
            printf("(%f, %f)\n", point.pos.x, point.pos.y);
        }
    }

   private:
    // Render stuff
    // lidar drawer (ld) framebuffer obj and color texture
    GLuint ld_fbo, ld_color_tex;
    Shader dot_shader = Shader("src/shaders/vert/zoom_dot.vert",
                               "src/shaders/frag/colored_dot.frag");

    std::vector<LidarPoint> points;

    // grid lines
    unsigned int grid_VAO, grid_VBO;
    Shader grid_shader =
        Shader("src/shaders/vert/circle.vert", "src/shaders/frag/colored.frag");

    // testing stuff
    u_int64_t sample_count = 0;  // lifetime sample count
};