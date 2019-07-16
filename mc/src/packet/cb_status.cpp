#include "packet.h"

std::string mc::PacketStatusResponse::to_string() const {
    return std::string("status('") + status.value() + "')";
}

mc::Varint::Int mc::PacketStatusResponse::calculate_body_length() const {
    return status.length().get_byte_count() + *status.length();
}

void mc::PacketStatusResponse::write_body(mc::Buffer &buffer) {
    buffer.write(status);
}

