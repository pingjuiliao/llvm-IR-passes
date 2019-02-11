cmake_minimum_required(VERSION 3.10)

find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_INCLUDE_DIRS})

## set variables
get_filename_component(PROJECT_NAME  ${PROJECT_SOURCE_DIR} NAME)
SET(MAIN fibonacci.c)
SET(MAIN_SRC ${MAIN}.in)

## copy *.c script
configure_file (
    "${PROJECT_SOURCE_DIR}/${MAIN_SRC}"
    "${PROJECT_BINARY_DIR}/${MAIN}"
)

## copy run.sh script
configure_file (
    "${PROJECT_SOURCE_DIR}/run.sh.in"
    "${PROJECT_BINARY_DIR}/run.sh"
)
file(APPEND ${PROJECT_BINARY_DIR}/run.sh
    clang\ -Xclang\ -load\ -Xclang\ ./libLLVM${PROJECT_NAME}.so\ ${MAIN})

add_library( LLVM${PROJECT_NAME} MODULE
    pass.cpp
)

