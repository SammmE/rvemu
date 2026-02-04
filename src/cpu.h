#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include <string>

class Cpu {
public:
    uint32_t pc; // program counter
    
    // 32 General Purpose Registers (x0 - x31)
    std::array<uint32_t, 32> regs; 
   
    // RAM
    std::vector<uint8_t> memory;

    Cpu(const std::vector<uint8_t>& code); // binary code to load into memory

    
    uint32_t fetch(); // next 4 bytes
    void execute(uint32_t inst); // decode and run
    void step(); // fetch and execute one instruction
   
    // debugging
    void dump_regs();
};
