#ifndef MC_SERVER_PACKET_H
#define MC_SERVER_PACKET_H

#include "types.h"

namespace mc {


    struct BasePacket {

        Varint packet_id;

        void read(int fd);

        void write(int fd);


    private:
        Varint::Int calculate_full_length() const;

    protected:
        // length of all packet-specific fields
        virtual Varint::Int calculate_body_length() const = 0;

//        virtual void
    };


}

#endif
