#ifndef MC_SERVER_PACKET_H
#define MC_SERVER_PACKET_H

#include "types.h"
#include "io.h"

#include <utility>
#include <type_traits>

namespace mc {
    struct Field;

    struct Packet {
        explicit Packet(Varint::Int packet_id,
                        std::string name,
                        std::initializer_list<Field *> fields)
                : packet_id(packet_id), name(std::move(name)), fields(fields) {}

        std::string to_string() const;

        Varint packet_id;

        template<typename T>
        T &get_field_value(const std::string &field_name);

    protected:
        std::string name;
        std::vector<Field *> fields;
    };

    struct Field {
        explicit Field(std::string name) : name(std::move(name)) {}

        virtual size_t size() const = 0;

        virtual void read(Buffer &buffer) = 0;

        virtual void write(Buffer &buffer) const = 0;

        template<typename T>
        T &get_value();

        virtual std::string get_value_as_str() const = 0;

        std::string name;
    };

    template<typename T>
    struct FieldWrapper : Field {
        explicit FieldWrapper(const std::string &name) : Field(name) {}

        inline void read(Buffer &buffer) override {
            buffer.read(value);
        }

        inline void write(Buffer &buffer) const override {
            buffer.write(value);
        }

        T value{};
    };

    template<typename T>
    inline T &Field::get_value() {
        auto *wrapper = dynamic_cast<FieldWrapper<T> *>(this);
        if (wrapper == nullptr) {
            throw Exception(ErrorType::kBadField, "wrong type");
        }

        return wrapper->value;
    }

    struct VarintField : FieldWrapper<Varint> {
        using FieldWrapper::FieldWrapper;

        size_t size() const override {
            return value.get_byte_count();
        }

        std::string get_value_as_str() const override {
            return std::to_string(*value);
        }
    };

    struct UShortField : FieldWrapper<UShort> {
        using FieldWrapper::FieldWrapper;

        size_t size() const override {
            return sizeof(value);
        }

        std::string get_value_as_str() const override {
            return std::to_string(value);
        }
    };

    struct LongField : FieldWrapper<Long> {
        using FieldWrapper::FieldWrapper;

        size_t size() const override {
            return sizeof(value);
        }

        std::string get_value_as_str() const override {
            return std::to_string(value);
        }
    };

    struct StringField : FieldWrapper<String> {
        using FieldWrapper::FieldWrapper;

        size_t size() const override {
            const Varint &length = value.length();
            return length.get_byte_count() + length.get_real_value();
        }

        std::string get_value_as_str() const override {
            const char *val = value.value();
            std::string str("\"");
            if (val != nullptr)
                str += val;
            return str + "\"";
        }
    };

    struct PacketServerBound : public Packet {
        using Packet::Packet;

        virtual ~PacketServerBound() = default;

        void read_body(Buffer &buffer);
    };

    struct PacketClientBound : public Packet {
        using Packet::Packet;

        virtual ~PacketClientBound() = default;

        void write(Buffer &buffer);

        Varint calculate_full_length() const;
    };

    struct PacketHandshake : public PacketServerBound {
        PacketHandshake() : PacketServerBound(0x00, "handshake", {
                new VarintField("protocol_version"),
                new StringField("server_address"),
                new UShortField("server_port"),
                new VarintField("next_state"),
        }) {}
    };

    struct PacketEmpty : public PacketServerBound {
        PacketEmpty() : PacketServerBound(0x00, "empty", {}) {}
    };

    struct PacketStatusResponse : public PacketClientBound {
        PacketStatusResponse() : PacketClientBound(0x00, "status", {
                new StringField("status"),
        }) {}
    };

    struct PacketPing : public PacketServerBound {
        PacketPing() : PacketServerBound(0x01, "ping", {
                new LongField("payload"),
        }) {}
    };

    struct PacketPong : public PacketClientBound {
        PacketPong() : PacketClientBound(0x01, "ping", {
                new LongField("payload"),
        }) {}
    };

    template<typename T>
    T &mc::Packet::get_field_value(const std::string &field_name) {
        // TODO ordered map?
        for (Field *field : fields) {
            if (field->name == field_name) {
                return field->get_value<T>();
            }
        }

        throw Exception(ErrorType::kBadField, "no field with the given name", field_name);
    }


}

#endif
