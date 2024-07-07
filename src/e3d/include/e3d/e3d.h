#pragma once

#include <stdint.h>

#include <memory>
#include <string>

#include "e3d_def.h"

namespace e3d {

class E3D_EXPORT Engine {
 public:
  Engine(const std::string& title, uint32_t width, uint32_t height);
  ~Engine();
  void run();
};

}  // namespace e3d