#include <memory>
#include <string>
namespace e3d {

class Window {
 private:
  std::string title;
  uint32_t width;
  uint32_t height;

 public:
  Window(const std::string title, uint32_t width, uint32_t height);
  ~Window();

  auto makeWindow(const std::string title, uint32_t width, uint32_t height) -> std::shared_ptr<Window>;
};

}  // namespace e3d