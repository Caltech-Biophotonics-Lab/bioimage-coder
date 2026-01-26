#include <array>
#include <catch2/catch_test_macros.hpp>

#include "mock_usb.h"

TEST_CASE("Bulk read from mock USB", "[mock_usb]") {
    hardware_drivers::MockUSB mock_usb(0);
    const auto byte_transferred = mock_usb.bulk_read();
    REQUIRE(byte_transferred == 1024);

    const uint32_t header = mock_usb.decode<uint32_t>();
    REQUIRE(header == 0x123abc00);
}