#include <memory>
#include <string>
#define SDL_MAIN_HANDLED
#include <SDL.h>
namespace e3d {

class Window {
 private:
  SDL_Window *sdl_window{};
  std::string title;
  uint32_t width;
  uint32_t height;

 public:
  Window(const std::string title, uint32_t width, uint32_t height);
  ~Window();
  bool pollEvent(SDL_Event *event);
};
// auto makeWindow(const std::string title, uint32_t width, uint32_t height) -> std::shared_ptr<Window>;
}  // namespace e3d