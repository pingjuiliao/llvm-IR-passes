cmake_minimum_required(VERSION 3.10)

find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_INCLUDE_DIRS})

configure_file (
    "${PROJECT_SOURCE_DIR}/fibonacci.c.in"
    "${PROJECT_BINARY_DIR}/fibonacci.c"
)
configure_file (
    "${PROJECT_SOURCE_DIR}/cmd.sh.in"
    "${PROJECT_BINARY_DIR}/cmd.sh"
)

add_library( LLVMPrintFunc MODULE
    print_func.cpp
)

