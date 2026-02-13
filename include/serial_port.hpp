#pragma once

#include <boost/asio.hpp>
using namespace boost;

#include <string>
#include <mutex>
#include <thread>

// constexpr uint UART_BAUDRATE = 115200 * 8;
constexpr unsigned long UART_BAUDRATE = 115200;

class SerialPort {
   public:
    // disallow copy or assign
    // each serial port is unique
    SerialPort(const SerialPort&) = delete;
    SerialPort& operator=(const SerialPort&) = delete;

    void config_terminal(unsigned long baudrate = UART_BAUDRATE);

    SerialPort(const std::string& port_name);

    // dummy uninitialized serial port for debugging
    SerialPort() { start_thread(); }

    // opens new port
    // closes old port if new port is availible
    // throws runtime exception if failed
    void open(const std::string& path);

    // blocking read, reads full buffer
    void read();

    // continuously reads serial port input
    // blocks the thread, run in seperate thread!
    void start_thread() {
        thread_ = std::thread([this] {
            io_ctx_.restart();
            io_ctx_.run();
        });
    }
    // void start_async_read();
    void stop_async_read();

    // returns async read buffer
    std::string pull_buffer();

    // blocking, writes entire message
    void write(std::string& msg);
    void write_byte(uint8_t c);
    void write_bytes(uint8_t* data, size_t n);

    ~SerialPort() {
        if (serial_port_.is_open()) {
            serial_port_.cancel();
            serial_port_.close();
        }
        work_guard_.reset();
        io_ctx_.stop();
        if (thread_.joinable()) thread_.join();
    }

   private:
    asio::io_context io_ctx_;
    asio::serial_port serial_port_ = asio::serial_port(io_ctx_);
    asio::executor_work_guard<asio::io_context::executor_type> work_guard_ =
        asio::make_work_guard(io_ctx_);

    std::thread thread_;

    std::mutex rx_mtx_;
    std::string rx_buffer_;

    std::string input_buffer_ = std::string(1024, '\0');
    asio::mutable_buffer asio_buffer_ =
        asio::buffer(input_buffer_.data(), input_buffer_.size());

    void async_read();
};

// stuff to help send data
enum class DacOption {
    VOLTAGE = 'V',
    kP = 'P',
    kI = 'I',
    kD = 'D',
};

inline void set_data(DacOption type, float val, SerialPort& sp) {
    // send opcode
    sp.write_byte(static_cast<uint8_t>(type));
    sp.write_bytes((uint8_t*)(&val), sizeof(float));
}