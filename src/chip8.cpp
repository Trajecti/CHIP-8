#include "chip8.h"
#include <iostream>

int main() {
  Chip8 test;
  test.initialize();
  //test.emulateCycle();
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

unsigned short Chip8::decodeOpcode(unsigned short opcode) {
  switch (opcode & 0xF000) {
    case 0x0000:
      return opcode & 0xF0FF;
    case 0x8000:
      return opcode & 0xF00F;
    case 0xE000:
      return opcode & 0xF0FF;
    case 0xF000:
      return opcode & 0xF0FF;
    default:
      return opcode & 0xF000;
  };
}

void Chip8::emulateCycle() {
  // Fetch opcode
  opcode = memory[pc] << 8 | memory[pc + 1];
  opFunc opcode_function = opcode_to_instr_fn_map[decodeOpcode(opcode)];
  (this->*opcode_function)(opcode);
}

void Chip8::clearScreen(unsigned short) {
  for (int i = 0; i < NUM_PIXELS; ++i) {
      gfx[i] = 0;
  }
}

void Chip8::returnFromSubroutine(unsigned short) {
    pc = c_stack.front();
    c_stack.pop_front();
    --sp;
}


void Chip8::jumpAddress(unsigned short opcode) {
    unsigned short dest_address = opcode & 0x0FFF;
    pc = dest_address;
    return;
}

void Chip8::callAddress(unsigned short opcode) {
    unsigned short subroutine_address = opcode & 0x0FFF;
    ++sp;
    c_stack.push_front(pc);
    pc = subroutine_address;
    return;
}

// If opcode leads with 0x3000, skip if equal otherwise
//  if 0x4000, skip if not equal. If 0x5000, compare two
//   registers and skip if equal.
void Chip8::skip(unsigned short opcode) {
    unsigned short register1 = opcode & 0x0F00 >> 2;
    unsigned short register2 = opcode & 0x00F0 >> 1;
    unsigned short value = opcode & 0x00FF;
    bool skipIfEqual = ((opcode & 0xF000) == 0x3000);
    bool compareRegisters = ((opcode & 0xF000) == 0x5000);
    if (compareRegisters && (V[register1] == V[register2])) {
        pc+=2;
    } else if ((skipIfEqual && value == V[register1])
        || (!skipIfEqual && value != V[register1])) {
        pc+=2;
    }
    return;
}

