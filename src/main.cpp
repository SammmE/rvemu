#include "cpu.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

std::vector<uint8_t> load_binary(const std::string &filename) {
  std::ifstream file(filename, std::ios::binary | std::ios::ate);

  if (!file) {
    std::cerr << "Error: Could not open file '" << filename << "'" << std::endl;
    exit(1);
  }

  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  if (size == 0) {
    std::cerr << "Error: File '" << filename << "' is empty." << std::endl;
    exit(1);
  }

  std::vector<uint8_t> buffer(size);
  if (!file.read((char *)buffer.data(), size)) {
    std::cerr << "Error: Could not read file data." << std::endl;
    exit(1);
  }

  return buffer;
}

int main(int argc, char *argv[]) {
  std::vector<uint8_t> program;
  if (argc < 2) {
    program = {
        0x93, 0x00, 0x50, 0x00, // 0x00500093 (addi x1, x0, 5)

        0x13, 0x01, 0xa0, 0x00, // 0x00a00113 (addi x2, x0, 10)

        0xb3, 0x81, 0x20, 0x00, // 0x002081b3 (add x3, x1, x2)

        0x63, 0x00, 0x00, 0x00 // 0x00000063 (beq x0, x0, 0
    };
  } else {
    program = load_binary(argv[1]);
  }

  Cpu cpu(program);

  int cycles = std::min(100, static_cast<int>(program.size()));

  std::cout << cycles << " cycles" << std::endl;

  for (int i = 0; i < cycles; i++) {
    if (cpu.pc >= cpu.memory.size()) {
      std::cout << "PC out of bounds (End of program?)" << std::endl;
      break;
    }

    cpu.tick();
  }

  cpu.dump_regs();

  return 0;
}
