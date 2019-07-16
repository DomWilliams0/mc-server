#include "packet.h"
#include "loguru.hpp"
#include "util.h"
#include <sstream>

void mc::BasePacket::write(mc::Buffer &buffer) {
    mc::Varint length(calculate_full_length());
    DLOG_F(INFO, "full length of outgoing packet is %d (0x%x)", *length, *length);

    // header
    buffer.write(length);
    buffer.write(packet_id);

    // body
    write_body(buffer);
}

mc::Varint mc::BasePacket::calculate_full_length() const {
    mc::Varint::Int body_length = calculate_body_length();
    // TODO dont make each packet do each field, surely we can automate it

    // add packet_id in header
    body_length += packet_id.get_byte_count();

    return mc::Varint(body_length);
}

void mc::BasePacket::read_body(mc::Buffer &buffer) {
    UNUSED(buffer);
    throw Exception(ErrorType::kNotImplemented, "packet is outbound-only", typeid(this).name());
}

void mc::BasePacket::write_body(mc::Buffer &buffer) {
    UNUSED(buffer);
    throw Exception(ErrorType::kNotImplemented, "packet is inbound-only", typeid(this).name());
}

mc::Varint::Int mc::BasePacket::calculate_body_length() const {
    throw Exception(ErrorType::kNotImplemented, "packet is inbound-only", typeid(this).name());
}


