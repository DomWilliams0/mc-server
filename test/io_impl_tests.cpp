
#include <cstdio>
#include "io.h"

namespace mc {
    size_t Socket::read(uint8_t *out, ssize_t n) {
        return buffer.read(out, n);
    }

    ssize_t Socket::write(uint8_t *in, ssize_t n) {
        return buffer.write(in, n);
    }
}