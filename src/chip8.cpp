#include <fstream>
#include <iostream>
#include <random>
#include "chip8.h"



std::array<uint8_t, FONTSET_SIZE> fontset = { 0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
                                              0x20, 0x60, 0x20, 0x20, 0x70, // 1
                                              0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
                                            	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
                                            	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
                                            	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
                                            	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
                                            	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
                                            	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
                                            	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
                                            	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
                                            	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
                                            	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
                                            	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
                                            	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
                                            	0xF0, 0x80, 0xF0, 0x80, 0x80 }; // F
Chip8::Chip8(){
  //Create opcode to function pointer
  opMap.insert(std::make_pair(0x0000, &Chip8::op0));
  opMap.insert(std::make_pair(0x1000, &Chip8::i1nnn));
  opMap.insert(std::make_pair(0x2000, &Chip8::i2nnn));
  opMap.insert(std::make_pair(0x3000, &Chip8::i3xkk));
  opMap.insert(std::make_pair(0x4000, &Chip8::i4xkk));
  opMap.insert(std::make_pair(0x5000, &Chip8::i5xy0));
  opMap.insert(std::make_pair(0x6000, &Chip8::i6xkk));
  opMap.insert(std::make_pair(0x7000, &Chip8::i7xkk));
  opMap.insert(std::make_pair(0x8000, &Chip8::op8));
  opMap.insert(std::make_pair(0x9000, &Chip8::i9xy0));
  opMap.insert(std::make_pair(0xA000, &Chip8::iAnnn));
  opMap.insert(std::make_pair(0xB000, &Chip8::iBnnn));
  opMap.insert(std::make_pair(0xC000, &Chip8::iCxkk));
  opMap.insert(std::make_pair(0xD000, &Chip8::iDxyn));
  opMap.insert(std::make_pair(0xE000, &Chip8::opE));
  opMap.insert(std::make_pair(0xF000, &Chip8::opF));

  opMap0.insert(std::make_pair(0x0, &Chip8::i00E0));
  opMap0.insert(std::make_pair(0xE, &Chip8::i00EE));

  opMap8.insert(std::make_pair(0x0, &Chip8::i8xy0));
  opMap8.insert(std::make_pair(0x1, &Chip8::i8xy1));
  opMap8.insert(std::make_pair(0x2, &Chip8::i8xy2));
  opMap8.insert(std::make_pair(0x3, &Chip8::i8xy3));
  opMap8.insert(std::make_pair(0x4, &Chip8::i8xy4));
  opMap8.insert(std::make_pair(0x5, &Chip8::i8xy5));
  opMap8.insert(std::make_pair(0x6, &Chip8::i8xy6));
  opMap8.insert(std::make_pair(0x7, &Chip8::i8xy7));
  opMap8.insert(std::make_pair(0xE, &Chip8::i8xyE));

  opMapE.insert(std::make_pair(0x1, &Chip8::iExA1));
  opMapE.insert(std::make_pair(0xE, &Chip8::iEx9E));

  opMapF.insert(std::make_pair(0x07, &Chip8::iFx07));
  opMapF.insert(std::make_pair(0x0A, &Chip8::iFx0A));
  opMapF.insert(std::make_pair(0x15, &Chip8::iFx15));
  opMapF.insert(std::make_pair(0x18, &Chip8::iFx18));
  opMapF.insert(std::make_pair(0x1E, &Chip8::iFx1E));
  opMapF.insert(std::make_pair(0x29, &Chip8::iFx29));
  opMapF.insert(std::make_pair(0x33, &Chip8::iFx33));
  opMapF.insert(std::make_pair(0x55, &Chip8::iFx55));
  opMapF.insert(std::make_pair(0x65, &Chip8::iFx65));

  //Initialize program counter
  programCounter = PROG_START_ADDR;

  //load fontset to memory
  std::copy(fontset.begin(), fontset.end(), ram.begin()+FONTSET_START_ADDR);
  //Initialize RNG
  randByte = std::uniform_int_distribution<uint8_t>(0,155U);
  generator.seed(device());
}

void Chip8::loadROM(const std::string &filename){
  //Open file as a binary stream and move ptr to the end
  std::ifstream file(filename, std::ios::binary | std::ios::ate);

  if(file.is_open()){
    //get size of file
    std::streampos size = file.tellg();
    //allocate memory for a buffer to hold contents of file
    char* buffer = new char[size];

    //Move pointer to the beginning of the file to fill the buffer
    file.seekg(0, std::ios::beg);
    file.read(buffer, size);
    file.close();
    //Copy buffer into memory
    for (long i = 0; i<size; ++i){
      ram[PROG_START_ADDR+i] = buffer[i];
    }
    //free memory because this is what we do in c++
    delete[] buffer;
  }
}

