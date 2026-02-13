#pragma once

#include "gui.hpp"
#include "serial_port.hpp"
#include <string>

class Parser {
   public:
    void parse(GUIData& gui_data);
    void clear_rx_buffer() { rx_buffer.clear(); }

   private:
    std::string rx_buffer;
};
