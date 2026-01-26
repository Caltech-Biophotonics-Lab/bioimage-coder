#include "mock_usb.h"

#include <algorithm>

#include "frame-header.h"

using nonstd::span;

namespace {
constexpr uint8_t frame_id = 0xEE;
}  // namespace

namespace hardware_drivers {
int
MockUSB::bulk_read(milliseconds, std::optional<span<uint8_t>> dst_buffer) {
    // Header pattern: 0x00bc3a12 01bc3a12 ... 05bc3a12 **bc3a12 **@@3a12 ****...
    using frame_capture_card::constants::signature;
    const std::array<uint32_t, 8> dummy_header{
        signature,
        signature | 0x01,
        signature | 0x02,
        signature | 0x03,
        signature | 0x04,
        signature | 0x05,
        signature | cam_id,
        (signature & 0xffff0000) | (static_cast<uint32_t>(frame_id) << 8) | led_id};

    /// @note cam_id may change over time.
    std::copy_n(reinterpret_cast<const uint8_t*>(dummy_header.data()),
                dummy_header.size() * sizeof(uint32_t), buffer.begin());
    if (dst_buffer == std::nullopt) {
        // Simulate builk read to internal buffer.
        return buffer.size();
    }

    // Simulate data transfer from USB to destination buffer, up to the buffer
    // capacity.
    const auto payload_length = std::min(static_cast<size_t>(dst_buffer->size()), buffer.size());
    std::copy_n(buffer.begin(), payload_length, dst_buffer->begin());
    return payload_length;
}
}  // namespace hardware_drivers