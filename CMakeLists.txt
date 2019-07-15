cmake_minimum_required(VERSION 3.14)
project(mc_server)

SET(CMAKE_CXX_STANDARD 17)

if (UNIX AND NOT APPLE)
	execute_process(COMMAND ${CMAKE_C_COMPILER} -fuse-ld=gold -Wl,--version ERROR_QUIET OUTPUT_VARIABLE ld_version)
	if ("${ld_version}" MATCHES "GNU gold")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold -Wl,--disable-new-dtags")
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=gold -Wl,--disable-new-dtags")
		message("using gold linker")
	endif()
endif()

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

add_subdirectory(mc)

add_executable(${PROJECT_NAME} server.cpp main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE mc)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)

enable_testing()
add_subdirectory(test)
