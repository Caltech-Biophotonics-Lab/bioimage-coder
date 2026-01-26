#pragma once
#include <array>
#include <chrono>
#include <cstdint>
#include <stdexcept>
#include <optional>

// Include this after stdexcept
#include <nonstd/span.hpp>

namespace hardware_drivers {

using std::chrono::milliseconds;
using namespace std::chrono_literals;

/** A mock Linux USB driver for instrument emulation. Also known as dry-run. */
class MockUSB {
   public:
    static constexpr bool is_mock = true;
    const uint8_t id{};

    std::array<uint8_t, 1024> buffer{};

    /** Mock camera id. */
    uint8_t cam_id{0x04};
    uint8_t led_id{0xEE};

    constexpr MockUSB(uint8_t usb_id) : id{usb_id} {}

    template <typename T>
    inline const T& decode(size_t offset = 0) const {
        // Assume little-endian
        return *reinterpret_cast<const T*>(buffer.data() + offset);
    }

    /** Simulates libusb_control_transfer(). */
    template <class Query>
    [[nodiscard]] Query control_read() const {
        // Simulates libusb_control_transfer().
        constexpr auto mock_control_transfer = [](uint16_t w_value, uint16_t w_index, uint8_t* data,
                                                  uint16_t length) {
            // Do nothing
        };
        static_assert(Query::dev_addr == 0);
        static_assert(Query::reg_addr > 0);

        Query query{};
        mock_control_transfer(Query::dev_addr, Query::reg_addr, reinterpret_cast<uint8_t*>(&query),
                              sizeof(query));
        return query;
    }

    template <class Command>
    [[nodiscard]] bool control_write(Command cmd) const {
        // Simulates libusb_control_transfer().
        constexpr auto mock_control_transfer = [](uint16_t w_value, uint16_t w_index, uint8_t* data,
                                                  uint16_t length) {
            // Do nothing.
        };

        if constexpr (Command::dev_addr == 0) {
            // FPGA native commands
            mock_control_transfer(Command::dev_addr, Command::reg_addr,
                                  reinterpret_cast<uint8_t*>(&cmd), sizeof(Command));
        } else {
            // i2c-over-FPGA commands, broadcasted to the CMOS sensors.
            static_assert(sizeof(cmd.value) == 1);
            mock_control_transfer((Command::dev_addr << 1), cmd.addr, &(cmd.value),
                                  sizeof(cmd.value));
        }
        return true;
    }

    [[nodiscard]] int bulk_read(milliseconds timeout = 400ms,
                                std::optional<nonstd::span<uint8_t>> dst_buffer = std::nullopt);
};
}  // namespace hardware_drivers