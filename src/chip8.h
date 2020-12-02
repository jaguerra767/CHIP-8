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
  //If Vx > Vy, then VF is set to 1, otherwise 0.
  //Then Vy is subtracted from Vx, and the results stored in Vx.
  void i8xy6(); //(SHR) Bit shift right
  //If the least-significant bit of Vx is 1, then VF is set to 1,
  //otherwise 0. Then Vx is divided by 2.
  void i8xy7(); //(SUBN) Set Vx = Vy - Vx, set VF = NOT borrow.
  //If Vy > Vx, then VF is set to 1, otherwise 0.
  //Then Vx is subtracted from Vy, and the results stored in Vx.
  void i8xyE(); //(SHL) bit shift left
  void i9xy0(); // skip next instruction if Vx!= Vy
  void iAnnn(); // LD Index, addr , The value of index register is set to i1nnn
  void iBnnn(); // JP V0 addr, jump to nnn+V0
  //The program counter is set to nnn plus the value of V0.
  void iCxkk(); // Set Vx = random byte AND kk.
  /*The interpreter generates a random number from 0 to 255,
  which is then ANDed with the value kk.
  The results are stored in Vx.
  See instruction 8xy2 for more information on AND.*/
  void iDxyn();/*Display n-byte sprite starting at memory location I at (Vx, Vy),
  set VF = collision.

  The interpreter reads n bytes from memory,
  starting at the address stored in I.
  These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
  Sprites are XORed onto the existing screen.
  If this causes any pixels to be erased,
  VF is set to 1, otherwise it is set to 0.
  If the sprite is positioned so part of it is outside the coordinates of the display,
  it wraps around to the opposite side of the screen.
  See instruction 8xy3 for more information on XOR, and section 2.4,*/
  void iEx9E(); /*Skip next instruction if key with the value of Vx is pressed.

  Checks the keyboard, and if the key corresponding to the value of Vx is
  currently in the down position, PC is increased by 2.*/
  void iExA1();/*Skip next instruction if key with the value of Vx is not pressed.

  Checks the keyboard, and if the key corresponding to the value of Vx
  is currently in the up position, PC is increased by 2.
  */
  void iFx07();//Set Vx = delay timer value.
  //The value of DT is placed into Vx.
  void iFx0A();//Wait for a key press, store the value of the key in Vx.
  //All execution stops until a key is pressed, then the value of that key is stored in Vx.
  void iFx15();//Set delay timer = Vx.
  void iFx18();//Set sound timer = Vx
  void iFx1E();//Set I = I + Vx.
  void iFx29();/*Set I = location of sprite for digit Vx.

  The value of I is set to the location for the hexadecimal sprite corresponding to
  the value of Vx.
  See section 2.4, Display, for more information on the Chip-8 hexadecimal font.*/
  void iFx33();/*Store BCD representation of Vx in memory locations I, I+1, and I+2.

  The interpreter takes the decimal value of Vx, and places the hundreds digit in
  memory at location in I, the tens digit at location I+1,
  and the ones digit at location I+2.*/
  void iFx55();/*Store registers V0 through Vx in memory starting at location I.

  The interpreter copies the values of registers V0 through Vx into memory,
  starting at the address in I.
  */
  void iFx65();/*Read registers V0 through Vx from memory starting at location I.

  The interpreter reads values from memory starting at location I
  into registers V0 through Vx.*/
};
#endif
