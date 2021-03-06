#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>
#include <string>
#include <random>
#include <array>
#include <map>


const uint16_t PROG_START_ADDR = 0x200;
const uint16_t FONTSET_START_ADDR = 0x50;
const uint8_t  FONTSET_SIZE = 80;
const uint16_t NNN_MASK = 0x0FFFu;
const uint16_t VX_MASK = 0x0F00u;
const uint16_t VY_MASK = 0x00FFu;
const uint16_t KK_MASK = 0x00FFu;
const uint16_t N_MASK = 0x000Fu;
const uint8_t DISP_H = 32;
const uint8_t DISP_W = 64;
const uint16_t OP_CODE_MASK = 0xF000u;
const uint16_t OP_CODE_MASK_B = 0x000Fu;



class Chip8{


public:
  Chip8();
  void loadROM(const std::string &file);
  void cycle();
  uint8_t keyboard[16]{};//go back and make const
  uint32_t display[DISP_W * DISP_H]{};
  typedef void (Chip8::*MFP)();
  std::map <uint16_t, MFP> opMap;
  std::map <uint16_t, MFP> opMap0;
  std::map <uint16_t, MFP> opMap8;
  std::map <uint16_t, MFP> opMapE;
  std::map <uint16_t, MFP> opMapF;

private:
  std::array<uint8_t,4096> ram{}; //4KB of memory from 0x000 to 0xFFF,
//0x000 to 0x1FF should not be used by programs.
  std::array<uint8_t,16> registers{}; //16 8 bit registers V0 to VF can be a flag
  std::array<uint16_t,16> stack{};
  uint16_t index{};//register used to store memory addresses
  uint8_t delay{};
  uint8_t sound{};
  uint16_t programCounter{}; //stores currently executing address
  uint8_t stackPointer{};//points to the top of the stack
  uint16_t opcode{};

  std::random_device device;
  std::mt19937 generator;
  std::uniform_int_distribution<uint8_t> randByte;
  //Opcode functions
  void op0();
  void op8();
  void opE();
  void opF();


  void i00E0(); //(CLS) Clear the display
  void i00EE();//(RET) Return from subroutine
  void i1nnn();//(JP addr) Jump to location nnn
  void i2nnn();//(CALL addr) Call subroutine at nnn
  void i3xkk(); //skip next Instruction if Vx=kk
  void i4xkk(); //skip next instruction if Vx!=kk
  void i5xy0(); //skip next instruction if Vx=Vy
  void i6xkk(); //(LD Vx, byte)set Vx = kk
  void i7xkk(); //(ADD) set Vx = Vx+kk
  void i8xy0(); // (LD Vx, Vy) set Vx = Vy
  void i8xy1(); //(OR Vx, Vy) set Vx = Vx OR Vy
  void i8xy2(); //(AND Vx, Vy)
  void i8xy3(); //(XOR)
  void i8xy4(); //(ADD w/ VF=carry)
  void i8xy5(); // (SUB) Set Vx=Vx-Vy set VF = NOT borrow.
  void i8xy6(); //(SHR) Bit shift right
  void i8xy7(); //(SUBN) Set Vx = Vy - Vx, set VF = NOT borrow.
  void i8xyE(); //(SHL) bit shift left
  void i9xy0(); // skip next instruction if Vx!= Vy
  void iAnnn(); // LD Index, addr , The value of index register is set to i1nnn
  void iBnnn(); // JP V0 addr, jump to nnn+V0
  void iCxkk(); // Set Vx = random byte AND kk.
  void iDxyn();//Display n-byte sprite starting at memory location I at (Vx, Vy),
  //set VF = collision.
  void iEx9E(); //Skip next instruction if key with the value of Vx is pressed.
  void iExA1();//Skip next instruction if key with the value of Vx is not pressed.
  void iFx07();//Set Vx = delay timer value.
  void iFx0A();//Wait for a key press, store the value of the key in Vx.
  void iFx15();//Set delay timer = Vx.
  void iFx18();//Set sound timer = Vx
  void iFx1E();//Set I = I + Vx.
  void iFx29();//Set I = location of sprite for digit Vx.
  void iFx33();//Store BCD representation of Vx in memory locations I, I+1, and I+2.
  void iFx55();//Store registers V0 through Vx in memory starting at location I.
  void iFx65();/*Read registers V0 through Vx from memory starting at location I.

  The interpreter reads values from memory starting at location I
  into registers V0 through Vx.*/
};
#endif
