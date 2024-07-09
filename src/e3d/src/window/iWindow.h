#pragma once
#include <e3d/e3d.h>

#include <memory>
#include <string>
namespace e3d {

class IWindow {
 public:
  virtual ~IWindow() = default;
  virtual bool pollEvent(WindowEvent event) = 0;
  
};

std::shared_ptr<IWindow> createWindow(const std::string& title, uint32_t width, uint32_t height);
}  // namespace e3d