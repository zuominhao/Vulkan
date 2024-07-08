#include "window.h"

#include <iostream>

namespace e3d {
Window::Window(const std::string title, uint32_t width, uint32_t height) {
  auto sdl_module_ = std::make_shared<Window>(title, width, height);
  // 获取支持的vulkan扩展
  // uint32_t extensions_count = 0;
  // SDL_Vulkan_GetInstanceExtensions(sdl_window, &extensions_count, nullptr);
  // std::vector<const char *> extensions(extensions_count);
  // SDL_Vulkan_GetInstanceExtensions(sdl_window, &extensions_count, extensions.data());
  // vulkan_extensions = std::move(extensions);
}
Window::~Window() {}

bool Window::pollEvent(SDL_Event *event) { return SDL_PollEvent(event); }

// auto Window::makeWindow(const std::string title, uint32_t width, uint32_t height) -> std::shared_ptr<Window> {
//   return std::make_shared<Window>(title, width, height);
// };
}  // namespace e3d