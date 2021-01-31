#include "chip8.h"
#include "platform.h"

#include <chrono>
#include <iostream>

int main(int argc, char **argv){
  if (argc != 4){
    std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  int displayScale = std::stoi(argv[1]);
	int cycleDelay = std::stoi(argv[2]);
	char const* romFilename = argv[3];

  Platform platform("CHIP-8 Emulator", DISP_W * displayScale, DISP_H * displayScale, DISP_W, DISP_H);

  Chip8 chip8;
  chip8.loadROM(romFilename);

  int displayPitch = sizeof(chip8.display[0])*DISP_W;
  auto lastCycleTime = std::chrono::high_resolution_clock::now();
  bool quit = false;

  while(!quit){
    quit = platform.ProcessInput(chip8.keyboard);
    auto currentTime = std::chrono::high_resolution_clock::now();
    float delta = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
    if (delta > cycleDelay){
      lastCycleTime = currentTime;
      chip8.cycle();
      platform.Update(chip8.display, displayPitch);
    }
  }
  return 0;
}
