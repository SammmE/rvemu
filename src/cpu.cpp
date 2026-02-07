#include "cpu.h"
#include <cstdint>
#include <cstdio>
#include <iomanip>
#include <iostream>

const uint32_t UART0_ADDR = 0x10000000;

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

void Cpu::write_reg(uint32_t r, uint32_t val) {
  if (r != 0)
    regs[r] = val;
}

uint32_t Cpu::read_reg(uint32_t r) {
  if (r == 0)
    return 0;
  return regs[r];
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
                  ((inst >> 9) & 0x800) | ((inst >> 20) & 0x7FE);

  printf("Fetched instruction: 0x%08x\n"
         "  opcode: 0x%x\n"
         "  rd:     x%d\n"
         "  funct3: 0x%x\n"
         "  rs1:    x%d\n"
         "  rs2:    x%d\n"
         "  funct7: 0x%x\n"
         "  imm_i:  0x%x\n"
         "  imm_s:  0x%x\n"
         "  imm_b:  0x%x\n"
         "  imm_u:  0x%x\n"
         "  imm_j:  0x%x\n",
         inst, opcode, rd, funct3, rs1, rs2, funct7, imm_i, imm_s, imm_b, imm_u,
         imm_j);

  switch (opcode) {

    // R TYPE
  case 0x33:
    switch (funct3) {
    case 0x0:
      if (funct7 == 0x00) {
        // ADD (R-Type)
        // rd = rs1 + rs2

        write_reg(rd, read_reg(rs1) + read_reg(rs2));
      } else if (funct7 == 0x20) {
        // SUB (R-Type)
        // rd = rs1 - rs2

        write_reg(rd, read_reg(rs1) - read_reg(rs2));
      }
      break;
    case 0x1:
      // Shift Left Logical (SLL) (R-Type)
      // rd = rs1 << rs2

      write_reg(rd,
                read_reg(rs1) << (read_reg(rs2) & 0x1F)); // Limit shift to 0-31
      break;
    case 0x2:
      // Set Less Than (SLT) (R-Type)
      // rd = (rs1 < rs2)?1:0

      write_reg(rd, ((int32_t)read_reg(rs1) < (int32_t)read_reg(rs2)) ? 1 : 0);
      break;
    case 0x3:
      // Set Less Than (U) (SLTU) (R-Type)
      // rd = (rs1 < rs2)?1:0

      write_reg(rd, (read_reg(rs1) < read_reg(rs2)) ? 1 : 0);
      break;
    case 0x4:
      // XOR (R-Type)
      // rd = rs1 ˆ rs2

      write_reg(rd, read_reg(rs1) ^ read_reg(rs2));
      break;
    case 0x5:
      if (funct7 == 0x00) {
        // Shift Right Logical (SRL) (R-Type)
        // rd = rs1 >> rs2

        write_reg(rd, read_reg(rs1) >> (read_reg(rs2) & 0x1F));
      } else if (funct7 == 0x20) {
        // Shift Right Arith (SRA) (R-Type)
        // rd = rs1 >> rs2

        write_reg(rd, ((int32_t)read_reg(rs1)) >> (read_reg(rs2) & 0x1F));
      }
      break;
    case 0x6:
      // OR (R-Type)
      // rd = rs1 | rs2

      write_reg(rd, read_reg(rs1) | read_reg(rs2));
      break;
    case 0x7:
      // AND (R-Type)
      // rd = rs1 & rs2

      write_reg(rd, read_reg(rs1) & read_reg(rs2));
      break;
    }
    break;

    // I TYPE
  case 0x13:
    switch (funct3) {
    case 0x0:
      // ADD Immediate (ADDI) (I-Type)
      // rd = rs1 + imm

      write_reg(rd, read_reg(rs1) + imm_i);
      break;
    case 0x1:
      // Shift Left Logical Imm (SLLI) (I-Type)
      // rd = rs1 << imm[0:4]

      write_reg(rd, read_reg(rs1) << (imm_i & 0x1F));
      break;
    case 0x2:
      // Set Less Than Imm (SLTI) (I-Type)
      // rd = (rs1 < imm)?1:0

      write_reg(rd, (int32_t)read_reg(rs1) < imm_i);
      break;
    case 0x3:
      // Set Less Than Imm (U) (SLTIU) (I-Type)
      // rd = (rs1 < imm)?1:0

      write_reg(rd, read_reg(rs1) < (uint32_t)imm_i);
      break;
    case 0x4:
      // XOR Immediate (XORI) (I-Type)
      // rd = rs1 ˆ imm

      write_reg(rd, read_reg(rs1) ^ imm_i);
      break;
    case 0x5:
      if ((imm_i & 0x400) == 0) {
        // Shift Right Logical Imm (SRLI) (I-Type)
        // rd = rs1 >> imm[0:4]

        write_reg(rd, read_reg(rs1) >> (imm_i & 0x1F));
      } else {
        // Shift Right Arith Imm (SRAI) (I-Type)
        // rd = rs1 >> imm[0:4]

        write_reg(rd, ((int32_t)read_reg(rs1)) >> (imm_i & 0x1F));
      }
      break;
    case 0x6:
      // OR Immediate (ORI) (I-Type)
      // rd = rs1 | imm

      write_reg(rd, read_reg(rs1) | imm_i);
      break;
    case 0x7:
      // AND Immediate (ANDI) (I-Type)
      // rd = rs1 & imm

      write_reg(rd, read_reg(rs1) & imm_i);
      break;
    }
    break;

  // LOAD (I TYPE)
  case 0x03:
    switch (funct3) {
    case 0x0:
      // Load Byte (LB) (I TYPE)
      // rd = M[rs1+imm][0:7]

      write_reg(rd, (int32_t)(int8_t)load8(read_reg(rs1) + imm_i));
      break;

    case 0x1:
      // Load Half (LH) (I TYPE)
      // rd = M[rs1+imm][0:15]

      write_reg(rd, (int32_t)(int16_t)load16(read_reg(rs1) + imm_i));
      break;

    case 0x2:
      // Load Word (LW) (I TYPE)
      // rd = M[rs1+imm][0:31]

      write_reg(rd, load32(read_reg(rs1) + imm_i));
      break;

    case 0x4:
      // Load Byte (U) (LBU) (I TYPE)
      // rd = M[rs1+imm][0:7]

      write_reg(rd, load8(read_reg(rs1) + imm_i));
      break;

    case 0x5:
      // Load Half (U) (LHU) (I TYPE)
      // rd = M[rs1+imm][0:15]

      write_reg(rd, load16(read_reg(rs1) + imm_i));
      break;
    }
    break;

  // S TYPE
  case 0x23:
    switch (funct3) {
    case 0x0:
      // Store Byte (SB) (S TYPE)
      // M[rs1+imm][0:7] = rs2[0:7]

      store8(read_reg(rs1) + imm_s, read_reg(rs2));
      break;

    case 0x1:
      // Store Half (SH) (S TYPE)
      // M[rs1+imm][0:15] = rs2[0:15]

      store16(read_reg(rs1) + imm_s, read_reg(rs2));
      break;

    case 0x2:
      // Store Word (SW) (S TYPE)
      // M[rs1+imm][0:31] = rs2[0:31]

      store32(read_reg(rs1) + imm_s, read_reg(rs2));
      break;
    }
    break;

    // B TYPE
  case 0x63:
    switch (funct3) {
    case 0x0:
      // Branch == (BEQ) (B TYPE)
      // if(rs1 == rs2) PC += imm

      if (read_reg(rs1) == read_reg(rs2)) {
        pc = pc + imm_b - 4; // I will add 4 later!
      }
      break;

    case 0x1:
      // Branch != (BNE) (B TYPE)
      // if(rs1 != rs2) PC += imm
      if (read_reg(rs1) != read_reg(rs2)) {
        pc += imm_b;
      }
      break;

    case 0x4:
      // Branch < (BLT) (B TYPE)
      // if(rs1 < rs2) PC += imm

      if ((int32_t)read_reg(rs1) < (int32_t)read_reg(rs2)) {
        pc += imm_b;
      }
      break;

    case 0x5:
      // Branch >= (BGE) (B TYPE)
      // if(rs1 >= rs2) PC += imm

      if ((int32_t)read_reg(rs1) >= (int32_t)read_reg(rs2)) {
        pc += imm_b;
      }
      break;

    case 0x6:
      // Branch < (U) (BLTU) (B TYPE)
      // if(rs1 < rs2) PC += imm

      if (read_reg(rs1) < read_reg(rs2)) {
        pc += imm_b;
      }
      break;

    case 0x7:
      // Branch >= (U) (BGEU) (B TYPE)
      // if(rs1 >= rs2) PC += imm

      if (read_reg(rs1) >= read_reg(rs2)) {
        pc += imm_b;
      }
      break;
    }
    break;

  case 0x37:
    // Load Upper Imm (LUI) (U Type)
    // rd = imm << 12

    write_reg(rd, imm_u);
    break;

  case 0x17:
    // Add Upper Imm to PC (AUIPC) (U Type)
    // rd = PC + (imm << 12)

    write_reg(rd, pc + imm_u);
    break;

  case 0x6F:
    // Jump And Link (JAL) (J Type)
    // rd = PC + (imm << 12)

    write_reg(rd, pc + 4);
    pc += imm_j;
    break;

  // JALR (I-Type)
  case 0x67:
    // Jump And Link Reg (JALR) (J Type)
    // rd = PC+4; PC = rs1 + imm

    write_reg(rd, pc + 4);
    pc = (read_reg(rs1) + imm_i) &
         ~1; // Mask with ~1 (0xFFFFFFFE) to ensure 2-byte alignment
    break;

  // SYSTEM (I-Type)
  case 0x73:
    // ECALL and EBREAK share funct3=0, distinguished by imm bits
    if (funct3 == 0x0) {
      if (imm_i == 0x0) {
      /* ECALL */ } else if (imm_i == 0x1) { /* EBREAK */
      }
    }
    break;

  default:
    std::cout << "Unknown Opcode: " << std::hex << opcode << "\n";
    break;
  }
}

void Cpu::tick() {
  uint32_t inst = fetch();

  decode_execute(inst);

  pc += 4;
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
  if (addr == UART0_ADDR) {
    std::cout << (char)val;
    std::cout.flush();
    return;
  }

  if (addr >= memory.size()) {
    return;
  }

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
