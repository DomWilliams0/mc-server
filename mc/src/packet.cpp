#include "packet.h"

void mc::BasePacket::read(mc::Buffer &buffer) {
    // packet id
    buffer.read(packet_id);

    // packet-specific body
    read_body(buffer);
}

void mc::BasePacket::write(mc::Buffer &buffer) {

}

mc::Varint::Int mc::BasePacket::calculate_full_length() const {
    return 0;
}

mc::Varint::Int mc::PacketHandshake::calculate_body_length() const {
    return 0;
}

void mc::PacketHandshake::read_body(mc::Buffer &buffer) {
    buffer.read(protocol_version);
    buffer.read(server_address);
    buffer.read(server_port);
    buffer.read(next_state);
}

void mc::PacketHandshake::write_body(mc::Buffer &buffer) {

}
