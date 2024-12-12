cmake_minimum_required(VERSION 3.5.0)
project(RISCV_SIM VERSION 0.1.0 LANGUAGES C CXX)

set (SOURCES
    src/main.cpp
    src/constants.cpp
    src/cache.cpp
    src/instr.cpp
    src/preprocessor.cpp
    src/processor.cpp
    src/utils.cpp
)

add_executable(RISCV_SIM ${SOURCES})
target_include_directories(RISCV_SIM PUBLIC include)

