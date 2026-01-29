#include "lidar_drawer.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>

// IMGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "gui.hpp"

LidarDrawer::LidarDrawer() {
    // FRAME BUFFERS
    glGenFramebuffers(1, &ld_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, ld_fbo);

    // Color texture
    glGenTextures(1, &ld_color_tex);
    glBindTexture(GL_TEXTURE_2D, ld_color_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, DRAWER_WIDTH, DRAWER_HEIGHT, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           ld_color_tex, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("FBO NOT COMPLETE!\n");

    // start screen as black
    this->reset();

    // setup grid VAO
    // generate circle
    // 2 floats per position
    float grid_verts[2 * GRID_CIRCLE_DIVS];
    for (int x = 0; x < GRID_CIRCLE_DIVS; x++) {
        float angle = static_cast<float>(x) * 2 * PI / GRID_CIRCLE_DIVS;
        grid_verts[2 * x] = std::cosf(angle);
        // grid_verts[2 * x + 1] = x / 10.0f;
        grid_verts[2 * x + 1] = 0;
        grid_verts[2 * x + 1] = std::sinf(angle);

        // printf("Angle(%.3f), (%.3f, %.3f)\n", angle, grid_verts[2 * x],
        //        grid_verts[2 * x + 1]);
    }
    glGenVertexArrays(1, &grid_VAO);
    glBindVertexArray(grid_VAO);
    glGenBuffers(1, &grid_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, grid_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(grid_verts), grid_verts,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                          (void*)(0));
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void LidarDrawer::render(GUIData& gui_data) {
    // bind framebuffer and clear
    glBindFramebuffer(GL_FRAMEBUFFER, ld_fbo);
    glViewport(0, 0, DRAWER_WIDTH, DRAWER_HEIGHT);
    glm::vec3 clear_color(0.1f, 0.1f, 0.1f);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw circular grid (polar)
    // keep VAO as dummy for drawing points
    glBindVertexArray(grid_VAO);
    grid_shader.use();
    grid_shader.set_vec3("a_color", glm::vec3(0.3f));
    grid_shader.set_float("a_zoom", gui_data.drawer_zoom);
    for (int x = 1; x <= 8; x++) {
        grid_shader.set_float("a_radius", x);
        glDrawArrays(GL_LINE_LOOP, 0, GRID_CIRCLE_DIVS);
    }

    // set shared uniforms
    glPointSize(10.0f);
    dot_shader.use();
    dot_shader.set_float("a_zoom", gui_data.drawer_zoom);

    // render points
    for (auto& point : points) {
        glm::vec3 point_color =
            glm::mix(clear_color, glm::vec3(1.0f, 0.0f, 0.0f), point.lifetime);
        dot_shader.set_vec2("a_pos", point.pos);
        dot_shader.set_vec3("a_color", point_color);
        glDrawArrays(GL_POINTS, 0, 1);

        point.lifetime -= gui_data.delta_time;
    }

    // erase points with expired lifetimes
    points.erase(
        std::remove_if(points.begin(), points.end(),
                       [](LidarPoint point) { return point.lifetime <= 0.0f; }),
        points.end());

    // unbind to default
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}