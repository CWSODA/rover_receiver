#include "log.hpp"

#include <iostream>
#include <fstream>

void GUILog::append_log(const std::string& other, LogOption op) {
    // raw.append(other);
    // printf("log: %s\n", other.c_str());

    switch (op) {
        case LogOption::Plain:
            // ignore /0s
            for (char c : other) {
                if (c == '\0') continue;
                printable += c;
            }
            break;
        case LogOption::Incomplete:
            throw std::runtime_error("GUI log should not receive incomplete!");
            break;
        case LogOption::Warning: {
            // print all values as HEX
            for (unsigned char c : other) {
                char buf[16];
                snprintf(buf, 16, "(%02X)", c);
                printable += "WARNING: ";
                printable += buf;
            }
            printable += '\n';
        } break;
        case LogOption::L: {
            break;
            printable += "$L";
            // print the resulting values as HEX
            // 9 data bytes, u8 + 2 floats
            for (int x = 0; x < 9; x++) {
                char buf[8];
                snprintf(buf, 7, "(%02X)", (uint8_t)other.at(x + 2));
                printable += buf;

                // add seperation between values
                // 0|1234|5678
                if (x == 0 || x == 4) {
                    printable += '|';
                }
            }
            printable += '\n';
            break;
        }
    }

    // printf("printable: %s\n", printable.c_str());
}

void GUILog::export_log(const char* filename) {
    std::ofstream file(filename);
    file << this->printable;
    file.close();
}