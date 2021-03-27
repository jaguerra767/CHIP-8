#ifndef PLATFORM_H
#define PLATFORM_H
#include <cstdint>
#include <array>

class SDL_Window;
class SDL_Renderer;
class SDL_Texture;

class Platform
{
public:
  Platform(char const* title, int windowW, int windownH, int textureW, int textureH);
  ~Platform();
  void Update(void const* buffer, int pitch);
  bool ProcessInput(uint8_t *keys);

private:
  SDL_Window* window{};
  SDL_Renderer* renderer{};
  SDL_Texture* texture{};
};
#endif
