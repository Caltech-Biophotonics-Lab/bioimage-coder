#pragma once
#include <chrono>
#include <stdexcept>

//
#include <nonstd/span.hpp>

#include "frame-commands.h"

namespace message_router {
using namespace std::chrono_literals;
using nonstd::span;

template <class USBInterface>
class FrameCaptureCard {
   public:
    FrameCaptureCard(uint8_t usb_id);

    /** Transmit the commands over the USB3.0 port. The FPGAs on the frame
     * capture cards internally routes the signals to either the 2nd stage
     * FPGAs, or boradcasts the messages to all 96 CMOS sensors. */
    template <class Command>
    [[nodiscard]] bool sendCommand(Command&& cmd) {
        if constexpr (USBInterface::is_mock &&
                      std::is_same_v<Command, frame_capture_card::commands::write_led_id_t>) {
            usb.led_id = cmd.value;
        }
        return usb.control_write(std::forward<Command>(cmd));
    }

    /** Query the capture card's ID.
     *
     * If the USB interface is a mock class, simulate the board ID with the USB
     * port number. Otherwise, query directly from the DIP switch on the capture
     * board over USB interface.
     */
    uint8_t readBoardID() const {
        if constexpr (USBInterface::is_mock) {
            return usb.id;
        } else {
            return usb.control_read(frame_capture_card::commands::read_board_id_t{});
        }
    }

    /** Read single frame from one of the 24 cameras.
     *
     * @param[in] (Optional) memory range to receive pixel data from 24 cameras.
     * @param[in] (Optional) Estimated intervals between consecutive incoming
     * USB packets.
     *
     * @return LED id and the board id.
     */
    template <bool check_data_num = false>
    frame_capture_card::frame_metadata_t captureSingleFrame(
        span<uint8_t> image_buffer, const std::chrono::milliseconds timeout = 400ms);

   private:
    USBInterface usb;
};
}  // namespace message_router

#include "frame-capture-card-impl.hpp"