#pragma once
#include <memory>
#include <string>
class IWindow {
 public:
  virtual ~IWindow() = default;
  virtual bool pollEvent() = 0;
};

std::shared_ptr<IWindow> createWindow(const std::string& title, uint32_t width, uint32_t height);
