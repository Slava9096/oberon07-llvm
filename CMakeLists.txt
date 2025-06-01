cmake_minimum_required(VERSION 3.18)
project(oberon07_llvm_esp32)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# Find LLVM
set(LLVM_DIR "/usr/lib/cmake/llvm")
# set(LLVM_DIR "/opt/llvm-project/build/lib/cmake/llvm")

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

# Add LLVM include directories
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Add include directories for generated files
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/frontend
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Generate Flex and Bison files
BISON_TARGET(parser
    src/frontend/parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.hpp
)

FLEX_TARGET(lexer
    src/frontend/flex.l
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
)

ADD_FLEX_BISON_DEPENDENCY(lexer parser)

# Add source files
set(SOURCES
    src/main.cpp
    src/frontend/frontend.cpp
    src/middleend/middleend.cpp
    src/backend/backend.cpp
    src/backend/linker.cpp
    src/frontend/ast/base.cpp
    ${BISON_parser_OUTPUTS}
    ${FLEX_lexer_OUTPUTS}
)

# Add header files
set(HEADERS
    src/frontend/frontend.h
    src/middleend/middleend.h
    src/backend/backend.h
    src/backend/linker.h
    src/frontend/ast/base.h
    src/frontend/ast/statement.h
    src/frontend/ast/arithmeticexpression.h
    src/frontend/ast/booleanexpression.h
    src/frontend/ast/lvalue.h
    src/frontend/ast/relationalexpression.h
)

# Create executable
add_executable(oberon07_llvm_esp32 ${SOURCES} ${HEADERS})

# Set include directories for the target
target_include_directories(oberon07_llvm_esp32 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/frontend
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Link against LLVM
target_link_libraries(oberon07_llvm_esp32 PRIVATE
    LLVM
)

# Add compiler definitions
target_compile_definitions(oberon07_llvm_esp32 PRIVATE
    _GNU_SOURCE
    __STDC_CONSTANT_MACROS
    __STDC_FORMAT_MACROS
    __STDC_LIMIT_MACROS
)

# Set output directories
set_target_properties(oberon07_llvm_esp32 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
# Enable testing and include test subdirectory
enable_testing()
add_subdirectory(test)
