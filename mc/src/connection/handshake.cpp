#include "connection.h"
#include "packet.h"
#include "util.h"


mc::ConnectionHandshaking::ConnectionHandshaking(const mc::Socket &socket) : BaseConnection(socket) {}

mc::PacketServerBound *mc::ConnectionHandshaking::match_packet(Varint::Int packet_id) {
    switch (packet_id) {
        case 0x00:
            return new PacketHandshake;
        default:
            return nullptr;
    }
}

mc::BaseConnection *mc::ConnectionHandshaking::handle_packet(PacketServerBound *packet, PacketClientBound **response) {
    auto *handshake = dynamic_cast<PacketHandshake *>(packet);
    UNUSED(response);
    Varint next_state = handshake->get_field_value<Varint>("next_state");

    if (*next_state == 1) {
        // status
        return new ConnectionStatus(*this);
    }

    if (*next_state == 2) {
        // login
        return new ConnectionLogin(*this);
    }

    throw Exception(ErrorType::kBadEnum, "bad next_state");
}

