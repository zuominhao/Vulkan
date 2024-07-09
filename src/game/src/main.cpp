#include <e3d/e3d.h>

#include <iostream>

void handleEvent(e3d::WindowEvent event){
    std::cout << "555" << std::endl;
}

void handleEvent1(){
    std::cout << "555" << std::endl;
}

int main() {
  std::string name = "5";
  auto engine = e3d::createEngine("game", 1280, 720);
  e3d::EventListener userFunc = handleEvent;
  engine->addEventListener([name](e3d::WindowEvent event) {
    handleEvent(event);
  });
  engine->run();
  return EXIT_SUCCESS;
}
