/** @file */
#include <algorithm>
#include <stdexcept>
#include <thread>

#include "constants.h"
#include "frame-commands.h"
#include "frame-header.h"

#ifdef USING_FIBER

#include <boost/fiber/all.hpp>
using boost::this_fiber::sleep_for;
using boost::this_fiber::yield;

#else

using std::this_thread::sleep_for;

#endif

namespace message_router {

enum cyusb_packet : int32_t {
    USB_BURST_SIZE = 16 * 1024,
    USB_PACK_SIZE = 16 * 1024,
    USB_BUF_SIZE = 512 * 1024,
    MAX_QUEUE_SZ = 64
};
template <class U>
FrameCaptureCard<U>::FrameCaptureCard(uint8_t usb_id) : usb{usb_id} {}

template <class USBInterface>
template <bool check_data_num>
frame_capture_card::frame_metadata_t
FrameCaptureCard<USBInterface>::captureSingleFrame(span<uint8_t> image_buffer,
                                                   const std::chrono::milliseconds timeout) {
    using namespace frame_capture_card::frame_header;
    using camera::n_pixels;
    using frame_capture_card::commands::read_pixel_count_t;

    assert(image_buffer.size() >= n_pixels);

    if constexpr (check_data_num) {
        int error_cnt = 0;
        for (; error_cnt < 3; error_cnt++) {
            const auto frame_id = usb.template control_read<read_pixel_count_t>();
            if (frame_id.value > 0) break;
            sleep_for(400ms);
        }
        if (error_cnt >= 3) {
            throw std::runtime_error("No image data from FPGA");
        }
    }

    int byte_transferred = 0;

    // Seek image header
    // Header pattern: 0x00bc3a12 01bc3a12 ... 05bc3a12 **bc3a12 **@@3a12 ****...
    //                                                             ^frame_id
    //                                                  ^cam_id  ^led_id  ^ data
    {
        constexpr int error_limit = 1000;
        int error_cnt = 0;
        for (; error_cnt < error_limit; error_cnt++) {
            byte_transferred = usb.bulk_read(timeout);

#ifdef USING_FIBER
            yield();
#endif
            const uint32_t checksum = usb.template decode<uint32_t>();
            if ((checksum & 0xffffff00) == frame_capture_card::constants::signature) {
                break;
            }
        }
        if (error_cnt >= error_limit) {
            throw std::runtime_error("Cannot find image header");
        }
    }

    // Seek the image header
    const runway_t runway = usb.template decode<runway_t>();
    const int32_t header_offset =
        sizeof(full_header_t) - runway.offset * sizeof(runway_t) - sizeof(header_t);

    // Cast to header C-struct
    const header_t header = usb.template decode<header_t>(header_offset);

    // Skip the header, copy the first chunk of data
    if (byte_transferred > header_offset) {
        const auto source_chunk =
            span<uint8_t>{usb.buffer}.subspan(header_offset, byte_transferred - header_offset);
        std::copy(source_chunk.begin(), source_chunk.end(), image_buffer.begin());
    }

    // Receive the rest of the image.
    for (int idx = byte_transferred - header_offset; idx < n_pixels; idx += byte_transferred) {
        // Wait until the data is available
        if constexpr (check_data_num) {
            while (!(usb.template control_read<read_pixel_count_t>() < n_pixels - idx)) {
                sleep_for(1ms);
            }
        }

        byte_transferred = usb.bulk_read(timeout, image_buffer.subspan(idx, n_pixels - idx));

#ifdef USING_FIBER
        yield();
#endif
    }

    if constexpr (USBInterface::is_mock) {
        // Simulate streaming of pixels from the next camera. In practice, the
        // frames arrives out of order.
        usb.cam_id = (usb.cam_id % frame_capture_card::n_cameras_per_board) + 1;
    }
    return {header.cam_id, header.led_id};
}
}  // namespace message_router