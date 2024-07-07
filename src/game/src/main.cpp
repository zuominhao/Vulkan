#include <e3d/e3d.h>

#include <iostream>

int main() {
  auto engine = e3d::createEngine("game", 1280, 720);
  engine->run();
  return EXIT_SUCCESS;
}
