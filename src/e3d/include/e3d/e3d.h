#pragma once

#include <stdint.h>

#include <iostream>
#include <memory>
#include <string>

#include "e3d_def.h"

namespace e3d {

class E3D_EXPORT Engine {
 public:
  virtual ~Engine() = default;
  virtual void run() = 0;
};

E3D_EXPORT auto createEngine(const std::string title, uint32_t width, uint32_t height) -> std::shared_ptr<Engine>;

}  // namespace e3d