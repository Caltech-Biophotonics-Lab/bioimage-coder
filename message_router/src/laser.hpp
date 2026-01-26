#pragma once
#include "message_router.h"

namespace excitation = message::excitation;

namespace message_router {

template <class S>
void
MessageOverSerial<S>::sendCommand(excitation::laser cmd) {
    if (cmd.power > 0) {
        fmt::print(FMT_STRING("[ ] Laser {:s} power at {:d}x for {:d}s\n"), toString(cmd.ch),
                   cmd.power, cmd.time.count());
    } else {
        fmt::print(FMT_STRING("[ ] Laser {:s} off\n"), toString(cmd.ch));
    }
    impl::executeCommand(serial, "B {:d} {:d} {:d}\n", cmd.time.count(), cmd.power,
                         static_cast<uint8_t>(cmd.ch));
}

}  // namespace message_router
