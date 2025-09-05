cmake_minimum_required(VERSION 3.22)
project(zmq-playground LANGUAGES C)

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

add_executable(req src/req.c)
add_executable(rep src/rep.c)
add_executable(pub src/pub.c)
add_executable(sub src/sub.c)

target_link_libraries(req PRIVATE libzmq)
target_link_libraries(rep PRIVATE libzmq)
target_link_libraries(pub PRIVATE libzmq)
target_link_libraries(sub PRIVATE libzmq)

include_directories(include/)
