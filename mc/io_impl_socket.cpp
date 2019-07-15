
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include "io.h"
#include "loguru.hpp"
#include "types.h"

namespace mc {
    size_t Socket::read(uint8_t *out, ssize_t n) {
        ssize_t ret = ::read(socket, out, n);
        if (ret < 0) {
            throw Exception(ErrorType::kIo, "socket read", std::strerror(errno));
        }

        return ret;
    }

    ssize_t Socket::write(uint8_t *in, ssize_t n) {
        ssize_t ret = ::write(socket, in, n);
        if (ret < 0) {
            throw Exception(ErrorType::kIo, "socket write", std::strerror(errno));
        }

        return ret;
    }
}
