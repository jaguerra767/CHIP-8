#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>
#include <string>
#include <random>
#include <array>
#include <map>

class Chip8{


public:
  Chip8();
  void loadROM(const std::string &file);
  void cycle();
  std::array<uint8_t, 16> keyboard{};
  std::array<uint32_t, 64*32> display{};
  typedef void (Chip8::*MFP)();
  std::map <uint16_t, MFP> opMap;

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
