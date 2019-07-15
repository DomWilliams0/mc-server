#ifndef MC_SERVER_TYPES_H
#define MC_SERVER_TYPES_H

#include <cstdint>
#include <array>
#include <exception>
#include <memory>

namespace mc {
    enum class ErrorType {
        kIo,
        kTooShort,
        kMemory,
        kBadEnum,
        kUnexpectedRequest,
        kUnexpectedPacketId,
        kEof,
        kNotImplemented,
    };

    class Exception : public std::exception {
    public:
        Exception(ErrorType type, std::string reason, std::string strerror = "") :
                type(type), reason(std::move(reason)), context(std::move(strerror)) {}


        void log() const;

        ErrorType type;
        std::string reason, context;


    };

    class Varint {
    public:
        using Bytes = std::array<uint8_t, 5>;
        using Int = int32_t;

        Varint() : Varint(0) {};

        explicit Varint(Int value);

        explicit Varint(const mc::Varint::Bytes &bytes);


        unsigned int get_byte_count() const;

        Int get_real_value() const;

        const Bytes &get_bytes() const;

        inline Int operator*() const { return get_real_value(); }


    private:
        unsigned int byte_count;
        Int real_value;
        Bytes bytes{};
    };


    typedef uint16_t UShort;
    typedef int64_t Long;

    class String {
    public:
        String() : String(0, nullptr) {}

        // copied
        String(Varint::Int length, char *str);

        inline const char *value() const { return str.get(); }

        inline const char *operator*() const { return value(); }

        inline const Varint &length() const { return len; }

    private:
        std::unique_ptr<char[]> str; // unicode
        Varint len;
    };


}

#endif
