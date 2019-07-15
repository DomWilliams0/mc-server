#include <memory>

#include <cassert>
#include "types.h"
#include "loguru.hpp"

unsigned int mc::Varint::get_byte_count() const {
    return byte_count;
}

mc::Varint::Int mc::Varint::get_real_value() const {
    return real_value;
}

const mc::Varint::Bytes &mc::Varint::get_bytes() const {
    return bytes;
}

mc::Varint::Varint(mc::Varint::Int value) {
    int n = 0;
    uint32_t in = value;

    do {
        uint8_t next = in & 0x7fu;
        in >>= 7u;

        if (in)
            next |= 0x80u;

        bytes[n++] = next;
    } while (in);

    assert(n <= 5);
    byte_count = n;
    real_value = value;
}

mc::Varint::Varint(const mc::Varint::Bytes &bytes) {
    uint32_t out = 0;
    int i = 0;

    do {
        out |= (uint32_t) (bytes[i] & 0x7fu) << (7u * i);
    } while ((bytes[i++] & 0x80u) != 0);

    std::copy(std::begin(bytes), std::end(bytes), std::begin(this->bytes));
    this->byte_count = i;
    this->real_value = out;
}

void mc::Exception::log() const {
    const char *err;
    switch (type) {
        case ErrorType::kIo:
            err = "Io";
            break;
        case ErrorType::kTooShort:
            err = "TooShort";
            break;
        case ErrorType::kBadEnum:
            err = "BadEnum";
            break;
        case ErrorType::kUnexpectedRequest:
            err = "UnexpectedRequest";
            break;
        case ErrorType::kNotImplemented:
            err = "NotImplemented";
            break;
        case ErrorType::kMemory:
            err = "Memory";
            break;
        case ErrorType::kUnexpectedPacketId:
            err = "UnexpectedPacketId";
            break;
        case ErrorType::kEof:
            err = "Eof";
            break;
    }


    if (context.empty())
        LOG_F(ERROR, "error %s: %s", err, reason.c_str());
    else
        LOG_F(ERROR, "error %s: %s (%s)", err, reason.c_str(), context.c_str());
}

mc::String::String(mc::Varint::Int length, char *str) : len(length) {
    this->str = std::make_unique<char[]>(length + 1);
    std::copy(str, str + length, this->str.get());
    this->str.get()[length] = '\0';
}

