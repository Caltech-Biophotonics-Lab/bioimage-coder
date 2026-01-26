#pragma once
#include <array>
#include <boost/fiber/barrier.hpp>
#include <cstdint>

#include "fiber-messages.h"

void imageCaptureWorker(const uint8_t board_id, fiber_messages::capture::queue_t& capture_queue,
                        fiber_messages::write::queue_t& write_queue);