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

  int32_t imm_i = (int32_t)inst >> 20;
  int32_t imm_s = ((int32_t)(inst & 0xFE000000) >> 20) | ((inst >> 7) & 0x1F);
  int32_t imm_b = ((int32_t)(inst & 0x80000000) >> 19) | ((inst & 0x80) << 4) |
                  ((inst >> 20) & 0x7E0) | ((inst >> 7) & 0x1E);
  int32_t imm_u = inst & 0xFFFFF000;
  int32_t imm_j = ((int32_t)(inst & 0x80000000) >> 11) | (inst & 0xFF000) |
                  ((inst >> 9) & 0x800) | ((inst >> 20) & 0x7FE0);

  std::cout << "Fetched instruction: 0x" << std::hex << std::setw(8)
            << std::setfill('0') << inst << "\n";
  std::cout << "  opcode: 0x" << std::hex << (int)opcode << "\n";
  std::cout << "  rd: x" << std::dec << (int)rd << "\n";
  std::cout << "  funct3: 0x" << std::hex << (int)funct3 << "\n";
  std::cout << "  rs1: x" << std::dec << (int)rs1 << "\n";
  std::cout << "  rs2: x" << std::dec << (int)rs2 << "\n";
  std::cout << "  funct7: 0x" << std::hex << (int)funct7 << "\n";

  switch (opcode) {

    // R TYPE
  case 0x33:
    switch (funct3) {
    case 0x0:
      if (funct7 == 0x00) {
      /* ADD */ } else if (funct7 == 0x20) {
      /* SUB */ }
      break;
    case 0x1:
      /* SLL */
      break;
    case 0x2:
      /* SLT */
      break;
    case 0x3:
      /* SLTU */
      break;
    case 0x4:
      /* XOR */
      break;
    case 0x5:
      if (funct7 == 0x00) {
      /* SRL */ } else if (funct7 == 0x20) {
      /* SRA */ }
      break;
    case 0x6:
      /* OR */
      break;
    case 0x7:
      /* AND */
      break;
    }
    break;

    // I TYPE
  case 0x13:
    switch (funct3) {
    case 0x0:
      /* ADDI: rd = rs1 + imm_i */
      break;
    case 0x1:
      /* SLLI: rd = rs1 << shamt (check imm_i validity) */
      break;
    case 0x2:
      /* SLTI: rd = (rs1 < imm_i) ? 1 : 0 */
      break;
    case 0x3:
      /* SLTIU: rd = (unsigned(rs1) < unsigned(imm_i)) ? 1 : 0 */
      break;
    case 0x4:
      /* XORI: rd = rs1 ^ imm_i */
      break;
    case 0x5:
      // Logic for SRLI vs SRAI depends on the top bits of the immediate
      if ((imm_i & 0x400) == 0) {
      /* SRLI: Logical Right Shift */ } else {
      /* SRAI: Arithmetic Right Shift */ }
      break;
    case 0x6:
      /* ORI: rd = rs1 | imm_i */
      break;
    case 0x7:
      /* ANDI: rd = rs1 & imm_i */
      break;
    }
    break;

    // LOAD (I TYPE)
  case 0x03:
    switch (funct3) {
    case 0x0:
      /* LB: Load Byte (Sign-extended) */
      break;
    case 0x1:
      /* LH: Load Halfword (Sign-extended) */
      break;
    case 0x2:
      /* LW: Load Word */
      break;
    case 0x4:
      /* LBU: Load Byte (Zero-extended) */
      break;
    case 0x5:
      /* LHU: Load Halfword (Zero-extended) */
      break;
    }
    break;

    // S TYPE
  case 0x23:
    // Uses imm_s constructed above
    switch (funct3) {
    case 0x0:
      /* SB: Store Byte */
      break;
    case 0x1:
      /* SH: Store Halfword */
      break;
    case 0x2:
      /* SW: Store Word */
      break;
    }
    break;

    // B TYPE
  case 0x63:
    // Uses imm_b constructed above. Remember to add to PC!
    switch (funct3) {
    case 0x0:
      /* BEQ: if (rs1 == rs2) ... */
      break;
    case 0x1:
      /* BNE: if (rs1 != rs2) ... */
      break;
    case 0x4:
      /* BLT: if (rs1 < rs2) ... (signed) */
      break;
    case 0x5:
      /* BGE: if (rs1 >= rs2) ... (signed) */
      break;
    case 0x6:
      /* BLTU: if (rs1 < rs2) ... (unsigned) */
      break;
    case 0x7:
      /* BGEU: if (rs1 >= rs2) ... (unsigned) */
      break;
    }
    break;

    // LUI (U-Type)
  case 0x37:
    // LUI: Load Upper Immediate. rd = imm_u
    break;

    // AUIPC (U-Type)
  case 0x17:
    // AUIPC: Add Upper Immediate to PC. rd = pc + imm_u
    break;

    // JAL (J-Type)
  case 0x6F:
    // JAL: Jump and Link. rd = pc + 4; pc += imm_j
    break;

    // JALR (I-Type)
  case 0x67:
    // JALR: Jump and Link Register.
    // target = (rs1 + imm_i) & ~1
    // rd = pc + 4; pc = target
    break;

    // SYSTEM (I-Type)
  case 0x73:
    // ECALL and EBREAK share funct3=0, distinguished by imm bits
    if (funct3 == 0x0) {
      if (imm_i == 0x0) {
      /* ECALL */ } else if (imm_i == 0x1) { /* EBREAK */ } }
    break;

  default:
    std::cout << "Unknown Opcode: " << std::hex << opcode << "\n";
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
    std::cout << "x" << std::dec << std::setw(2) << i << ": " << "0x"
              << std::hex << std::setw(8) << std::setfill('0') << regs[i]
              << "  " << "x" << std::dec << std::setw(2) << i + 1 << ": "
              << "0x" << std::hex << std::setw(8) << std::setfill('0')
              << regs[i + 1] << "  " << "x" << std::dec << std::setw(2) << i + 2
              << ": " << "0x" << std::hex << std::setw(8) << std::setfill('0')
              << regs[i + 2] << "  " << "x" << std::dec << std::setw(2) << i + 3
              << ": " << "0x" << std::hex << std::setw(8) << std::setfill('0')
              << regs[i + 3] << "\n";
  }
  std::cout << "PC : 0x" << std::hex << std::setw(8) << std::setfill('0') << pc
            << "\n";
}
