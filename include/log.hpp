#pragma once

#include <string>

enum class LogOption { Incomplete, Plain, Warning, L };

class GUILog {
   public:
    const char* get_printable_pointer() { return printable.c_str(); }
    void append_log(const std::string& other, LogOption op);

    void log_plain(std::string& buf, size_t start, size_t end) {
        if (start <= end) {
            std::string s = buf.substr(start, end - start + 1);
            append_log(s, LogOption::Plain);
        }
    }

    void clear_log() {
        printable.clear();
        raw.clear();
    }

    void export_log(const char* filename);

   private:
    std::string raw;
    std::string printable;
};
