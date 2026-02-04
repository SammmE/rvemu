cmake_minimum_required(VERSION 3.10)

project(RiscVEmulator VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wall -Wextra -Werror)

add_executable(riscv_emu 
    src/main.cpp 
    src/cpu.cpp
)
