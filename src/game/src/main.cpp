#include <e3d/e3d.hpp>
#include <iostream>

void printc() {
  std::cout << "111111111111111111111111111" << std::endl;
}

int main() {
  printc();
  e3d::Engine engine;

  try {
    engine.Run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}