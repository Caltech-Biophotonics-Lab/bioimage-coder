#pragma once
#include <array>

#include "fiber-messages.h"

void fileWriteWorker(fiber_messages::write::queue_t& write_queue);