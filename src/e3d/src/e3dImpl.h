#pragma once
#include <e3d/e3d.h>

#include <memory>
namespace e3d {

class E3dImpl : public Engine {
 public:
  E3dImpl(const std::string title, uint32_t width, uint32_t height);
  ~E3dImpl();
  void run();
};

}  // namespace e3d