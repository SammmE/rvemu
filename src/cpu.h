#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include <string>

class Cpu {
public:
    uint32_t pc; // program counter
    std::array<uint32_t, 32> regs; 
    std::vector<uint8_t> memory;
    struct {
        uint32_t mstatus = 0;
        uint32_t mepc = 0;
        uint32_t satp = 0;
    } csrs;

    Cpu(const std::vector<uint8_t>& code);

    uint32_t fetch();
    void decode_execute(uint32_t instruction);
    void tick(); // fetch & execute one instruction

    // load/store
    uint32_t load32(uint32_t addr);
    uint16_t load16(uint32_t addr);
    uint8_t load8(uint32_t addr);
    void store32(uint32_t addr, uint32_t val);
    void store16(uint32_t addr, uint16_t val);
    void store8(uint32_t addr, uint8_t val);
   
    // debugging
    void dump_regs();
};
