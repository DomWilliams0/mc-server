#include "packet.h"
#include "util.h"

mc::Varint::Int mc::PacketPong::calculate_body_length() const {
    return sizeof(payload);
}

void mc::PacketPong::write_body(mc::Buffer &buffer) {
    buffer.write(payload);
}

std::string mc::PacketPong::to_string() const {
    return std::string("pong(") + std::to_string(payload) + ")";
}
