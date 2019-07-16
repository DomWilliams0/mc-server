#include "connection.h"
#include "packet.h"
#include "util.h"


mc::PacketServerBound *mc::ConnectionLogin::match_packet(Varint::Int packet_id) {
    UNUSED(packet_id);
    throw mc::Exception(mc::ErrorType::kNotImplemented, "login");
}

mc::BaseConnection *mc::ConnectionLogin::handle_packet(PacketServerBound *packet, PacketClientBound **response) {
    UNUSED(packet);
    UNUSED(response);
    throw mc::Exception(mc::ErrorType::kNotImplemented, "login");
}
