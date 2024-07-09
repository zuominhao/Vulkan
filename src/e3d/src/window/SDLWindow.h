#pragma once
#include <SDL.h>

#include <stdexcept>
#include <string>

#include "IWindow.h"
namespace e3d {

class SDLWindow : public IWindow {
 public:
  SDLWindow(const std::string& title, uint32_t width, uint32_t height);
  ~SDLWindow();
  bool pollEvent(WindowEvent event) override;

 private:
  SDL_Window* window_;
  bool should_close_ = false;
};
}  // namespace e3d