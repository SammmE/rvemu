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
  if (argc < 2) {
    std::cerr << "Usage: ./rvemu <binary_file>" << std::endl;
    return 1;
  }

  std::vector<uint8_t> code = load_binary(argv[1]);

  Cpu cpu(code);

  int cycles = std::min(100, static_cast<int>(code.size()));

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
