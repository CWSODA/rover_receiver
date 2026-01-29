#include "graph.hpp"

void Graph::plot() {
    static ImPlotAxisFlags axis_flags = 0;
    static ImPlotLineFlags line_flags = 0;

    static bool is_lock_x = true;
    static float x_win_size = 1000.0f;

    static float y_min = -0.2f;
    static float y_max = 3.5f;
    static bool is_update_y = true;

    if (ImPlot::BeginPlot("ADC Readings")) {
        ImPlot::SetupAxes("Time (Samples)", "Voltage (V)", axis_flags,
                          axis_flags);

        // Axis, min, max
        if (is_lock_x) {
            ImPlot::SetupAxisLimits(ImAxis_X1, t_data_.size() - x_win_size,
                                    t_data_.size(), ImPlotCond_Always);
        }

        if (is_update_y) {
            ImPlot::SetupAxisLimits(ImAxis_Y1, y_min, y_max, ImPlotCond_Always);
            is_update_y = false;
        }

        if (!t_data_.empty()) {
            // label, x-data, y-data, count
            ImPlot::PlotLine<float>("Voltage", t_data_.data(), v_data_.data(),
                                    t_data_.size(), line_flags, 0,
                                    sizeof(float));
        }

        ImPlot::EndPlot();
    }

    // X-Axis options
    ImGui::TextUnformatted("X-Axis Window Size: ");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(300.0f);
    ImGui::DragFloat("##x_win_size", &x_win_size, 1.0f, 1.0f, FLT_MAX, "%.0f",
                     ImGuiSliderFlags_AlwaysClamp);
    ImGui::SameLine(0.0f, 36.0f);
    ImGui::Checkbox("Lock X-Axis", &is_lock_x);

    // Y-Axis options
    // min
    ImGui::TextUnformatted("Y-Axis MIN: ");
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::SetNextItemWidth(100.0f);
    if (ImGui::DragFloat("##y_min", &y_min, 0.01f, -FLT_MAX, FLT_MAX, "%.2f")) {
        y_min = std::min(y_min, y_max - 0.01f);
    }
    // max
    ImGui::SameLine(0.0f, 50.0f);
    ImGui::TextUnformatted("Y-Axis MAX: ");
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::SetNextItemWidth(100.0f);
    if (ImGui::DragFloat("##y_max", &y_max, 0.01f, -FLT_MAX, FLT_MAX, "%.2f")) {
        y_max = std::max(y_max, y_min + 0.01f);
    }
    // set button
    ImGui::SameLine(0.0f, 50.0f);
    if (ImGui::Button("Set Y-Axis bounds")) {
        is_update_y = true;
    }
}

void Graph::add_point(float t, float v) {
    t_data_.push_back(t);
    v_data_.push_back(v);
}