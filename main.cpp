#include "io.h"
#include "loguru.hpp"
#include "connection.h"
#include <memory>

static void handle_connection(int fd) {
    mc::Socket socket(fd);
    std::unique_ptr<mc::BaseConnection> connection;

    // starts as handshaking
    connection = std::make_unique<mc::ConnectionHandshaking>(socket);

    try {
        while (connection->keep_alive()) {
            DLOG_F(INFO, "=========== reading a new packet =========== ");
            mc::Buffer packet(socket.read());
            auto new_connection = std::unique_ptr<mc::BaseConnection>(connection->handle_packet(packet));

            if (new_connection != connection) {
                LOG_F(INFO, "changing state from %s to %s",
                      connection->type_name().c_str(), new_connection->type_name().c_str());
                connection.swap(new_connection);
            }
        }
    } catch (const mc::Exception &e) {
        e.log();
    }

}

int main() {
    extern void start_server(int port, void (*handler)(int));
    start_server(25565, handle_connection);
}