void Chip8::cycle(){
  //Decode opcode
  opcode = (ram[programCounter] << 8u) | ram[programCounter + 1];
  //Increment programCounter

  programCounter += 2;
  //Retrieve function pointer from opMap
  auto itMap = opMap.find(opcode & OP_CODE_MASK);
  if (itMap != opMap.end()){
    MFP operation = opMap[(opcode & OP_CODE_MASK)];
    std::invoke(operation,this);
  }
  //Sound and Delay decrements
  if (delay > 0){
    --delay;
  }
  if (sound > 0)
  {
    --sound;
  }
}

void Chip8::op0(){
  auto itMap0 = opMap0.find(opcode & 0x000Fu);
  if (itMap0 != opMap0.end()){
    MFP operation0 = opMap0[opcode & 0x000Fu];
    std::invoke(operation0,this);
  }
}

void Chip8::op8(){
  auto itMap8 = opMap8.find(opcode & 0x000Fu);
  if (itMap8 != opMap8.end()){
    MFP operation8 = opMap8[opcode & 0x000Fu];
    std::invoke(operation8, this);
  }
}

void Chip8::opE(){
  auto itMapE = opMapE.find(opcode & 0x000Fu);
  if (itMapE != opMapE.end()){
    MFP operationE = opMapE[opcode & 0x000Fu];
    std::invoke(operationE, this);
}
}
void Chip8::opF(){
  auto itMapF = opMapF.find(opcode & 0x00FFu);
  if (itMapF != opMapF.end()){
    MFP operationF = opMapF[opcode & 0x00FFu];
    std::invoke(operationF, this);
}
}
void Chip8::i00E0(){
  memset(display, 0, sizeof(display));

}

void Chip8::i00EE(){
  //The interpreter sets the program counter to the
  //address at the top of the stack, then subtracts 1 from the stack pointer.
  --stackPointer;
  programCounter =  stack[stackPointer];
}
void Chip8::i1nnn(){
  //(JP addr) Jump to location nnn
  //The interpreter sets the program counter to i1nnn
  uint16_t address = opcode & NNN_MASK;
  programCounter = address;
}
void Chip8::i2nnn(){
  //The interpreter increments the stack pointer, then then puts the
  //current PC on the top of the stack. The PC is then set to nnn.
  uint16_t address = opcode & NNN_MASK;
  stack[stackPointer] = programCounter;
  ++stackPointer;
  programCounter = address;
}
void Chip8::i3xkk(){
  //The interpreter compares register Vx to kk and if they are equal
  //increments the program counter by 2
  uint8_t Vx = (opcode & VX_MASK)>>8u;
  uint8_t kk = opcode & KK_MASK;
  if (registers[Vx]==kk){
    programCounter+=2;
  }
}
void Chip8::i4xkk(){
  //The interpreter compares register Vx to kk and if they are not equal
  //increments the program counter by 2
  uint8_t Vx = (opcode & VX_MASK)>>8u;
  uint8_t kk = opcode & KK_MASK;
  if (registers[Vx]!=kk){
    programCounter+=2;
  }
}
void Chip8::i5xy0(){
  //skip next instruction if Vx=Vy
  uint8_t Vx = (opcode & VX_MASK)>>8u;
  uint8_t Vy = (opcode & VY_MASK)>>4u;
  if (registers[Vx]==registers[Vy]){
    programCounter+=2;
  }
}
void Chip8::i6xkk(){
  //(LD Vx, byte)set Vx = kk
  uint8_t Vx = (opcode & VX_MASK)>>8u;
  uint8_t kk = opcode & KK_MASK;
  registers[Vx] = kk;
}
void Chip8::i7xkk(){
  //(ADD) set Vx = Vx+kk
  uint8_t Vx = (opcode & VX_MASK)>>8u;
  uint8_t kk = opcode & KK_MASK;
  registers[Vx]+=kk;
}
void Chip8::i8xy0(){
  // (LD Vx, Vy) set Vx = Vy
  uint8_t Vx = (opcode & VX_MASK)>>8u;
  uint8_t Vy = (opcode & VY_MASK)>>4u;
  registers[Vx]=registers[Vy];
}
void Chip8::i8xy1(){
  //(OR Vx, Vy) set Vx = Vx OR Vy
  uint8_t Vx = (opcode & VX_MASK)>>8u;
  uint8_t Vy = (opcode & VY_MASK)>>4u;
  registers[Vx] |= registers[Vy];
}
void Chip8::i8xy2(){
  //(AND Vx, Vy)
  uint8_t Vx = (opcode & VX_MASK)>>8u;
  uint8_t Vy = (opcode & VY_MASK)>>4u;
  registers[Vx] &= registers[Vy];
}
void Chip8::i8xy3(){
  //(XOR)
  uint8_t Vx = (opcode & VX_MASK)>>8u;
  uint8_t Vy = (opcode & VY_MASK)>>4u;
  registers[Vx] ^= registers[Vy];
}
void Chip8::i8xy4(){
  //(ADD w/ VF=carry)
  uint8_t Vx = (opcode & VX_MASK)>>8u;
  uint8_t Vy = (opcode & VY_MASK)>>4u;
  uint16_t sum = registers[Vx]+registers[Vy];
  if (sum > 255u){
    registers[0xF]=1;
    }
    else{
      registers[0xF]=0;
    }
    registers[Vx] = sum & 0xFFu;
}
void Chip8::i8xy5(){
  //If Vx > Vy, then VF is set to 1, otherwise 0.
  //Then Vy is subtracted from Vx, and the results stored in Vx.
  uint8_t Vx = (opcode & VX_MASK)>>8u;
  uint8_t Vy = (opcode & VY_MASK)>>4u;
  if (registers[Vx]>registers[Vy]){
    registers[0xF] = 1;
  }
  else {
    registers[0xF] = 0;
  }
  registers[Vx]-=registers[Vy];
}

