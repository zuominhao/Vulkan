#include "SDLModule.h"
namespace e3d {

SDLModule::SDLModule(const std::string& title, uint32_t width, uint32_t height) {
  // 初始化 SDL 库，启用视频、定时器和游戏控制器子系统
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
    throw std::runtime_error("SDL_Init failed, " + std::string(SDL_GetError()));
  }

  // 设置 SDL 的输入法显示用户界面的提示
  SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

  // 定义窗口的标志，使用 Vulkan 渲染器，窗口可调整大小，高 DPI 支持
  SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

  // 创建一个 SDL 窗口
  SDL_Window* window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, window_flags);

  // 检查窗口是否创建成功，如果失败则抛出异常
  if (!window) {
    throw std::runtime_error("SDL_CreateWindow failed, " + std::string(SDL_GetError()));
  }

  // 保存创建的窗口指针到类成员变量 sdl_window 中
  sdl_window = window;
}

SDLModule::~SDLModule() {
  if (sdl_window) {
    SDL_DestroyWindow(sdl_window);
  }
  SDL_Quit();
}

SDL_Window* SDLModule::getWindow() const {
  return sdl_window;
}
}  // namespace e3d