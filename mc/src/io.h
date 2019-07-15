#ifndef MC_SERVER_IO_H
#define MC_SERVER_IO_H

#include <vector>
#include <cstdint>
#include <array>
#include "types.h"

namespace mc {

    class Socket;

    class Buffer {
    public:
        // blank
        Buffer();

        // copied
        Buffer(const uint8_t *input, size_t len);

        size_t read(uint8_t *out, ssize_t n);

        size_t write(uint8_t *in, ssize_t n);

        template<typename T>
        void read(T &out);


    private:
        std::vector<uint8_t> buf;
        size_t len, capacity, cursor;
    };

    class Socket {
    public:
#ifdef TESTING

        Socket(const uint8_t *input, size_t len) : buffer(input, len) {}

#else
        Socket(int socket) : socket(socket) {}
#endif

        Buffer read();

        void write(const Buffer &in);

    private:
        ssize_t read(uint8_t *out, ssize_t n);

        // helper
        template<size_t n>
        ssize_t read(const std::array<uint8_t, n> &array) {
            return read((uint8_t *) array.data(), n);
        }

        ssize_t write(uint8_t *in, ssize_t n);

#ifdef TESTING
        Buffer buffer;
#else
        int socket;
#endif
    };


    template<>
    void Buffer::read(Varint &out);

    template<>
    void Buffer::read(String &out);

    template<>
    void Buffer::read(UShort &out);

    template<>
    void Buffer::read(Long &out);
}

#endif
