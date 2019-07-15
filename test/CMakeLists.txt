project(mc_test)

set(SOURCES main.cpp catch.hpp test_types.cpp io_impl_tests.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE mc)

add_test(${PROJECT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME})
