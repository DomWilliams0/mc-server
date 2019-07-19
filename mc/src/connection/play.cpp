#include "connection.h"
#include "packet.h"
#include "util.h"


mc::BaseConnection *mc::ConnectionPlay::handle_packet(mc::Varint::Int packet_id, mc::Buffer &packet,
                                                      mc::PacketClientBound **response) {
    UNUSED(packet_id);
    UNUSED(packet);
    UNUSED(response);
    return this;
}
