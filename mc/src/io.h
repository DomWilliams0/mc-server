#ifndef MC_SERVER_IO_H
#define MC_SERVER_IO_H

#include <vector>
#include <cstdint>

namespace mc {

    class Buffer {
    public:

    private:
        std::vector<uint8_t> buffer;
        size_t cursor;
    };

    class Socket {
    public:
        void do_thing();
    };

}

#endif
