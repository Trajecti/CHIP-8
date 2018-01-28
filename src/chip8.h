#ifndef CHIP8_H
#define CHIP8_H
#include <map>
#include <deque>

class Chip8;

typedef void (Chip8::*opFunc)(unsigned short);

class Chip8 {
 public:
  void initialize();
  unsigned short decodeOpcode(unsigned short opcode);
  void emulateCycle();

 private:
  const static int INITIAL_PC_VAL = 0x20;
  int drawFlag;

  // CHIP-8 4K system memory mappings
  // 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
  // 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
  // 0x200-0xFFF - Program ROM and work RAM
  const static short MEM_SIZE = 4096;
  unsigned char memory[4096];

  // Index register (idx_register) and program counter (pc)
  // go from 0x000 to 0xFFF
  unsigned short pc;
  unsigned short idx_register;

  // The CHIP-8 has 35 opcodes two bytes long each
  unsigned short opcode;

  // The CHIP-8 has 16 8 bit CPU registers with the
  //  last used for the carry operation.
  const static short NUM_REGISTERS = 15;
  unsigned char V[NUM_REGISTERS];

  // Black and white graphics on a 64x32 pixel screen
  const static short ROW_PIXELS = 64;
  const static short COL_PIXELS = 32;
  const static short NUM_PIXELS = ROW_PIXELS * COL_PIXELS;
  unsigned char gfx[NUM_PIXELS];

  // The CHIP-8 has two timer registers at 60Hz that
  // count down to zero when set above zero
  unsigned char delay_timer;
  unsigned char sound_timer;

  // The CHIP-8 has up to 16 levels of stack for
  //  the stack pointer(sp) to iterate subroutines through
  const static short STACK_SIZE = 16;
  std::deque<unsigned short> c_stack;
  unsigned short sp;

  // The CHIP-8 has a HEX key pad from 0x0 to 0xF
  unsigned char key[16];

  // Values for the CHIP-8 fontset
  const static short FONT_SET_LEN = 80;
  static unsigned char font_set[FONT_SET_LEN];

  void clearScreen(unsigned short opcode);
  void returnFromSubroutine(unsigned short opcode);
  void jumpAddress(unsigned short opcode);
  void callAddress(unsigned short opcode);
  void skip(unsigned short opcode);

  std::map<unsigned short, opFunc> opcode_to_instr_fn_map = {
      {0x00E0, &Chip8::clearScreen},
      {0x00EE, &Chip8::returnFromSubroutine},
      {0x1000, &Chip8::jumpAddress},
      {0x2000, &Chip8::callAddress},
      {0x3000, &Chip8::skip},
      {0x4000, &Chip8::skip},
      {0x5000, &Chip8::skip},
    };
};


unsigned char Chip8::font_set[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
    0x20, 0x60, 0x20, 0x20, 0x70,  // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
    0xF0, 0x80, 0xF0, 0x80, 0x80   // F
};
#endif
