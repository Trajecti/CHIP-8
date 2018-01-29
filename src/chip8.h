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
  const static short NUM_REGISTERS = 16;
  const static short CARRY_REGISTER = 15;
  unsigned char V[NUM_REGISTERS];

  // Black and white graphics on a 64x32 pixel screen
  const static short ROW_PIXELS = 64;
  const static short COL_PIXELS = 32;
  const static short NUM_PIXELS = ROW_PIXELS * COL_PIXELS;
  const static short SPRITE_WIDTH = 8;
  unsigned char gfx[COL_PIXELS][ROW_PIXELS];

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
  const static short NO_WAITING_KEYPRESS = -1;
  short register_key_press;

  void clearScreen(unsigned short);
  void returnFromSubroutine(unsigned short);
  void jumpAddress(unsigned short);
  void callAddress(unsigned short);
  void skip(unsigned short);
  void loadToMemory(unsigned short);
  void addToRegister(unsigned short);
  void arithmeticalOperations(unsigned short);
  void setRegisterI(unsigned short);
  void jumpLinkRegister(unsigned short);
  void rand(unsigned short);
  void draw(unsigned short);
  void skipOnKeyPress(unsigned short);
  void storeKeyPressVal(unsigned short);
  void getDelayTimer(unsigned short);
  void setTimer(unsigned short);
  void addToIndex(unsigned short);
  void setIndexToSprite(unsigned short);
  void storeDecimal(unsigned short);
  void writeToMemory(unsigned short);
  void readToMemory(unsigned short);

  unsigned short font_set[80] = {
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
  std::map<unsigned short, opFunc> opcode_to_instr_fn_map = {
      {0x00E0, &Chip8::clearScreen},
      {0x00EE, &Chip8::returnFromSubroutine},
      {0x1000, &Chip8::jumpAddress},
      {0x2000, &Chip8::callAddress},
      {0x3000, &Chip8::skip},
      {0x4000, &Chip8::skip},
      {0x5000, &Chip8::skip},
      {0x6000, &Chip8::loadToMemory},
      {0x7000, &Chip8::addToRegister},
      {0x8000, &Chip8::arithmeticalOperations},
      {0x9000, &Chip8::skip},
      {0xA000, &Chip8::setRegisterI},
      {0xB000, &Chip8::jumpLinkRegister},
      {0xC000, &Chip8::rand},
      {0xD000, &Chip8::draw},
      {0xE09E, &Chip8::skipOnKeyPress},
      {0xE0A1, &Chip8::skipOnKeyPress},
      {0xF007, &Chip8::getDelayTimer},
      {0xF00A, &Chip8::storeKeyPressVal},
      {0xF015, &Chip8::setTimer},
      {0xF018, &Chip8::setTimer},
      {0xF01E, &Chip8::addToIndex},
      {0xF029, &Chip8::setIndexToSprite},
      {0xF033, &Chip8::storeDecimal},
      {0xF055, &Chip8::writeToMemory},
      {0xF065, &Chip8::readToMemory},
    };

    std::map<char,unsigned short> key_chars = {
        {'1',0x0},{'2',0x1},{'3',0x2},{'4',0x3},
        {'q',0x4},{'w',0x5},{'e',0x6},{'r',0x7},
        {'a',0x8},{'s',0x9},{'d',0xA},{'f',0xB},
        {'q',0xC},{'w',0xD},{'e',0xE},{'r',0xF},
    };
};


#endif
