#include "packet.h"


std::string mc::PacketPing::to_string() const {
    return std::string("ping(") + std::to_string(payload) + ")";
}

void mc::PacketPing::read_body(mc::Buffer &buffer) {
    buffer.read(payload);
}
