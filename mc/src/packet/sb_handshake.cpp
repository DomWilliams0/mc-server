#include "packet.h"
#include <sstream>

void mc::PacketHandshake::read_body(mc::Buffer &buffer) {
    buffer.read(protocol_version);
    buffer.read(server_address);
    buffer.read(server_port);
    buffer.read(next_state);
}

std::string mc::PacketHandshake::to_string() const {
    std::ostringstream ss;
    ss << "handshake(protocol=" << *protocol_version <<
       ", server=" << *server_address << ":" << server_port <<
       ", next_state=" << *next_state << ")";

    return ss.str();
}

