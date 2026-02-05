#include "cpu.h"
#include <cstdint>
#include <iomanip>
#include <iostream>

Cpu::Cpu(const std::vector<uint8_t> &code) : pc(0), memory(1024 * 1024, 0) {
  pc = 0;
  regs.fill(0);

  memory.resize(1024 * 1024, 0); // 1 MB memory

  if (code.size() > memory.size()) {
    std::cerr << "Error: Binary too large for memory!" << std::endl;
    exit(1);
  }

  std::copy(code.begin(), code.end(), memory.begin());
}

uint32_t Cpu::fetch() {
  if (pc > memory.size() - 4) {
    std::cerr << "PC out of bounds!!! 0x" << std::hex << pc << std::endl;
    exit(1);
  }

  return (uint32_t)memory[pc] | ((uint32_t)memory[pc + 1] << 8) |
         ((uint32_t)memory[pc + 2] << 16) | ((uint32_t)memory[pc + 3] << 24);
}

void Cpu::decode_execute(uint32_t inst) {}

void Cpu::tick() {
  uint32_t inst = fetch();

  decode_execute(inst);
}
