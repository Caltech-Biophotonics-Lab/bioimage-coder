#include <array>
#include <bitset>
#include <catch2/catch_test_macros.hpp>

#define USING_FIBER
#include "constants.h"
#include "frame-capture-card.h"
#include "mock_usb.h"

using frame_capture_card::n_boards;
using hardware_drivers::MockUSB;
namespace {

enum state_t { IDLE = 0, ACTIVE, COMPLETE };
std::array<state_t, n_boards> fiber_state;
constexpr size_t n_images = 24;

using frame_arrival_t = std::bitset<frame_capture_card::n_cameras_per_board>;
std::array<frame_arrival_t, n_boards> masks;

template <uint8_t board_id>
void
capture_task(size_t n_images) {
    static_assert(board_id < n_boards);
    fiber_state[board_id] = IDLE;

    // Initialize camera board
    message_router::FrameCaptureCard<MockUSB> capture_card{board_id};

    // Allocate memory
    std::vector<uint8_t> raw_pixels(camera::n_pixels);

    fiber_state[board_id] = ACTIVE;
    for (size_t trial = 0; trial < n_images; trial++) {
        const auto ret = capture_card.captureSingleFrame<false>(raw_pixels);
        REQUIRE(ret.cam_id >= 1);
        masks[board_id].set(ret.cam_id - 1);
    }

    fiber_state[board_id] = COMPLETE;
}

}  // namespace

TEST_CASE("Read the entire frame", "[get_image]") {
    using boost::fibers::fiber;

    for (const auto& state : fiber_state) {
        REQUIRE(state == IDLE);
    }

    constexpr auto n_iterations = 24;
    std::array tasks{fiber{capture_task<0>, n_iterations}, fiber{capture_task<1>, n_iterations},
                     fiber{capture_task<2>, n_iterations}, fiber{capture_task<3>, n_iterations}};

    boost::this_fiber::yield();
    for (const auto& state : fiber_state) {
        REQUIRE(state == ACTIVE);
    }

    for (auto& task : tasks) {
        task.join();
    }

    for (const auto& state : fiber_state) {
        REQUIRE(state == COMPLETE);
    }

    for (const auto& frame_arrival_mask : masks) {
        REQUIRE(frame_arrival_mask == ((1UL << 24) - 1));
    }
}