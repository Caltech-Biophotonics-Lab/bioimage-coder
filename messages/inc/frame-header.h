#pragma once
#include <cstdio>

namespace frame_capture_card {

namespace constants {
/** Image frame header's magic number. */
constexpr uint32_t signature = 0x123abc00;
}  // namespace constants

/** Serialization protocol of the frame header */
namespace frame_header {
// Header pattern: 0x00bc3a12 01bc3a12 ... 05bc3a12 **bc3a12 **bc3a12 ****...
//                                                  ^cam_id  ^led_id  ^ data
#pragma pack(push, 1)
struct runway_t {
    uint8_t offset;
    uint8_t delimiter[3];
};
static_assert(sizeof(runway_t) == sizeof(uint32_t));

// Header pattern: 0x00bc3a12 01bc3a12 ... 05bc3a12 **bc3a12 **bc3a12 ****...
//                                                  ^cam_id  ^led_id  ^ data
struct header_t {
    uint8_t cam_id;
    uint8_t delimiter[3];
    uint8_t led_id;
    uint8_t frame_id;  // Internal frame index
    uint8_t delimiter2[2];
};
static_assert(sizeof(header_t) == sizeof(uint32_t) * 2);

struct full_header_t {
    runway_t runway[6];
    header_t header;
};
static_assert(sizeof(full_header_t) == sizeof(uint32_t) * 8);
#pragma pack(pop)

}  // namespace frame_header
}  // namespace frame_capture_card