#include <e3d/e3d.h>

#include "window/window.h"
namespace e3d {

class E3dImpl : public Engine {
  std::shared_ptr<Window> window_;

 public:
  E3dImpl::E3dImpl(const std::string title, uint32_t width, uint32_t height) {
    std::cout << "Engine is created" << std::endl;
    window_ = std::make_shared<Window>(title, width, height);
    std::cout << "Window is created" << std::endl;
  };

  E3dImpl::~E3dImpl() {};
  void E3dImpl::run() {
    bool quit = false;
    while (!quit) {
      SDL_Event event;
      while (window_->pollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          quit = true;
        } else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
          quit = true;
        }
      }

      // gpu_->Render([this](VkCommandBuffer command_buffer, uint32_t image_index) { scene_renderer_->Render(command_buffer, image_index); });
    }
  };
};

auto createEngine(const std::string title, uint32_t width, uint32_t height) -> std::shared_ptr<Engine> {
  return std::make_shared<E3dImpl>(title, width, height);
};
}  // namespace e3d