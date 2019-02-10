cmake_minimum_required(VERSION 3.10)

find_package(LLVM REQUIRED CONFIG)

## add definitions
add_definitions(${LLVM_DEFINITIONS})
message("add_definitions(\$\{LLVM_DEFINITIONS\}\n")
message("LLVM_DEFINITIONS is ${LLVM_DEFINITIONS} \n")

## include $ link directories
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
message("include_directories(\$\{LLVM_INCLUDE_DIRS\}\n")
message("link_directories(\$\{LLVM_LIBRARY_DIRS\}\n")

add_subdirectory(magic_value)