void Chip8::i8xy6(){
  //If the least-significant bit of Vx is 1, then VF is set to 1,
  //otherwise 0. Then Vx is divided by 2.
  uint8_t Vx = (opcode & VX_MASK)>>8u;
  registers[0xF]=registers[Vx]&0x1u;
  registers[Vx]>>=1;
}

void Chip8::i8xy7(){
  //If Vy > Vx, then VF is set to 1, otherwise 0.
  //Then Vx is subtracted from Vy, and the results stored in Vx.
  uint8_t Vx = (opcode & VX_MASK)>>8u;
  uint8_t Vy = (opcode & VY_MASK)>>4u;
  if (registers[Vy]>registers[Vx]){
    registers[0xF] = 1;
  }
  else {
    registers[0xF] = 0;
  }
  registers[Vx]=registers[Vy]-registers[Vx];
}

void Chip8::i8xyE(){
  uint8_t Vx = (opcode & VX_MASK)>>8u;
  registers[0xF]=registers[Vx]&0x80u;
  registers[Vx]<<=1;
}
void Chip8::i9xy0(){
  // skip next instruction if Vx!= Vy
  uint8_t Vx = (opcode & VX_MASK)>>8u;
  uint8_t Vy = (opcode & VY_MASK)>>4u;
  if(registers[Vx]!=registers[Vy]){
    programCounter +=2;
  }
}
void Chip8::iAnnn(){
  // LD Index, addr , The value of index register is set to i1nnn
  uint16_t address = opcode & NNN_MASK;
  index = address;
}
void Chip8::iBnnn(){
  //The program counter is set to nnn plus the value of V0.
  uint16_t address = opcode & NNN_MASK;
  programCounter = registers[0] + address;
}

void Chip8::iCxkk(){
  // Set Vx = random byte AND kk.
  uint8_t Vx = (opcode & VX_MASK)>>8u;
  uint8_t kk = opcode & KK_MASK;
  registers[Vx] = randByte(generator) & kk;
}

