#include <e3d/e3d.h>

#include "window/iWindow.h"
namespace e3d {

class E3dImpl : public Engine {
  std::shared_ptr<IWindow> window_;
  EventListener userFunc_;

 public:
  E3dImpl::E3dImpl(const std::string title, uint32_t width, uint32_t height) {
    std::cout << "Engine is created" << std::endl;
    window_ = createWindow(title, width, height);
    std::cout << "Window is created" << std::endl;
  }

  E3dImpl::~E3dImpl() {}

  void E3dImpl::run() {
    bool running = true;
    while (running) {
      WindowEvent event;
      running = window_->pollEvent(event);
      userFunc_(event);
    }
  }
  void addEventListener(EventListener userFunc){
    userFunc_ = userFunc;
  } ;
};

auto createEngine(const std::string title, uint32_t width, uint32_t height) -> std::shared_ptr<Engine> {
  return std::make_shared<E3dImpl>(title, width, height);
};
}  // namespace e3d