project(mc)

# logging
set(LIB_SOURCES lib/loguru/loguru.cpp lib/loguru/loguru.hpp)

# sources
set(SOURCES src/connection.cpp src/connection.h src/packet.cpp src/packet.h src/types.cpp src/types.h src/io.cpp src/io.h
        io_impl_socket.cpp src/util.h)


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