void Chip8::iDxyn(){
  /*Display n-byte sprite starting at memory location I at (Vx, Vy),
  set VF = collision.

*/

  uint8_t Vx = (opcode & VX_MASK)>>8u;
  uint8_t Vy = (opcode & VY_MASK)>>4u;
  uint8_t n = opcode & N_MASK;

  uint8_t xPos = registers[Vx] % DISP_W;//wrap around screen
  uint8_t yPos = registers[Vy] % DISP_H;
  registers[0xF] = 0;
  for (int row = 0; row<n;row++){
    uint8_t spriteByte = ram[index+row];
    for (int col = 0; col < 8; col++ ){
      uint8_t spritePixel = spriteByte & (0x80u >> col);
			uint32_t* screenPixel = &display[(yPos + row) * DISP_W + (xPos + col)];

      //sprite pixel on
      if (spritePixel){
        if (*screenPixel ==0xFFFFFFFF){
          registers[0xF]=1;
        }
        //XOR with sprite pixel.
        *screenPixel ^=0xFFFFFFFF;
      }
    }
  }
}
void Chip8::iEx9E(){
  //Checks the keyboard, and if the key corresponding to the value of Vx is
  //currently in the down position, PC is increased by 2.*/
  uint8_t Vx = (opcode & VX_MASK) >> 8u;
	uint8_t key = registers[Vx];

	if (keyboard[key])
	{
		programCounter += 2;
	}
}
void Chip8::iExA1(){
  //Skips next instruction if key not pressed
  uint8_t Vx = (opcode & VX_MASK) >> 8u;
	uint8_t key = registers[Vx];

	if (!keyboard[key])
	{
		programCounter += 2;
	}
}
void Chip8::iFx07(){
  //Set Vx = delay timer value.
  //The value of DT is placed into Vx.
  uint8_t Vx = (opcode & VX_MASK) >> 8u;
  registers[Vx]=delay;
}
void Chip8::iFx0A(){
  //Wait for a key press, store the value of the key in Vx.

	uint8_t Vx = (opcode & VX_MASK) >> 8u;

	if (keyboard[0])
	{
		registers[Vx] = 0;
	}
	else if (keyboard[1])
	{
		registers[Vx] = 1;
	}
	else if (keyboard[2])
	{
		registers[Vx] = 2;
	}
	else if (keyboard[3])
	{
		registers[Vx] = 3;
	}
	else if (keyboard[4])
	{
		registers[Vx] = 4;
	}
	else if (keyboard[5])
	{
		registers[Vx] = 5;
	}
	else if (keyboard[6])
	{
		registers[Vx] = 6;
	}
	else if (keyboard[7])
	{
		registers[Vx] = 7;
	}
	else if (keyboard[8])
	{
		registers[Vx] = 8;
	}
	else if (keyboard[9])
	{
		registers[Vx] = 9;
	}
	else if (keyboard[10])
	{
		registers[Vx] = 10;
	}
	else if (keyboard[11])
	{
		registers[Vx] = 11;
	}
	else if (keyboard[12])
	{
		registers[Vx] = 12;
	}
	else if (keyboard[13])
	{
		registers[Vx] = 13;
	}
	else if (keyboard[14])
	{
		registers[Vx] = 14;
	}
	else if (keyboard[15])
	{
		registers[Vx] = 15;
	}
	else
	{
		programCounter -= 2;
	}
}

void Chip8::iFx15(){
  //Set delay timer = Vx.
  uint8_t Vx = (opcode & VX_MASK) >> 8u;
  delay = registers[Vx];
}
void Chip8::iFx18(){
  //Set sound timer = Vx
  uint8_t Vx = (opcode & VX_MASK) >> 8u;
  sound = registers[Vx];
}
void Chip8::iFx1E(){
//Set I = I + Vx.
  uint8_t Vx = (opcode & VX_MASK) >> 8u;
  index +=registers[Vx];
}
void Chip8::iFx29(){
  //set index to the value of Vx
  uint8_t Vx = (opcode & VX_MASK) >> 8u;
	index = FONTSET_START_ADDR + (5 * registers[Vx]);
}
void Chip8::iFx33(){
  /*Store BCD representation of Vx in memory locations I, I+1, and I+2.

  The interpreter takes the decimal value of Vx, and places the hundreds digit in
  memory at location in I, the tens digit at location I+1,
  and the ones digit at location I+2.*/
  uint8_t Vx = (opcode & VX_MASK) >> 8u;
  uint8_t value = registers[Vx];
  // Ones
  ram[index + 2] = value % 10;
  value /= 10;
  // Tens
  ram[index + 1] = value % 10;
  value /= 10;
  // Hundreds
  ram[index] = value % 10;
}
void Chip8::iFx55(){
  /*Store registers V0 through Vx in memory starting at location I.

  The interpreter copies the values of registers V0 through Vx into memory,
  starting at the address in I.
  */
  uint8_t Vx = (opcode & VX_MASK) >> 8u;
	for (int i = 0; i <= Vx; ++i)
	{
		ram[index + i] = registers[i];
	}
}
void Chip8::iFx65(){
  //Read registers V0 through Vx from memory starting at location I
  uint8_t Vx = (opcode & VX_MASK) >> 8u;
  for (int i = 0; i <= Vx; ++i)
	{
		registers[i] = ram[index + i];
	}
}
