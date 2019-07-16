#include "packet.h"
#include "util.h"

void mc::PacketEmpty::read_body(mc::Buffer &buffer) {
    // nop
    UNUSED(buffer);
}

std::string mc::PacketEmpty::to_string() const {
    return "empty()";
}

mc::Varint::Int mc::PacketEmpty::calculate_body_length() const {
    return 0;
}

