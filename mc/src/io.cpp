#include <stdio.h>
#include <cstring>
#include <netinet/in.h>
#include "io.h"
#include "types.h"
#include "loguru.hpp"

mc::Buffer::Buffer() : len(0), capacity(0), cursor(0) {}

mc::Buffer::Buffer(const uint8_t *input, size_t len) : buf(input, input + len), len(len), capacity(len), cursor(0) {}

size_t mc::Buffer::read(uint8_t *out, ssize_t n) {
    int left = len - cursor;
    if (n > left)
        n = left;

    std::copy(buf.cbegin() + cursor, buf.cbegin() + cursor + n, out);
    cursor += n;
    return n;
}

size_t mc::Buffer::write(uint8_t *in, ssize_t n) {
    // get outta here
    if (n == 0)
        return n;

    size_t new_size = len + n;

    // double size if necessary
    if (new_size > capacity) {
        size_t current_capacity = std::max(capacity, 8ul) * 2;
        size_t new_capacity = std::max(current_capacity, new_size);
        buf.resize(new_capacity);
        capacity = new_capacity;
    }

    std::copy(in, in + n, buf.begin() + len);
    len += n;
    return n;
}

mc::Buffer mc::Socket::read() {

    // read varint length
    mc::Varint::Bytes length_buffer;
    ssize_t n = read(length_buffer);
    if (n < 1)
        throw Exception(ErrorType::kTooShort, "reading packet length");

    mc::Varint length(length_buffer);
    Varint::Int decoded_length = *length;
    DLOG_F(INFO, "packet decoded length is %d (%d bytes)", decoded_length, length.get_byte_count());

    // allocate buffer for full body
    auto *body = new uint8_t[decoded_length];

    // write overread bytes into buffer
    ssize_t overread = n - length.get_byte_count();
    if (overread > 0)
        memcpy(body, length.get_bytes().data() + length.get_byte_count(), overread);

    // read rest of body
    uint8_t *body_write = body + overread;
    auto total_read = overread;
    while (total_read < decoded_length) {
        size_t left = decoded_length - total_read;

        n = read(body_write, left);
        if (n == 0)
            throw Exception(ErrorType::kEof, "reading body");

        total_read += n;
        body_write += n;
        DLOG_F(INFO, "read %zu/%d", total_read, decoded_length);
    }

    Buffer ret = Buffer(body, decoded_length);
    delete[] body;
    return ret;
}

void mc::Socket::write(const mc::Buffer &in) {

    size_t n = write((uint8_t *) in.buf.data(), in.len);
    if (n != in.len) {
        LOG_F(ERROR, "only wrote %zd/%zu", n, in.len);
        throw Exception(ErrorType::kIo, "writing body", std::to_string(n));
    }
}

// -----------

template<>
void mc::Buffer::read(mc::Varint &out) {
    // overread full varint size
    Varint::Bytes bytes;
    ssize_t n = read(bytes.data(), bytes.size());

    if (n < 1) {
        throw Exception(ErrorType::kTooShort, "not enough bytes for varint");
    }

    // decode
    out = Varint(bytes);

    // push back unused bytes
    int overread = n - out.get_byte_count();
    cursor -= overread;
}

template<>
void mc::Buffer::write(const mc::Varint &value) {
    size_t n = write((uint8_t *) value.get_bytes().data(), value.get_byte_count());
    if (n != value.get_byte_count()) {
        throw Exception(ErrorType::kTooShort, "could not write full varint");
    }
}


template<>
void mc::Buffer::read(mc::String &out) {
    // read length
    Varint length;
    read(length);

    // alloc null terminated string
    auto *string = static_cast<uint8_t *>(malloc(*length + 1));
    if (string == nullptr) {
        throw Exception(ErrorType::kMemory, "allocating String");
    }

    // read string
    ssize_t n = read(string, *length);
    if (n != *length) {
        LOG_F(ERROR, "only read %zd/%d bytes of string", n, *length);
        free(string);
        throw Exception(ErrorType::kTooShort, "reading String");
    }

    // null terminate
    string[*length] = 0;

    out = std::move(String(*length, (char *) string));
}

template<>
void mc::Buffer::write(const mc::String &value) {
    const Varint &length = value.length();
    write(length);

    size_t n = write((uint8_t *) *value, *length);
    if ((int) n != *length) {
        throw Exception(ErrorType::kTooShort, "could not write full string", std::to_string(*length));
    }
}

template<>
void mc::Buffer::read(mc::UShort &out) {
    UShort tmp;
    ssize_t n = read((uint8_t *) &tmp, sizeof(tmp));

    if (n != sizeof(tmp)) {
        LOG_F(ERROR, "only read %zd/%lu bytes of UShort", n, sizeof(tmp));
        throw Exception(ErrorType::kTooShort, "reading UShort");
    }

    out = ntohs(tmp);
}

template<>
void mc::Buffer::write(const mc::UShort &value) {
    UShort tmp = htonl(value);

    size_t n = write((uint8_t *) &tmp, sizeof(tmp));
    if (n != sizeof(tmp)) {
        throw Exception(ErrorType::kTooShort, "could not write UShort");
    }
}

template<>
void mc::Buffer::read(mc::Long &out) {
    Long tmp;
    ssize_t n = read((uint8_t *) &tmp, sizeof(tmp));

    if (n != sizeof(tmp)) {
        LOG_F(ERROR, "only read %zd/%lu bytes of Long", n, sizeof(tmp));
        throw Exception(ErrorType::kTooShort, "reading Long");
    }

    out = be64toh(tmp);
}


template<>
void mc::Buffer::write(const mc::Long &value) {
    Long tmp = htobe64(value);

    size_t n = write((uint8_t *) &tmp, sizeof(tmp));
    if (n != sizeof(tmp)) {
        throw Exception(ErrorType::kTooShort, "could not write Long");
    }

}
