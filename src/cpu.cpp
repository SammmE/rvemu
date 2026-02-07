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

void Cpu::decode_execute(uint32_t inst) {
  uint32_t opcode = inst & 0x7F;
  uint32_t rd = (inst >> 7) & 0x1F;
  uint32_t funct3 = (inst >> 12) & 0x7;
  uint32_t rs1 = (inst >> 15) & 0x1F;
  uint32_t rs2 = (inst >> 20) & 0x1F;
  uint32_t funct7 = (inst >> 25) & 0x7F;

  std::cout << "Fetched instruction: 0x" << std::hex << std::setw(8)
            << std::setfill('0') << inst << "\n";
  std::cout << "  opcode: 0x" << std::hex << (int)opcode << "\n";
  std::cout << "  rd: x" << std::dec << (int)rd << "\n";
  std::cout << "  funct3: 0x" << std::hex << (int)funct3 << "\n";
  std::cout << "  rs1: x" << std::dec << (int)rs1 << "\n";
  std::cout << "  rs2: x" << std::dec << (int)rs2 << "\n";
  std::cout << "  funct7: 0x" << std::hex << (int)funct7 << "\n";

  switch (opcode) {
  case 0x33:

    break;
  }
}

void Cpu::tick() {
  uint32_t inst = fetch();

  decode_execute(inst);
}

uint32_t Cpu::load32(uint32_t addr) {
  if (addr > memory.size() - 4)
    return 0;

  return (uint32_t)memory[addr] | ((uint32_t)memory[addr + 1] << 8) |
         ((uint32_t)memory[addr + 2] << 16) |
         ((uint32_t)memory[addr + 3] << 24);
}

uint16_t Cpu::load16(uint32_t addr) {
  if (addr > memory.size() - 2)
    return 0;
  return (uint16_t)memory[addr] | ((uint16_t)memory[addr + 1] << 8);
}

uint8_t Cpu::load8(uint32_t addr) {
  if (addr >= memory.size())
    return 0;
  return memory[addr];
}

void Cpu::store32(uint32_t addr, uint32_t val) {
  if (addr > memory.size() - 4)
    return;
  memory[addr] = (uint8_t)(val & 0xFF);
  memory[addr + 1] = (uint8_t)((val >> 8) & 0xFF);
  memory[addr + 2] = (uint8_t)((val >> 16) & 0xFF);
  memory[addr + 3] = (uint8_t)((val >> 24) & 0xFF);
}

void Cpu::store16(uint32_t addr, uint16_t val) {
  if (addr > memory.size() - 2)
    return;
  memory[addr] = (uint8_t)(val & 0xFF);
  memory[addr + 1] = (uint8_t)((val >> 8) & 0xFF);
}

void Cpu::store8(uint32_t addr, uint8_t val) {
  if (addr == 0x10000000) {
    std::cout << (char)val;
    return;
  }

  if (addr >= memory.size())
    return;
  memory[addr] = val;
}

void Cpu::dump_regs() {
  std::cout << "\n[Register Dump]\n";
  for (int i = 0; i < 32; i += 4) {
    std::cout << "x" << std::dec << std::setw(2) << i << ": "
              << "0x" << std::hex << std::setw(8) << std::setfill('0')
              << regs[i] << "  "
              << "x" << std::dec << std::setw(2) << i + 1 << ": "
              << "0x" << std::hex << std::setw(8) << std::setfill('0')
              << regs[i + 1] << "  "
              << "x" << std::dec << std::setw(2) << i + 2 << ": "
              << "0x" << std::hex << std::setw(8) << std::setfill('0')
              << regs[i + 2] << "  "
              << "x" << std::dec << std::setw(2) << i + 3 << ": "
              << "0x" << std::hex << std::setw(8) << std::setfill('0')
              << regs[i + 3] << "\n";
  }
  std::cout << "PC : 0x" << std::hex << std::setw(8) << std::setfill('0') << pc
            << "\n";
}
