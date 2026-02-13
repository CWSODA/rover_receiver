#include "gui.hpp"

// STD LIB
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <filesystem>

// GUI
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// IMGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// SELF HEADERS
#include "win_manager.hpp"
#include "shader.hpp"
#include "serial_port.hpp"
#include "graph.hpp"
#include "lib.hpp"
#include "log.hpp"
#include "parser.hpp"

#define TODO throw std::invalid_argument("TODO!");

const std::string default_port_name = "/dev/tty.usbserial-FTU7C2WR";

void run_gui() {
    if (!glfwInit()) throw std::runtime_error("Unable to start glfw!");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window =
        glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "DAC", NULL, NULL);
    if (!window) {
        const char* description;
        int code = glfwGetError(&description);
        printf("GLFW Error: %s (Code %d)\n", description, code);
        glfwTerminate();
        throw std::runtime_error("ERROR");
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Failed to initialize GLAD!");

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    gladLoadGL();
    /* END OF WINDOW CREATION */

    // Loop values
    GUIData gui_data;
    Parser parser;
    gui_data.window = window;
    try {
        gui_data.sp.open(default_port_name);
    } catch (std::runtime_error) {
        // do nothing, use unset serial port
        // let user select their own port
    }
    glfwSetWindowUserPointer(window, (void*)&gui_data);

    // gui_data.lidar_drawer.gen_test_points();
    // gui_data.lidar_drawer.print_points();

    /* SETUP IMGUI */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& imgui_io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    /* SETUP IMPLOT */
    ImPlot::CreateContext();

    // SETUP GEOMETRY

    // retina display is double the pixels
    glViewport(0, 0, WIN_WIDTH * 2, WIN_HEIGHT * 2);

    // clear buffer just before first loop
    gui_data.sp.pull_buffer();
    while (!glfwWindowShouldClose(window)) {
        // update delta time
        float current_time = glfwGetTime();
        gui_data.delta_time = current_time - gui_data.prev_time;
        gui_data.prev_time = current_time;

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        parser.parse(gui_data);

        // render the lidar points
        gui_data.lidar_drawer.render(gui_data);

        // IMGUI STUFF
        // new IMGUI frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(WIN_WIDTH, WIN_HEIGHT));

        if (ImGui::Begin(
                "ROVER", NULL,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoBringToFrontOnFocus |
                    ImGuiWindowFlags_NoNavFocus)) {
            ImGui::SetWindowFontScale(1.5f);

            ImGui::BeginTabBar("MENU");

            if (ImGui::BeginTabItem("MAIN")) {
                float offset = (ImGui::GetWindowSize().x - DRAWER_WIDTH) / 2;
                ImGui::SetCursorPosX(offset);
                ImGui::Image(
                    (ImTextureID)(intptr_t)gui_data.lidar_drawer.get_texture(),
                    ImVec2(DRAWER_WIDTH, DRAWER_HEIGHT));
                ImGui::SetNextItemWidth(200);
                ImGui::SliderInt("Signal Strength Threshold",
                                 &gui_data.lidar_drawer.strength_threshold, 0,
                                 255, "%d", ImGuiSliderFlags_AlwaysClamp);

                // calculate test data, time averaged by 1 sec
                // points per second and frames per second
                static float data_cd = 0.0f;
                static float pps = 0.0f;
                static float fps = 0.0f;
                static uint64_t last_sample_count = 0;
                static uint64_t last_frame_count = 0;
                data_cd += gui_data.delta_time;
                if (data_cd >= 1.0f) {
                    uint64_t cur_sample_count =
                        gui_data.lidar_drawer.get_sample_count();
                    pps = static_cast<float>(cur_sample_count -
                                             last_sample_count) /
                          data_cd;
                    last_sample_count = cur_sample_count;

                    uint64_t cur_frame_count = gui_data.frame_count;
                    fps =
                        static_cast<float>(cur_frame_count - last_frame_count) /
                        data_cd;
                    last_frame_count = cur_frame_count;

                    data_cd = 0.0f;  // reset cooldown
                }

                // DEBUG RATES
                ImGui::Text("Points per sec: %f", pps);
                float fps_offset = ImGui::GetItemRectSize().x + 100;
                ImGui::SameLine(fps_offset);
                ImGui::Text("Frames per sec: %f", fps);

                ImGui::Checkbox("Snapshot", &gui_data.lidar_drawer.is_snapshot);

                port_select(gui_data);

                ImGui::EndTabItem();
            }

            static bool auto_scroll = true;
            if (ImGui::BeginTabItem("LOG")) {
                float footer_height = ImGui::GetFrameHeightWithSpacing();
                ImVec2 child_size = ImVec2(0, -footer_height);

                if (ImGui::BeginChild("##CHILD", child_size)) {
                    ImGui::TextUnformatted("Output Log:");
                    ImGui::SetWindowFontScale(1.0f);
                    ImGui::Text("%s", gui_data.gui_log.get_printable_pointer());

                    if (auto_scroll) ImGui::SetScrollY(ImGui::GetScrollMaxY());

                    ImGui::EndChild();
                }
                if (ImGui::Button("CLEAR LOG")) {
                    gui_data.gui_log.clear_log();
                }
                ImGui::SameLine();
                if (ImGui::Button("EXPORT LOG")) {
                    gui_data.gui_log.export_log("log.txt");
                }
                ImGui::SameLine();
                ImGui::Checkbox("Auto-scroll", &auto_scroll);

                ImGui::SameLine();
                if (ImGui::Button("Clear RX Buffer")) {
                    parser.clear_rx_buffer();
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();

            ImGui::End();
        }

        // IMGUI RENDER
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwPollEvents();
        glfwSwapBuffers(window);
        process_input(window, gui_data);
    }

    // IMGUI Shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ImPlot::DestroyContext();

    glfwTerminate();
}

void make_header(const char* msg) {
    std::string text = msg;
    ImGui::PushFontSize(HEADER_FONT_SIZE);
    ImGui::TextColored(HEADER_COLOR, "%s", text.c_str());
    ImGui::PopFontSize();
}

void make_PID_slider(const char* label, float* s_val, const float val,
                     DacOption dac_opt, SerialPort& sp) {
    // a label/ID is required. Have name after ## to hide label
    std::string slider_name = "##";
    slider_name += label;
    std::string button_label = "Set ";
    button_label += label;

    ImGui::SetNextItemWidth(400);
    if (ImGui::SliderFloat(slider_name.c_str(), s_val, 0.0f, 1.0f, "%.4f")) {
        // make sure PID values are non-negative
        *s_val = std::max(*s_val, 0.0f);
    }
    ImGui::SetItemTooltip("hint: cmd+click to type in value!");

    ImGui::SameLine();
    if (ImGui::Button(button_label.c_str())) {
        // write to il matto
        std::cout << "Setting " << label << ": " << *s_val << std::endl;

        set_data(dac_opt, *s_val, sp);
    }

    ImGui::SameLine();
    ImGui::Text("Current value: %.4f", val);
}

void port_select(GUIData& gui_data) {
    namespace fs = std::filesystem;

    static std::string current_port = "Select a port!";
    static std::string failed_path;
    static float error_timer = 0.0f;

    ImGui::TextUnformatted("Port Select: ");
    ImGui::SameLine();
    if (ImGui::BeginCombo("##Port Select", current_port.c_str())) {
        for (auto& entry : fs::directory_iterator("/dev")) {
            std::string path = entry.path().string();
            if (path.find("tty.") != std::string::npos ||
                path.find("cu.") != std::string::npos) {
                bool is_selected = (path == current_port);

                if (ImGui::Selectable(path.c_str(), is_selected)) {
                    // selected port
                    try {
                        gui_data.sp.open(path);

                        current_port = path;
                    } catch (std::runtime_error e) {
                        // do nothing, keep original port
                        // show error msg
                        std::cerr << e.what() << std::endl;
                        failed_path = path;
                        error_timer = 5.0f;
                    }
                }
            }
        }

        ImGui::EndCombo();
    }
    if (error_timer > 0.0f) {
        ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, error_timer),
                           "Unable to open port: %s", failed_path.c_str());
        error_timer -= gui_data.delta_time;
    }
}