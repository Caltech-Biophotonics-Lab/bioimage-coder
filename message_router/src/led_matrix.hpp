#pragma once
#include "message_router.h"

namespace led_matrix = message::led_matrix;
namespace message_router {

template <class S>
void
MessageOverSerial<S>::sendCommand(led_matrix::switch_to p) {
    impl::executeCommand(serial, "m {:d} {:d}\n", p.x, p.y);
}

template <class S>
void
MessageOverSerial<S>::sendCommand(led_matrix::blank) {
    impl::executeCommand(serial, "f\n");
}

template <class S>
void
MessageOverSerial<S>::sendCommand(led_matrix::next) {
    impl::executeCommand(serial, "n\n");
}

template <class S>
void
MessageOverSerial<S>::sendCommand(led_matrix::color_t color) {
    using c = led_matrix::color_t;
    switch (color) {
        case c::R:
            impl::executeCommand(serial, "c 1 0 0\n");
            return;
        case c::G:
            impl::executeCommand(serial, "c 0 1 0\n");
            return;
        case c::B:
            impl::executeCommand(serial, "c 0 0 1\n");
            return;
    }
}

}  // namespace message_router