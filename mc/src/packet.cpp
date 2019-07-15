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

void mc::PacketHandshake::read_body(mc::Buffer &buffer) {
    buffer.read(protocol_version);
    buffer.read(server_address);
    buffer.read(server_port);
    buffer.read(next_state);
}

std::string mc::PacketHandshake::to_string() const {
    std::ostringstream ss;
    ss << "handshake(protocol=" << *protocol_version <<
       ", server=" << *server_address << ":" << server_port <<
       ", next_state=" << *next_state << ")";

    return ss.str();
}

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

std::string mc::PacketStatusResponse::to_string() const {
    return std::string("status('") + status.value() + "')";
}

mc::Varint::Int mc::PacketStatusResponse::calculate_body_length() const {
    return status.length().get_byte_count() + *status.length();
}

void mc::PacketStatusResponse::write_body(mc::Buffer &buffer) {
    buffer.write(status);
}

std::string mc::PacketPing::to_string() const {
    return std::string("ping(") + std::to_string(payload) + ")";
}

void mc::PacketPing::read_body(mc::Buffer &buffer) {
    buffer.read(payload);
}

mc::Varint::Int mc::PacketPong::calculate_body_length() const {
    return sizeof(payload);
}

void mc::PacketPong::write_body(mc::Buffer &buffer) {
    buffer.write(payload);
}

std::string mc::PacketPong::to_string() const {
    return std::string("pong(") + std::to_string(payload) + ")";
}
