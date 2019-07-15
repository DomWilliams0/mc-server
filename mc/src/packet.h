#ifndef MC_SERVER_PACKET_H
#define MC_SERVER_PACKET_H

#include "types.h"
#include "io.h"

namespace mc {

    class BasePacket {
    public:
        Varint packet_id;

        void read(Buffer &buffer);

        void write(Buffer &buffer);

        Varint::Int calculate_full_length() const;

    protected:
        // length of all packet-specific fields
        virtual Varint::Int calculate_body_length() const = 0;

        virtual void read_body(Buffer &buffer) = 0;

        virtual void write_body(Buffer &buffer) = 0;
    };


    class PacketHandshake : public BasePacket {
    protected:
        Varint::Int calculate_body_length() const override;

        void read_body(Buffer &buffer) override;

        void write_body(Buffer &buffer) override;

    public:
        Varint protocol_version;
        String server_address;
        UShort server_port{};
        Varint next_state;
    };


}

#endif
