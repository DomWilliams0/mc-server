#ifndef MC_SERVER_TYPES_H
#define MC_SERVER_TYPES_H

#include <cstdint>
#include <array>
#include <exception>

namespace mc {
    enum class ErrorType {
        kIo,
        kTooShort,
        kMemory,
        kBadEnum,
        kUnexpectedRequest,
        kEof,
        kNotImplemented,
    };

    class Exception : public std::exception {
    public:
        Exception(ErrorType type, std::string reason, std::string strerror = "") :
                type(type), reason(std::move(reason)), strerror(std::move(strerror)) {}


        void log() const;

        ErrorType type;
        std::string reason, strerror;


    };

    class Varint {
    public:
        using Bytes = std::array<uint8_t, 5>;
        using Int = int32_t;

        Varint() : Varint(0) {};

        Varint(Int value);

        Varint(const mc::Varint::Bytes &bytes);


        int get_byte_count() const;

        Int get_real_value() const;

        const Bytes &get_bytes() const;

        inline Int operator*() const { return get_real_value(); }


    private:
        int byte_count;
        Int real_value;
        Bytes bytes{};
    };


    typedef uint16_t UShort;
    typedef int64_t Long;

    class String {
    public:
        String() : String(0, nullptr) {}

        String(Varint::Int length, char *str);

        inline const char * const value() const { return str; }

    private:
        Varint len;
        char *str; // unicode
    };


}

#endif
