#include <string>
#include <iostream>

#include "serial_port.hpp"
#include "log.hpp"
#include "gui.hpp"
#include "lib.hpp"

#define TODO exit(-1);

// Basically a state machine
enum class CmdType { None, Awaiting, L, T };
class Cmd {
   public:
    CmdType type = CmdType::None;
    bool is_none() { return type == CmdType::None; }
    bool is_await() { return type == CmdType::Awaiting; }

    LogOption type_to_log() {
        switch (type) {
            case (CmdType::L):
                return LogOption::L;
                break;
            case (CmdType::T):
                return LogOption::T;
                break;
            default:
                return LogOption::Plain;
        }
        return LogOption::Plain;
    }

    // sets type given char
    // returns true if successful, false if invalid
    bool set_type(char c) {
        switch (c) {
            case ('L'):
                type = CmdType::L;
                break;
            case ('T'): {  // used for timing
                type = CmdType::T;
                break;
            }
            default:
                return false;
        }
        return true;
    }
};

void parse(GUIData& gui_data) {
    static std::string rx_buffer;
    std::string incoming_data = gui_data.sp.pull_buffer();
    rx_buffer += incoming_data;
    // rx_buffer = "$L3....ewrf";
    if (rx_buffer.size() == 0) return;

    // start parsing inputs
    size_t start = 0;
    Cmd curr_cmd;
    for (size_t i = 0; i < rx_buffer.size(); i++) {
        char c = rx_buffer.at(i);

        if (curr_cmd.is_none()) {
            if (c != '$') {
                continue;
            }

            // c == '$'
            if (i != 0) {
                gui_data.gui_log.log_plain(rx_buffer, start, i - 1);
            }
            // await command opcode
            curr_cmd.type = CmdType::Awaiting;
            start = i;
            continue;
        }

        // currently expecting command values
        // await is right after $
        if (curr_cmd.is_await()) {
            if (!curr_cmd.set_type(c)) {
                // invalid command
                std::cout << "Warning: Invalid opcode: " << c << "!\n";
                curr_cmd.type = CmdType::None;
                gui_data.gui_log.append_log(
                    rx_buffer.substr(start, i - start + 1), LogOption::Warning);
                start = i + 1;
            }
            if (curr_cmd.type == CmdType::T) {
                gui_data.frame_count++;
                gui_data.gui_log.append_log("", LogOption::T);
                curr_cmd.type = CmdType::None;
                start = i + 1;
            }
            continue;
        }

        // otherwise in command

        // ignore
        // if (curr_cmd.type == CmdType::A) {
        //     // read char as int value
        //     gui_data.ADC_value = static_cast<int>(rx_buffer.at(i));
        //     // add ADC_value to graph
        //     static float t = 0.0f;
        //     float voltage = uint8_to_v(gui_data.ADC_value);
        //     gui_data.graph.add_point(t, voltage);
        //     t += 1.0f;

        //     std::string s = rx_buffer.substr(start, i - start + 1);
        //     gui_data.gui_log.append_log(s, curr_cmd.type_to_log());
        //     start = i + 1;
        //     curr_cmd.type = CmdType::None;
        //     continue;
        // }

        // Lidar data point
        // opcode L (lidar)
        // signal strength (single byte unsigned)
        // distance (4 byte raw float)
        // angle (4 byte raw float)
        // 1 + 4 + 4 = 9 data bytes, 11 bytes total
        if (curr_cmd.type == CmdType::L) {
            size_t total_bytes = 11;
            // required bytes
            if (rx_buffer.size() - start < total_bytes) {
                // not enough bytes
                // printf("Broken\n");
                break;
            }
            // all bytes present
            std::string sub_str = rx_buffer.substr(start, total_bytes);
            gui_data.gui_log.append_log(sub_str, curr_cmd.type_to_log());

            // get values
            // float values on 4th and 8th bytes
            uint8_t sig_str = rx_buffer.at(i);
            float distance, angle;
            memcpy(&distance, &rx_buffer.at(start + 3), sizeof(float));
            memcpy(&angle, &rx_buffer.at(start + 7), sizeof(float));

            // store values
            // printf("Strength: %d\n", static_cast<int>(sig_str));
            // printf("Distance: %f\n", distance);
            // printf("Angle: %f\n", angle);
            gui_data.lidar_drawer.add_point(sig_str, distance, angle);

            // $L(byte)(float)(float)
            // (0)(1)(2)(3456)(789x)
            // i == 2 ^
            i += total_bytes - 3;
            start = i + 1;
            curr_cmd.type = CmdType::None;
            continue;
        }
    }

    if (curr_cmd.is_none()) {
        gui_data.gui_log.log_plain(rx_buffer, start, rx_buffer.size() - 1);
        rx_buffer.clear();
    } else {
        // save incomplete command
        rx_buffer = rx_buffer.substr(start, rx_buffer.size() - start);
    }
}