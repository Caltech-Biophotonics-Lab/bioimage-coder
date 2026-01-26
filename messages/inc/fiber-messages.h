#pragma once
#include <boost/fiber/buffered_channel.hpp>
#include <chrono>
#include <variant>
#include <vector>

#include "constants.h"
#include "fiber-messages.h"
#include "frame-commands.h"

namespace fiber_messages {

namespace capture {

using completions_signal_t = boost::fibers::buffered_channel<bool>;

struct dark_frame_t {
    completions_signal_t* completion{nullptr};
};
struct fpm_frame_t {
    uint8_t led_id{};
    completions_signal_t* completion{nullptr};
};
struct fluorescence_frame_t {
    int16_t zpos{};
    channel_t ch{EGFP};
    completions_signal_t* completion{nullptr};
};

namespace camera {
using frame_capture_card::commands::i2c_cmd_t;
using std::chrono::milliseconds;
/** CMOS initialization sequence. */
struct init_sequence_t {
    static constexpr size_t max_count = 64;
    std::array<i2c_cmd_t, max_count> commands{};
    size_t count{};
};

/** Set CMOS exposure time and analog gain. */
struct exposure_gain_t {
    i2c_cmd_t exposure_lb{};
    i2c_cmd_t exposure_ub{};
    i2c_cmd_t log2_analog_gain{};

    constexpr milliseconds exposure() const {
        using namespace std::chrono_literals;
        return 1ms * (exposure_lb.value | (static_cast<uint16_t>(exposure_ub.value) << 8));
    }

    constexpr uint16_t gain() const { return uint16_t(1) << log2_analog_gain.value; }

    template <uint32_t analog_gain = 1>
    static constexpr exposure_gain_t setExposureGain(milliseconds exposure) {
        exposure_gain_t config{};

        /// Mock conversion from exposure and gain to CMOS sensors.
        /// @todo implement USBInterface::sendCommand<i2c_16_t>() for 16-bit data transfer.
        config.exposure_lb = {0x1234, static_cast<uint8_t>(exposure.count() & 0xff)};
        config.exposure_ub = {0x1235, static_cast<uint8_t>((exposure.count() >> 8) & 0xff)};

        static_assert(__builtin_popcount(analog_gain) == 1);
        constexpr uint8_t log2_analog_gain = std::log2(analog_gain);
        static_assert(log2_analog_gain <= 16,
                      "Digital gain beyond 16x, wasteful. BioImage-Coder DSL refuses to proceed.");

        config.log2_analog_gain = {0x1236, log2_analog_gain};

        return config;
    }
};
static_assert(sizeof(exposure_gain_t) == sizeof(i2c_cmd_t) * 3);
static_assert(sizeof(exposure_gain_t) == sizeof(i2c_cmd_t) * 3);

using namespace std::chrono_literals;
static_assert(exposure_gain_t::setExposureGain<1>(0ms).gain() == 1);
static_assert(exposure_gain_t::setExposureGain<8>(0ms).gain() == 8);
static_assert(exposure_gain_t::setExposureGain<16>(0ms).gain() == 16);

static_assert(exposure_gain_t::setExposureGain<1>(0ms).exposure() == 0ms);
static_assert(exposure_gain_t::setExposureGain<1>(30ms).exposure() == 30ms);
static_assert(exposure_gain_t::setExposureGain<1>(1'000ms).exposure() == 1'000ms);
}  // namespace camera

using command_t = std::variant<dark_frame_t, fpm_frame_t, fluorescence_frame_t,
                               camera::init_sequence_t, camera::exposure_gain_t>;
using queue_t = boost::fibers::buffered_channel<command_t>;

}  // namespace capture

namespace write {
using frame_capture_card::frame_metadata_t;

struct dark_frame_t {
    uint8_t board_id{};
    uint8_t cam_id{};
    std::vector<uint8_t> image_frame{};

    constexpr dark_frame_t() = default;
    dark_frame_t(uint8_t b, frame_metadata_t m, std::vector<uint8_t>&& i)
        : board_id{b}, cam_id{m.cam_id}, image_frame{i} {}
};

struct fpm_frame_t {
    uint8_t board_id{};
    uint8_t cam_id{};
    uint8_t led_id{};
    std::vector<uint8_t> image_frame{};

    constexpr fpm_frame_t() = default;
    fpm_frame_t(uint8_t b, frame_metadata_t m, std::vector<uint8_t>&& i)
        : board_id{b}, cam_id{m.cam_id}, led_id{m.led_id}, image_frame{i} {}
};

struct fluorescence_frame_t {
    uint8_t board_id{};
    uint8_t cam_id{};
    int16_t zpos{};
    channel_t ch{EGFP};
    std::vector<uint16_t> image_frame{};
};
using command_t = std::variant<dark_frame_t, fpm_frame_t, fluorescence_frame_t>;
using queue_t = boost::fibers::buffered_channel<command_t>;

}  // namespace write

}  // namespace fiber_messages
