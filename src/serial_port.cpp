#include "serial_port.hpp"

#include <boost/asio.hpp>
using namespace boost;

// terminal config stuff
#include <termios.h>
#ifdef __APPLE__  // apple specific ioctl
#include <sys/ioctl.h>
#include <IOKit/serial/ioss.h>
#endif

#include <string>
#include <iostream>
#include <mutex>

// #define DEBUG_UART
// #define DEBUG_UART_HEX

void SerialPort::config_terminal(unsigned long baudrate) {
    // file descriptor, used to configure port
    int fd = serial_port_.native_handle();
    struct termios tty;

    // make port read raw bytes
    int t = tcgetattr(fd, &tty);
    cfmakeraw(&tty);
    tcsetattr(fd, TCSANOW, &tty);

// set baudrate (ONLY WORKS FOR MACOS)
#ifdef __APPLE__
    speed_t baudrate_speed = baudrate;
    ioctl(fd, IOSSIOSPEED, &baudrate_speed);
#else
    CRASHOUT !!!USE A COMPATIBLE LIBRARY TO SET BAUDRATE
#endif
}

SerialPort::SerialPort(const std::string& port_name) {
    serial_port_.open(port_name);

    if (!serial_port_.is_open()) {
        throw std::runtime_error("Unable to open serial port at: " + port_name);
    }
    config_terminal();

    system::error_code ec;
    // serial_port_.set_option(asio::serial_port_base::baud_rate(BAUD_RATE),
    // ec);
    serial_port_.set_option(asio::serial_port_base::character_size(8), ec);
    serial_port_.set_option(asio::serial_port_base::stop_bits(
                                asio::serial_port_base::stop_bits::one),
                            ec);
    serial_port_.set_option(
        asio::serial_port_base::parity(asio::serial_port_base::parity::none),
        ec);
    serial_port_.set_option(asio::serial_port_base::flow_control(
                                asio::serial_port_base::flow_control::none),
                            ec);

    if (ec) {
        throw std::runtime_error("Unable to setup serial port at: " +
                                 port_name);
    }

    thread_ = std::thread([this] {
        this->async_read();
        io_ctx_.restart();
        io_ctx_.run();
    });
}

void SerialPort::open(const std::string& path) {
    asio::serial_port new_port(io_ctx_);
    new_port.open(path);

    if (!new_port.is_open()) {
        throw std::runtime_error("Unable to open serial port at: " + path);
    }
    config_terminal();

    system::error_code ec;
    // new_port.set_option(asio::serial_port_base::baud_rate(BAUD_RATE), ec);
    new_port.set_option(asio::serial_port_base::character_size(8), ec);
    new_port.set_option(asio::serial_port_base::stop_bits(
                            asio::serial_port_base::stop_bits::one),
                        ec);
    new_port.set_option(
        asio::serial_port_base::parity(asio::serial_port_base::parity::none),
        ec);
    new_port.set_option(asio::serial_port_base::flow_control(
                            asio::serial_port_base::flow_control::none),
                        ec);

    if (ec) {
        // throw std::runtime_error("Unable to setup serial port at: " + path);
        throw std::runtime_error(ec.what());
    }

    // successfully opened port
    if (serial_port_.is_open()) {
        serial_port_.cancel();
        serial_port_.close();
    }
    serial_port_ = std::move(new_port);
    async_read();
}

void SerialPort::read() {
    system::error_code ec;
    int bytes_read = serial_port_.read_some(asio_buffer_, ec);

    if (!ec) std::cout.write(input_buffer_.data(), bytes_read);
}

void SerialPort::write(std::string& msg) {
    system::error_code ec;
    serial_port_.write_some(asio::buffer(msg.data(), msg.size()), ec);

    if (ec) {
        std::cout << "WARNING: Failed to write to serial port!\n";
    }
}

void SerialPort::write_byte(uint8_t c) {
    system::error_code ec;
    asio::write(serial_port_, asio::buffer(&c, 1), ec);

    if (ec) {
        std::cout << "WARNING: Failed to write byte to serial port!\n";
    }
}

void SerialPort::write_bytes(uint8_t* data, size_t n) {
    system::error_code ec;
    asio::write(serial_port_, asio::buffer(data, n), ec);

    // debugging float sending
    printf("Writing Bytes: %02X %02X %02X %02X\n", data[0], data[1], data[2],
           data[3]);

    if (ec) {
        std::cout << "WARNING: Failed to write bytes to serial port!\n";
    }
}

std::string SerialPort::pull_buffer() {
    std::string output;
    std::lock_guard<std::mutex> lock(rx_mtx_);
    if (rx_buffer_.empty()) return "";

    std::swap(output, rx_buffer_);

    return output;
}

void SerialPort::stop_async_read() { serial_port_.cancel(); }

void SerialPort::async_read() {
    serial_port_.async_read_some(
        asio_buffer_, [this](const system::error_code& ec, size_t bytes_read) {
            if (!ec) {
                std::lock_guard<std::mutex> lock(rx_mtx_);
                rx_buffer_.append(input_buffer_, 0, bytes_read);

#ifdef DEBUG_UART
                std::cout.write(input_buffer_.c_str(), bytes_read);
                std::cout.flush();
#endif
#ifdef DEBUG_UART_HEX
                for (unsigned char c : input_buffer_.substr(0, bytes_read)) {
                    printf("(%02X)\n", c);
                }
#endif

                this->async_read();
            } else {
                std::cout << "Async read error: " << ec.message() << "("
                          << ec.value() << ")\n";
            }
        });
}