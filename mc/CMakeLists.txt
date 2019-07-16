project(mc)

# logging
set(LIB_SOURCES lib/loguru/loguru.cpp lib/loguru/loguru.hpp)

# sources
set(SOURCES
        src/connection/connection.cpp
        src/connection/connection.h
        src/connection/handshake.cpp
        src/connection/status.cpp
        src/connection/login.cpp
        src/packet/packet.cpp
        src/packet/packet.h
        src/packet/cb_pong.cpp
        src/packet/cb_status.cpp
        src/packet/sb_empty.cpp
        src/packet/sb_handshake.cpp
        src/packet/sb_ping.cpp
        src/types.cpp
        src/types.h
        src/io.cpp
        src/io.h
        io_impl_socket.cpp
        src/util.h)


# create library
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${LIB_SOURCES})

# includes
target_include_directories(${PROJECT_NAME}
        PUBLIC
        src/
        lib/loguru
        )
# linkage
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        pthread
        dl
        )

# errors
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
