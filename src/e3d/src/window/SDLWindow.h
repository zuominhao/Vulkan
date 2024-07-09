#pragma once
#include <SDL.h>

#include <stdexcept>
#include <string>

#include "IWindow.h"

class SDLWindow : public IWindow {
 public:
  SDLWindow(const std::string& title, uint32_t width, uint32_t height);
  ~SDLWindow();
  bool pollEvent() override;

 private:
  SDL_Window* window_;
  SDL_Event event_;
  bool should_close_ = false;
};
