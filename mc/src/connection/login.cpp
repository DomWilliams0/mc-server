#include "connection.h"
#include "packet/packet.h"
#include "util.h"


mc::BasePacket *mc::ConnectionLogin::match_packet(mc::Varint::Int packet_id, mc::Buffer &packet) {
    UNUSED(packet_id);
    UNUSED(packet);
    throw mc::Exception(mc::ErrorType::kNotImplemented, "login");
}

mc::BaseConnection *mc::ConnectionLogin::handle_packet(mc::BasePacket *packet, BasePacket **response) {
    UNUSED(packet);
    UNUSED(response);
    throw mc::Exception(mc::ErrorType::kNotImplemented, "login");
}
