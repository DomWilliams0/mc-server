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

    // match packet type
    // TODO match_packet is leaked if anything errors
    PacketServerBound *concrete_packet = match_packet(*packet_id);
    if (concrete_packet == nullptr) {
        std::ostringstream ss;
        ss << "resolving packet id " << *packet_id << " in connection '" << type_name() << "'";
        throw Exception(ErrorType::kUnexpectedPacketId, ss.str());
    }

    // populate packet fields
    concrete_packet->read_body(packet);
    DLOG_F(INFO, "inbound packet: %s", concrete_packet->to_string().c_str());

    // handle packet in connection, maybe generating a response and a new state
    mc::PacketClientBound *response = nullptr;
    BaseConnection *new_connection = handle_packet(concrete_packet, &response);

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

