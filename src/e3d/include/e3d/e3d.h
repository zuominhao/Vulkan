#pragma once

#include <stdint.h>

#include <iostream>
#include <memory>
#include <string>
#include <functional>
#include "e3d_def.h"

namespace e3d {

struct WindowEvent {
  uint32_t type;
};
using EventListener = std::function<void(WindowEvent)>;

class E3D_EXPORT Engine {
 public:
  virtual ~Engine() = default;
  virtual void run() = 0;
  virtual void addEventListener(EventListener listener)= 0 ;
};

E3D_EXPORT auto createEngine(const std::string title, uint32_t width, uint32_t height) -> std::shared_ptr<Engine>;

}  // namespace e3d