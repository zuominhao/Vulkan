#include <e3d/e3d.h>

#include <iostream>

#include "window/window.h"

namespace e3d {
std::shared_ptr<Window> window_;
Engine::Engine(const std::string& title, uint32_t width, uint32_t height) {
  window_->makeWindow(title, width, height);
};
Engine::~Engine() {};

void Engine::run() {
  std::cout << "hello world" << std::endl;
};

}  // namespace e3d
