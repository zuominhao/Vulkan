#include <e3d/e3d.h>

#include <iostream>

int main() {
  e3d::Engine engine("game", 1280, 720);
  engine.run();
  return EXIT_SUCCESS;
}