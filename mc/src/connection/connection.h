#ifndef MC_SERVER_CONNECTION_H
#define MC_SERVER_CONNECTION_H

#include "io.h"
#include "packet.h"

namespace mc {


    class BaseConnection {
    public:
        explicit BaseConnection(const mc::Socket &socket);

        virtual ~BaseConnection() = default;

        BaseConnection(const BaseConnection &other);

        bool keep_alive() const { return keep_alive_; }

        BaseConnection *handle_packet(mc::Buffer &packet);

        inline virtual std::string type_name() const { return typeid(*this).name(); }


    protected:
        virtual PacketServerBound *match_packet(Varint::Int packet_id) = 0;

        virtual BaseConnection *handle_packet(PacketServerBound *packet, PacketClientBound **response) = 0;


        mc::Socket socket;
        bool keep_alive_ = true;
    };

    class ConnectionHandshaking : public BaseConnection {
    public:
        explicit ConnectionHandshaking(const Socket &socket);

    protected:

        PacketServerBound *match_packet(Varint::Int packet_id) override;

        BaseConnection *handle_packet(PacketServerBound *packet, PacketClientBound **response) override;
    };

    class ConnectionStatus : public BaseConnection {
    protected:
    public:
        explicit ConnectionStatus(const BaseConnection &other) : BaseConnection(other) {}

    protected:
        PacketServerBound *match_packet(Varint::Int packet_id) override;

        BaseConnection *handle_packet(PacketServerBound *packet, PacketClientBound **response) override;

    };

    class ConnectionLogin : public BaseConnection {
    public:
        explicit ConnectionLogin(const BaseConnection &other) : BaseConnection(other) {}

    protected:
        PacketServerBound *match_packet(Varint::Int packet_id) override;

        BaseConnection *handle_packet(PacketServerBound *packet, PacketClientBound **response) override;
    };

}

#endif
