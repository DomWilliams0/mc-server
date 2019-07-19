#include "connection.h"
#include "packet.h"
#include "util.h"


mc::ConnectionHandshaking::ConnectionHandshaking(const mc::Socket &socket) : BaseConnection(socket) {}

mc::BaseConnection *mc::ConnectionHandshaking::handle_packet(mc::Varint::Int packet_id, mc::Buffer &packet,
                                                             mc::PacketClientBound **response) {
    UNUSED(response);

    switch (packet_id) {
        case 0x00: {
            auto handshake(read_inbound_packet<PacketHandshake>(packet));

            Varint next_state = handshake.get_field_value<Varint>("next_state");

            if (*next_state == 1) {
                return new ConnectionStatus(*this);
            }

            if (*next_state == 2) {
                return new ConnectionLogin(*this);
            }

            throw Exception(ErrorType::kBadEnum, "bad next_state");

        }
        default:
            return nullptr;
    }
}

