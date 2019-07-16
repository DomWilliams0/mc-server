#include "catch.hpp"
#include "types.h"
#include "io.h"
#include "packet/packet.h"
#include <cstring>

TEST_CASE("packets", "[packets]") {
    SECTION("handshake") {
        std::array<uint8_t, 17> handshake_bytes = {0x10, 0x00, 0xea, 0x03, 0x09, 0x6c, 0x6f, 0x63,
                                                   0x61, 0x6c, 0x68, 0x6f, 0x73, 0x74, 0x63, 0xdd,
                                                   0x01};
        mc::Socket socket((uint8_t *) handshake_bytes.data(), handshake_bytes.size());
        mc::Buffer buffer(socket.read());

        mc::PacketHandshake handshake;
        buffer.read(handshake.packet_id);
        REQUIRE(*handshake.packet_id == 0);

        handshake.read_body(buffer);
        REQUIRE(*handshake.protocol_version == 490);
        REQUIRE(!strcmp(handshake.server_address.value(), "localhost"));
        REQUIRE(handshake.server_port == 25565);
        REQUIRE(*handshake.next_state == 1);
    }
}