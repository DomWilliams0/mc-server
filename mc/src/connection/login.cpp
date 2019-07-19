#include "connection.h"
#include "packet.h"
#include "util.h"


mc::BaseConnection *mc::ConnectionLogin::handle_packet(mc::Varint::Int packet_id, mc::Buffer &packet,
                                                       mc::PacketClientBound **response) {
    switch (packet_id) {
        case 0x00: {
            auto login_start(read_inbound_packet<PacketLoginStart>(packet));

            *response = new PacketLoginSuccess;
            (*response)->get_field_value<String>("uuid") = String("123e4567-e89b-12d3-a456-426655440000"); // TODO
            (*response)->get_field_value<String>("username") = login_start.get_field_value<String>("name");

            return new ConnectionPlay(*this);
        }

        default:
            return nullptr;

    }
}
