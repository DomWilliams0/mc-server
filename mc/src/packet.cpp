#include "packet.h"
#include "loguru.hpp"
#include "util.h"
#include <sstream>

std::string mc::Packet::to_string() const {
    std::ostringstream ss;
    ss << name << "(";
    for (size_t i = 0; i < fields.size(); ++i) {
        Field *f = fields[i];
        if (i != 0)
            ss << ", ";
        ss << f->name << "=" << f->get_value_as_str();
    }
    ss << ")";
    return ss.str();
}

void mc::PacketClientBound::write(mc::Buffer &buffer) {
    mc::Varint length(calculate_full_length());
    DLOG_F(INFO, "full length of outgoing packet is %d (0x%x)", *length, *length);

    // header
    buffer.write(length);
    buffer.write(packet_id);

    // body
    for (const Field *field : fields) {
        field->write(buffer);
    }
}

mc::Varint mc::PacketClientBound::calculate_full_length() const {
    mc::Varint::Int body_length = packet_id.get_byte_count();
    for (const Field *field : fields) {
        body_length += field->size();
    }

    return mc::Varint(body_length);
}

void mc::PacketServerBound::read_body(mc::Buffer &buffer) {
    for (Field *field : fields) {
        field->read(buffer);
    }
}

