#pragma once
#include <cstdint>

namespace frame_capture_card {

struct frame_metadata_t {
    uint8_t cam_id;
    uint8_t led_id;
};

namespace commands {
/** Device address for the 1st stage FPGA IC. */
constexpr uint16_t fpga_dev_addr{0x0000};

/** Read the image capture card's ID from the DIP switches on the board. */
struct read_board_id_t {
    static constexpr uint16_t dev_addr{fpga_dev_addr};
    static constexpr uint16_t reg_addr{0x0003};
    uint8_t value{};
};

/** Reset FPGA board. */
struct reset_t {
    static constexpr uint16_t dev_addr{fpga_dev_addr};
    static constexpr uint16_t reg_addr{0x0103};
    const uint8_t value{1};
};

/** Tag the upcoming image frames with the user-specified LED ID. */
struct write_led_id_t {
    static constexpr uint16_t dev_addr{fpga_dev_addr};
    static constexpr uint16_t reg_addr{0x0060};
    uint8_t value{};
};

/** Read the accumulated pixels in the FPGA buffer. */
struct read_pixel_count_t {
    static constexpr uint16_t dev_addr{fpga_dev_addr};
    static constexpr uint16_t reg_addr{0x0004};
    uint32_t value{};
};

/** Transmit the i2c command through the FPGA-over-USB3.0 port, and then
 * broadcast the command to all 24 CMOS cameras through the MIPI bus. */
#pragma pack(push, 2)
struct i2c_cmd_t {
    static constexpr uint16_t dev_addr{0x00F8};
    uint16_t addr{};
    uint8_t value{};
};
#pragma pack(pop)

}  // namespace commands
}  // namespace frame_capture_card