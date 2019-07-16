#include "connection.h"
#include "packet/packet.h"
#include "util.h"


mc::ConnectionHandshaking::ConnectionHandshaking(const mc::Socket &socket) : BaseConnection(socket) {}

mc::BasePacket *mc::ConnectionHandshaking::match_packet(mc::Varint::Int packet_id, mc::Buffer &packet) {
    UNUSED(packet);
    switch (packet_id) {
        case 0x00:
            return new PacketHandshake;
        default:
            return nullptr;
    }
}

mc::BaseConnection *mc::ConnectionHandshaking::handle_packet(mc::BasePacket *packet, BasePacket **response) {
    auto *handshake = dynamic_cast<PacketHandshake *>(packet);
    UNUSED(response);

    if (*handshake->next_state == 1) {
        // status
        return new ConnectionStatus(*this);
    }

    if (*handshake->next_state == 2) {
        // login
        return new ConnectionLogin(*this);
    }

    throw Exception(ErrorType::kBadEnum, "bad next_state");
}

