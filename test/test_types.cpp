#include "catch.hpp"
#include "types.h"
#include "io.h"

static void assert_varint(mc::Varint::Int value, const mc::Varint::Bytes &expected_bytes, int expected_byte_count) {
    mc::Varint encoded(value);
    REQUIRE(encoded.get_real_value() == value);
    REQUIRE(encoded.get_byte_count() == expected_byte_count);
    for (int i = 0; i < expected_byte_count; ++i) {
        REQUIRE(encoded.get_bytes()[i] == expected_bytes[i]);
    }

    mc::Varint decoded(expected_bytes);
    REQUIRE(encoded.get_real_value() == decoded.get_real_value());
    REQUIRE(encoded.get_byte_count() == decoded.get_byte_count());
    for (int i = 0; i < expected_byte_count; ++i) {
        REQUIRE(encoded.get_bytes()[i] == decoded.get_bytes()[i]);
    }

}

TEST_CASE("datatypes", "[types]") {
    SECTION("varint") {
        assert_varint(0, {0x00}, 1);
        assert_varint(1, {0x01}, 1);
        assert_varint(127, {0x7f}, 1);
        assert_varint(128, {0x80, 0x01}, 2);
        assert_varint(255, {0xff, 0x01}, 2);
        assert_varint(2147483647, {0xff, 0xff, 0xff, 0xff, 0x07}, 5);
        assert_varint(-1, {0xff, 0xff, 0xff, 0xff, 0x0f}, 5);
        assert_varint(-2147483648, {0x80, 0x80, 0x80, 0x80, 0x08}, 5);
    }
}