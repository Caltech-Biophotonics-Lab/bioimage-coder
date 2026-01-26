#include "master_task.h"

#include <fmt/format.h>

#include "bioimage-coder/executor.hpp"
#include "fiber-messages.h"
#include "main_protocol.hpp"

void
bioimageExecutorTask() {
    try {
        bioimage_coder::execute(mainProtocol());

        // Close serial port.
        serial_port.close();

    } catch (const asio::system_error& e) {
        fmt::print(stderr, FMT_STRING("ASIO error: {:s}\n"), e.what());
        for (auto& port : image_capture_handlers) {
            port.close();
        }
    }
}