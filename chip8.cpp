#include "chip8.h"

int main() {
  Chip8 test;
  test.initialize();
}

void Chip8::initialize() {
  // Reset all emulation variables
  // CHIP-8 program counter starts at 0x20
  pc = 0x20;
  opcode = 0;
  idx_register = 0;
  sp = 0;

  // Clears display, stack, registers and memory
  //  and given MEM_SIZE is the largest index value
  for (int i = 0; i < MEM_SIZE; ++i) {
    memory[i] = 0;
    if (i < NUM_PIXELS) {
      gfx[i] = 0;
    }
    if (i < NUM_REGISTERS) {
      V[i] = 0;
    }
    if (i < STACK_SIZE) {
      c_stack[i] = 0;
    }
  }

  // Load the fontset
  for (int i = 0; i < FONT_SET_LEN; ++i) {
    memory[i] = font_set[i];
  }

  // Reset timers
  delay_timer = 0;
  sound_timer = 0;
}


opFunc Chip8::decodeOpcode(unsigned short opcode) {
  switch (opcode & 0xF000) {
    case 0x0000:
      switch (opcode & 0x000F) {
        case 0x0000:
          return opcode_to_instr_fn_map[0x00E0];
          break;
        case 0x000E:
          return opcode_to_instr_fn_map[0x00EE];
          break;
      }
  };
}

void Chip8::emulateCycle() {
  // Fetch opcode
  opcode = memory[pc] << 8 | memory[pc + 1];
  opFunc opcode_function = decodeOpcode(opcode);
  (this->*opcode_function)(opcode);
}

void Chip8::clearScreen(unsigned short) {
  for (int i = 0; i < NUM_PIXELS; ++i) {
    gfx[i] = 0;
  }
}
