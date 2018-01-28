#include "chip8.h"
#include <cstdlib>

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
  for (int y = 0; y < ROW_PIXELS; ++y) {
    for (int x = 0; x < COL_PIXELS; ++x) {
      gfx[x][y] = 0;
    }
  }
  pc += 2;
  return;
}

void Chip8::returnFromSubroutine(unsigned short) {
    pc = c_stack.front();
    c_stack.pop_front();
    --sp;
    return;
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
    unsigned short register1 = opcode & 0x0F00 >> 8;
    unsigned short register2 = opcode & 0x00F0 >> 4;
    unsigned short value = opcode & 0x00FF;

    unsigned short leading_opcode = opcode & 0xF000;
    bool registers_equal = (V[register1] == V[register2]);
    bool register_and_val_equal = (V[register1] == value);
    switch (leading_opcode) {
    case 0x3000:
        pc += (4 * register_and_val_equal);
        return;
    case 0x4000:
        pc += (4 * !register_and_val_equal);
        return;
    case 0x5000:
        pc += (4 * registers_equal);
        return;
    case 0x9000:
        pc += (4 * !registers_equal);
        return;
    }
    return;
}

void Chip8::loadToMemory(unsigned short opcode) {
    unsigned short register1 = opcode & 0x0F00 >> 4;
    unsigned short value = opcode & 0x00FF;
    V[register1] = value;
    pc += 2;
    return;
}

void Chip8::addToRegister(unsigned short opcode) {
    unsigned short register1 = opcode & 0x0F00 >> 8;
    unsigned short value = opcode & 0x00FF;
    V[register1] += value;
    pc += 2;
    return;
}

void Chip8::arithmeticalOperations(unsigned short opcode) {
    unsigned short register1 = opcode & 0x0F00 >> 8;
    unsigned short register2 = opcode & 0x00F0 >> 4;
    unsigned short op = opcode & 0x000F;
    enum Operations {ASSIGN,OR,AND,XOR,ADD,SUB,SHR,SUBN,SHL};
    switch(op) {
      case ASSIGN:
        V[register1] = V[register2];
        break;
      case OR:
        V[register1] = V[register1] | V[register2];
        break;
     case AND:
        V[register1] = V[register1] & V[register2];
        break;
     case XOR:
        V[register1] = V[register1] ^ V[register2];
        break;
     case ADD: {
        unsigned short temp = V[register1] + V[register2];
        V[CARRY_REGISTER] = (temp > 255); // 255 is the max integer for 8 bits
        V[register1] = temp & 0xFFFF;
        break; }
     case SUB:
        V[CARRY_REGISTER] = (V[register1] > V[register2]);
        V[register1] = V[register1] - V[register2];
        break;
     case SHR:
        V[CARRY_REGISTER] = ((V[register1] & 0x0001) == 0x0001);
        V[register1] = V[register1] / 2;
        break;
     case SUBN:
        V[CARRY_REGISTER] = (V[register2] > V[register1]);
        V[register1] = V[register2] - V[register1];
        break;
     case SHL:
        V[CARRY_REGISTER] = ((V[register1] & 0x8000) == 0x8000);
        V[register1] = V[register1] * 2;
        break;
    default:
        break;
    };
    pc += 2;
    return;
}

void Chip8::setRegisterI(unsigned short opcode) {
    unsigned short value = opcode & 0x0FFF;
    idx_register = value;
    pc += 2;
    return;
}

void Chip8::jumpLinkRegister(unsigned short opcode) {
    unsigned short value = opcode & 0x0FFF;
    pc  = value + V[0];
    return;
};

void Chip8::rand(unsigned short opcode) {
    unsigned short register1 = opcode & 0x0F00 >> 8;
    unsigned short value = opcode & 0x00FF;
    unsigned short rand_var = std::rand() & 0xFFFF;
    V[register1] = value & rand_var;
    pc += 2;
    return;
}

void Chip8::draw(unsigned short opcode) {
    unsigned short register1 = opcode & 0x0F00 >> 8;
    unsigned short register2 = opcode & 0x00F0 >> 4;
    unsigned short height = opcode & 0x000F;
    unsigned short x_coord = V[register1];
    unsigned short y_coord = V[register2];
    unsigned short sprite_pixel, curr_bit,curr_x,curr_y;
    V[CARRY_REGISTER] = 0;

    for (int y = 0; y < height; ++y) {
        sprite_pixel = memory[idx_register + y];
        for (int x = 0; x < SPRITE_WIDTH; ++x) {
            curr_bit = 0x0080 >> x;
            if ((sprite_pixel & curr_bit) == 1) {
                curr_x = (x+x_coord)%COL_PIXELS;
                curr_y = (y+y_coord)%ROW_PIXELS;
                if (gfx[curr_x][curr_y] == 1) {
                    V[CARRY_REGISTER] = 1;
                }
                gfx[curr_x][curr_y] ^= 1;
            }
        }
    }
    drawFlag = true;
    pc += 2;
    return;
}

void Chip8::skipOnKeyPress(unsigned short opcode) {
    unsigned short register1 = opcode & 0x0F00 >> 8;
    unsigned short instr = opcode & 0x00FF;
    if (((key[V[register1]] != 1) && (instr == 0x009E))
        || ((key[V[register1]] == 1) && (instr == 0x009E))) {
        pc += 2;
    }
    pc += 2;
    return;
}

void Chip8::storeKeyPressVal(unsigned short opcode) {
    unsigned short register1 = opcode & 0x0F00 >> 8;
    while(true){
        char input = std::getchar();
        if (key_chars.count(input)) {
            V[register1] = key_chars[input];
            pc += 2;
            return;
       }
    }
}

void Chip8::getDelayTimer(unsigned short opcode) {
    unsigned short register1 = opcode & 0x0F00 >> 8;
    V[register1] = delay_timer;
    pc += 2;
    return;
}

void Chip8::setTimer(unsigned short opcode) {
    unsigned short register1 = opcode & 0x0F00 >> 8;
    unsigned short instr = opcode & 0x00FF;
    if (instr == 0x15) {
        delay_timer = V[register1];
    } else if (instr == 0x18) {
        sound_timer = V[register1];
    }
    pc += 2;
    return;
}

void Chip8::addToIndex(unsigned short opcode) {
    unsigned short register1 = opcode & 0x0F00 >> 8;
    idx_register += V[register1];
    pc += 2;
    return;
}

void Chip8::setIndexToSprite(unsigned short opcode) {
    unsigned short register1 = opcode & 0x0F00 >> 8;
    idx_register = font_set[5*register1]; // fontset has 5 locations per digit
    pc += 2;
    return;
}

void Chip8::storeDecimal(unsigned short opcode) {
    unsigned short register1 = opcode & 0x0F00 >> 8;
    memory[idx_register] = V[register1] /100;
    memory[idx_register + 1] = (V[register1] % 100) / 10;
    memory[idx_register + 2] = (V[register1] % 10);
    pc += 2;
    return;
}

void Chip8::writeToMemory(unsigned short opcode) {
    unsigned short register1 = opcode & 0x0F00 >> 8;
    for (int j = 0; j <= register1; ++j) {
        memory[idx_register + j] = V[j];
    }
    pc += 2;
    return;
}

void Chip8::readToMemory(unsigned short opcode) {
    unsigned short register1 = opcode & 0x0F00 >> 8;
    for (int j = 0; j <= register1; ++j) {
        V[j] = memory[idx_register + j];
    }
    pc += 2;
    return;
}
