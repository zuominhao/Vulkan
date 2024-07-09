#include "SDLWindow.h"

SDLWindow::SDLWindow(const std::string& title, uint32_t width, uint32_t height) : window_(nullptr), should_close_(false) {
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
    SDL_Quit();
    throw std::runtime_error("SDL_CreateWindow failed, " + std::string(SDL_GetError()));
  }

  // 保存创建的窗口指针到类成员变量 window_ 中
  window_ = window;
}

SDLWindow::~SDLWindow() {}

bool SDLWindow::pollEvent() {
  while (SDL_PollEvent(&event_)) {
    if (event_.type == SDL_QUIT) {
      should_close_ = true;
    } else if (event_.type == SDL_WINDOWEVENT && event_.window.event == SDL_WINDOWEVENT_CLOSE) {
      should_close_ = true;
    }
  }

  return !should_close_;
}

std::shared_ptr<IWindow> createWindow(const std::string& title, uint32_t width, uint32_t height) {
  return std::make_shared<SDLWindow>(title, width, height);
};