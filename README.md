# rvemu

### Running the project
1. To make the build directory: `cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -S . -B build`. NOTE: This project uses Clang, but you cal choose whichever
2. To build the project `make -C build`
3. To run the binary `./build/rvemu <input_files>`

### References used:
https://www.cs.sfu.ca/~ashriram/Courses/CS295/assets/notebooks/RISCV/RISCV_CARD.pdf
