#ifndef MC_SERVER_PACKET_H
#define MC_SERVER_PACKET_H

#include "types.h"
#include "io.h"

namespace mc {

    class BasePacket {
    public:
        explicit BasePacket(Varint::Int packet_id) : packet_id(packet_id) {}

        Varint packet_id;

        // only for inbound
        virtual void read_body(Buffer &buffer);

        void write(Buffer &buffer);

        Varint calculate_full_length() const;

        virtual std::string to_string() const = 0;

    protected:
        // length of all packet-specific fields, only for outbound
        virtual Varint::Int calculate_body_length() const;

        // only for outbound
        virtual void write_body(Buffer &buffer);
    };


    class PacketHandshake : public BasePacket {

    public:
        explicit PacketHandshake() : BasePacket(0x00) {}

        std::string to_string() const override;

        void read_body(Buffer &buffer) override;

    public:
        Varint protocol_version;
        String server_address;
        UShort server_port{};
        Varint next_state;
    };

    class PacketEmpty : public BasePacket {
    public:
        explicit PacketEmpty() : BasePacket(0x00) {}

        void read_body(Buffer &buffer) override;

        std::string to_string() const override;

    protected:
        Varint::Int calculate_body_length() const override;
    };

    class PacketStatusResponse : public BasePacket {
    public:
        explicit PacketStatusResponse(String &&status) : BasePacket(0x00), status(std::move(status)) {}

        std::string to_string() const override;

    protected:
        Varint::Int calculate_body_length() const override;

        void write_body(Buffer &buffer) override;

    private:
        String status;
    };

    // inbound ping
    class PacketPing : public BasePacket {
    public:
        explicit PacketPing() : BasePacket(0x01) {}

        std::string to_string() const override;

        void read_body(Buffer &buffer) override;

        Long payload{};

    };

    // outbound ping response
    class PacketPong : public BasePacket {
    public:
        explicit PacketPong(Long payload) : BasePacket(0x01), payload(payload) {}

        std::string to_string() const override;

    protected:
        Varint::Int calculate_body_length() const override;

        void write_body(Buffer &buffer) override;

    protected:
        Long payload{};

    };

}

#endif
