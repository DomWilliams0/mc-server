#ifndef MC_SERVER_CONNECTION_H
#define MC_SERVER_CONNECTION_H

#include "io.h"
#include "packet/packet.h"

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
        virtual BasePacket *match_packet(Varint::Int packet_id, mc::Buffer &packet) = 0;

        virtual BaseConnection *handle_packet(mc::BasePacket *packet, BasePacket **response) = 0;


        mc::Socket socket;
        bool keep_alive_ = true;
    };

    class ConnectionHandshaking : public BaseConnection {
    public:
        explicit ConnectionHandshaking(const Socket &socket);

    protected:

        BasePacket *match_packet(Varint::Int packet_id, mc::Buffer &packet) override;

        BaseConnection *handle_packet(mc::BasePacket *packet, BasePacket **response) override;
    };

    class ConnectionStatus : public BaseConnection {
    protected:
    public:
        explicit ConnectionStatus(const BaseConnection &other) : BaseConnection(other) {}

    protected:
        BasePacket *match_packet(Varint::Int packet_id, mc::Buffer &packet) override;

        BaseConnection *handle_packet(mc::BasePacket *packet, BasePacket **response) override;

    };

    class ConnectionLogin : public BaseConnection {
    public:
        explicit ConnectionLogin(const BaseConnection &other) : BaseConnection(other) {}

    protected:
        BasePacket *match_packet(Varint::Int packet_id, mc::Buffer &packet) override;

        BaseConnection *handle_packet(mc::BasePacket *packet, BasePacket **response) override;
    };

}

#endif
