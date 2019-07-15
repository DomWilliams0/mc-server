#ifndef MC_SERVER_TYPES_H
#define MC_SERVER_TYPES_H

#include <cstdint>
#include <array>

namespace mc {
    enum class ErrorType {
        kIo,
        kTooShort,
        kBadEnum,
        kUnexpectedRequest,
        kNotImplemented
    };

    class Varint {
    public:
        using Bytes = std::array<uint8_t, 5>;
        using Int = int32_t;

        Varint(Int value);

        Varint(const mc::Varint::Bytes &bytes);


        int get_byte_count() const;

        Int get_real_value() const;

        const Bytes &get_bytes() const;


    private:
        Varint() {}

        int byte_count;
        Int real_value;
        Bytes bytes{};
    };


    typedef uint16_t UShort;
    typedef int64_t Long;

    class String {
    public:
        String(Varint::Int length, char *str);

    private:
        Varint len;
        char *str; // unicode
    };


}

#endif
