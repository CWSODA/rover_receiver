#include <iostream>
#include <string>

#include "gui.hpp"
#include "lib.hpp"

// test includes
// remove on release
#include "parser.hpp"

int main() {
    std::cout << "Starting Program..." << std::endl;

    try {
        run_gui();
    } catch (std::runtime_error e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}