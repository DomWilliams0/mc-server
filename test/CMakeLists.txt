project(mc_test)

set(SOURCES main.cpp catch.hpp io_impl_tests.cpp
        test_types.cpp test_packets.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE mc)
target_compile_definitions(${PROJECT_NAME} PRIVATE TESTING=1)

add_test(${PROJECT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME})
