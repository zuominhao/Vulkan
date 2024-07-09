#pragma once
#include <vulkan/vulkan.h>

class Renderer {
 public:
  virtual ~Renderer() {}
  virtual void Render(VkCommandBuffer command_buffer, uint32_t image_index) = 0;
};