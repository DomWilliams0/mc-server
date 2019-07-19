#include "connection.h"
#include "packet.h"
#include "loguru.hpp"

#include <sstream>
#include <cassert>

mc::BaseConnection::BaseConnection(const mc::Socket &socket) : socket(socket) {}


mc::BaseConnection *mc::BaseConnection::handle_packet(mc::Buffer &packet) {
    // parse header
    Varint packet_id;
    packet.read(packet_id);
    DLOG_F(INFO, "packet id is %d", *packet_id);

    // pass off to this connection to read and handle
    mc::PacketClientBound *response = nullptr;
    BaseConnection *new_connection = handle_packet(*packet_id, packet, &response);

    // send response if necessary
    if (response != nullptr) {
        DLOG_F(INFO, "sending response: %s", response->to_string().c_str());

        mc::Buffer buffer;
        response->write(buffer);
        socket.write(buffer);
    }

    assert(new_connection != nullptr);
    return new_connection;
}

mc::BaseConnection::BaseConnection(const mc::BaseConnection &other) : socket(other.socket) {}

