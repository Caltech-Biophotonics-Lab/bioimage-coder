#include <array>
#include <catch2/catch_test_macros.hpp>

#include "constants.h"
#include "frame-capture-card.h"
#include "mock_usb.h"

TEST_CASE("Read the entire frame", "[get_image]") {
    using hardware_drivers::MockUSB;
    message_router::FrameCaptureCard<MockUSB> frame_capture_card(0);

    constexpr size_t n_pixels = camera::width * camera::height;
    std::vector<uint8_t> raw_pixels(n_pixels * frame_capture_card::n_cameras_per_board);

    constexpr bool assume_fifo_always_full = false;
    const auto [cam_id, led_id] =
        frame_capture_card.captureSingleFrame<assume_fifo_always_full>(raw_pixels);
    REQUIRE(cam_id == 0x04);
    REQUIRE(led_id == 0xEE);
}