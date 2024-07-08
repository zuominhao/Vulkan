#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <memory>
#include <stdexcept>
#include <string>

namespace e3d {
class SDLModule {
 private:
  SDL_Window* sdl_window;

 public:
  SDLModule(const std::string& title, uint32_t width, uint32_t height);
  ~SDLModule();

  SDL_Window* getWindow() const;
};

}  // namespace e3d