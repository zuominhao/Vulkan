#include "window.h"

#include <iostream>

namespace e3d {
Window::Window(const std::string title, uint32_t width, uint32_t height) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
    throw std::runtime_error("SDL_Init failed, " + std::string(SDL_GetError()));
  }

  SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

  SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  SDL_Window *window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, window_flags);
  if (!window)
    throw std::runtime_error("SDL_CreateWindow failed, " + std::string(SDL_GetError()));

  sdl_window = window;

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