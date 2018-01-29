#include "chip8.h"
#include "SDL2/SDL.h"



std::map<SDL_Keycode,char> chip8_keys = {
    {SDLK_1,'1'},{SDLK_2,'2'},{SDLK_3,'3'}, {SDLK_4,'4'},
    {SDLK_q,'q'},{SDLK_w,'w'},{SDLK_e,'e'}, {SDLK_r,'r'},
    {SDLK_a,'a'},{SDLK_s,'s'},{SDLK_d,'d'}, {SDLK_f,'f'},
    {SDLK_z,'z'},{SDLK_x,'x'},{SDLK_c,'c'}, {SDLK_v,'v'},
};

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
      gfx[i%COL_PIXELS][i/COL_PIXELS] = 0;
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
  register_key_press = NO_WAITING_KEYPRESS;
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

