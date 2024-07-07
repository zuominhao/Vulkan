#include "window.h"

#include <iostream>

namespace e3d {
Window::Window(const std::string title, uint32_t width, uint32_t height) {};
Window::~Window() {};

auto Window::makeWindow(const std::string title, uint32_t width, uint32_t height) -> std::shared_ptr<Window> {
  return std::make_shared<Window>(title, width, height);
};
}  // namespace e3d