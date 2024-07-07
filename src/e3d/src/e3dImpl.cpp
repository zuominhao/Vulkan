#include "e3dImpl.h"
namespace e3d {

E3dImpl::E3dImpl(const std::string title, uint32_t width, uint32_t height) {};
E3dImpl::~E3dImpl() {};
void E3dImpl::run() {
  std::cout << "Engine is created" << std::endl;
};

auto createEngine(const std::string title, uint32_t width, uint32_t height) -> std::shared_ptr<Engine> {
  return std::make_shared<E3dImpl>(title, width, height);
};
}  // namespace e3d