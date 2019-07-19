#ifndef MC_SERVER_CONNECTION_H
#define MC_SERVER_CONNECTION_H

#include "io.h"
#include "packet.h"
#include "loguru.hpp"

namespace mc {

    class BaseConnection {
    public:
        explicit BaseConnection(const mc::Socket &socket);

        virtual ~BaseConnection() = default;

        BaseConnection(const BaseConnection &other);

        bool keep_alive() const { return keep_alive_; }

        BaseConnection *handle_packet(mc::Buffer &packet);

        virtual std::string type_name() const = 0;


    protected:
        virtual BaseConnection *handle_packet(Varint::Int packet_id, Buffer &packet, PacketClientBound **response) = 0;

        template<typename T>
        T read_inbound_packet(Buffer &buffer) {
            T t;
            t.read_body(buffer);
            DLOG_F(INFO, "inbound packet: %s", t.to_string().c_str());
            return t;
        }

        mc::Socket socket;
        bool keep_alive_ = true;
    };

    class ConnectionHandshaking : public BaseConnection {
    public:
        explicit ConnectionHandshaking(const Socket &socket);

        inline std::string type_name() const override { return "handshake"; }

    protected:
        BaseConnection *handle_packet(Varint::Int packet_id, Buffer &packet, PacketClientBound **response) override;
    };

    class ConnectionStatus : public BaseConnection {
    protected:
    public:
        explicit ConnectionStatus(const BaseConnection &other) : BaseConnection(other) {}

        inline std::string type_name() const override { return "status"; }

    protected:
        BaseConnection *handle_packet(Varint::Int packet_id, Buffer &packet, PacketClientBound **response) override;

    };

    class ConnectionLogin : public BaseConnection {
    public:
        explicit ConnectionLogin(const BaseConnection &other) : BaseConnection(other) {}

        inline std::string type_name() const override { return "login"; }

    protected:
        BaseConnection *handle_packet(Varint::Int packet_id, Buffer &packet, PacketClientBound **response) override;
    };

}

#endif
