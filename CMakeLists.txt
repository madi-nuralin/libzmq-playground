cmake_minimum_required(VERSION 3.22)
project(zmq-playground LANGUAGES C CXX)

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall               # enable most warnings
        -Wextra             # enable extra warnings
        -Wpedantic          # standard compliance
        -Wshadow            # warn if shadowing variables
        -Wpointer-arith     # suspicious pointer arithmetic
        -Wcast-align        # warn about misaligned casts
        -Wstrict-prototypes # strict C function prototypes
        -Wmissing-prototypes
#        -Werror             # treat warnings as errors
        -O3                 # optimize
        -g                  # debug symbols
        -fno-common         # catch tentative defs
        -fstack-protector-strong
    )
endif()

add_subdirectory(external)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS
    log
    log_setup
)
include_directories(${Boost_INCLUDE_DIR})

add_executable(req src/req.c)
add_executable(rep src/rep.c)
add_executable(pub src/pub.c)
add_executable(sub src/sub.c)
add_executable(asio_pub src/asio_pub.cpp)
add_executable(asio_sub src/asio_sub.cpp)

target_link_libraries(req PRIVATE libzmq)
target_link_libraries(rep PRIVATE libzmq)
target_link_libraries(pub PRIVATE libzmq)
target_link_libraries(sub PRIVATE libzmq)
target_link_libraries(asio_pub PRIVATE libzmq Boost::log Boost::log_setup)
target_link_libraries(asio_sub PRIVATE libzmq Boost::log Boost::log_setup)

include_directories(include/)
