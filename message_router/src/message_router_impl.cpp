#include <fmt/format.h>

#include <array>
#include <asio.hpp>

#include "message_router.h"

namespace message_router {

namespace impl {
template <class SerialInterface>
void
executeCommand(SerialInterface& serial, std::string_view message) {
    asio::write(serial, asio::buffer(message));
    // asio::read_until(serial, asio::buffer(buffer.data(), buffer.size()), '\n');
}

template <class SerialInterface, typename FmtString, typename... Args>
void
executeCommand(SerialInterface& serial, FmtString fmt_string, Args&&... args) {
    constexpr auto size = 16;
    std::array<char, size> buffer;
    const auto [_, message_length] = fmt::format_to_n(
        buffer.begin(), size, std::forward<FmtString>(fmt_string), std::forward<Args>(args)...);

    asio::write(serial, asio::const_buffer{buffer.data(), message_length});
    // asio::read_until(serial, asio::buffer(buffer), '\n');
}

}  // namespace impl

template <class SerialInterface>
MessageOverSerial<SerialInterface>::MessageOverSerial(SerialInterface& s) : serial{s} {}

}  // namespace message_router

#include "laser.hpp"
#include "led_matrix.hpp"
#include "motion.hpp"

template class message_router::MessageOverSerial<asio::serial_port>;

#ifdef MOCK_SERIAL
// Physical serial devices, e.g. Arduino UNO R3, are very checp. We don't
// reallly need a mock serial for testing.
template class message_router::MessageOverSerial<MockSerial>;
#endif