#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_vulkan.h>

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

#include <Eigen/Core>
#include <Eigen/Dense>
#include <vulkan/vulkan.h>

#include <Eigen/Dense>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace e3d {

namespace helper {
inline static std::vector<char> ReadFile(const std::string &filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file!");
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();

  return buffer;
}

VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char> &code) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }

  return shaderModule;
}

inline static std::string ToStr(VkResult err) {
  static const std::unordered_map<VkResult, const char *> vkResultStrings = {
      {VK_SUCCESS, "VK_SUCCESS"},
      {VK_NOT_READY, "VK_NOT_READY"},
      {VK_TIMEOUT, "VK_TIMEOUT"},
      {VK_EVENT_SET, "VK_EVENT_SET"},
      {VK_EVENT_RESET, "VK_EVENT_RESET"},
      {VK_INCOMPLETE, "VK_INCOMPLETE"},
      {VK_ERROR_OUT_OF_HOST_MEMORY, "VK_ERROR_OUT_OF_HOST_MEMORY"},
      {VK_ERROR_OUT_OF_DEVICE_MEMORY, "VK_ERROR_OUT_OF_DEVICE_MEMORY"},
      {VK_ERROR_INITIALIZATION_FAILED, "VK_ERROR_INITIALIZATION_FAILED"},
      {VK_ERROR_DEVICE_LOST, "VK_ERROR_DEVICE_LOST"},
      {VK_ERROR_MEMORY_MAP_FAILED, "VK_ERROR_MEMORY_MAP_FAILED"},
      {VK_ERROR_LAYER_NOT_PRESENT, "VK_ERROR_LAYER_NOT_PRESENT"},
      {VK_ERROR_EXTENSION_NOT_PRESENT, "VK_ERROR_EXTENSION_NOT_PRESENT"},
      {VK_ERROR_FEATURE_NOT_PRESENT, "VK_ERROR_FEATURE_NOT_PRESENT"},
      {VK_ERROR_INCOMPATIBLE_DRIVER, "VK_ERROR_INCOMPATIBLE_DRIVER"},
      {VK_ERROR_TOO_MANY_OBJECTS, "VK_ERROR_TOO_MANY_OBJECTS"},
      {VK_ERROR_FORMAT_NOT_SUPPORTED, "VK_ERROR_FORMAT_NOT_SUPPORTED"},
      {VK_ERROR_FRAGMENTED_POOL, "VK_ERROR_FRAGMENTED_POOL"},
      {VK_ERROR_UNKNOWN, "VK_ERROR_UNKNOWN"},
      {VK_ERROR_OUT_OF_POOL_MEMORY, "VK_ERROR_OUT_OF_POOL_MEMORY"},
      {VK_ERROR_INVALID_EXTERNAL_HANDLE, "VK_ERROR_INVALID_EXTERNAL_HANDLE"},
      {VK_ERROR_FRAGMENTATION, "VK_ERROR_FRAGMENTATION"},
      {VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS, "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS"},
      {VK_PIPELINE_COMPILE_REQUIRED, "VK_PIPELINE_COMPILE_REQUIRED"},
      // 以下是针对特定扩展的 VkResult 值
      {VK_ERROR_SURFACE_LOST_KHR, "VK_ERROR_SURFACE_LOST_KHR"},
      {VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR"},
      {VK_SUBOPTIMAL_KHR, "VK_SUBOPTIMAL_KHR"},
      {VK_ERROR_OUT_OF_DATE_KHR, "VK_ERROR_OUT_OF_DATE_KHR"},
      {VK_ERROR_INCOMPATIBLE_DISPLAY_KHR, "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR"},
      // ... 这里可以继续添加其他扩展的 VkResult 值
      {VK_RESULT_MAX_ENUM, "VK_RESULT_MAX_ENUM"}};

  auto it = vkResultStrings.find(err);
  if (it != vkResultStrings.end()) {
    return std::string(it->second);
  }
  return "Unknown VkResult";
}

inline static bool IsExtensionAvailable(const std::vector<VkExtensionProperties> &properties, const char *extension) {
  for (const VkExtensionProperties &p : properties)
    if (strcmp(p.extensionName, extension) == 0)
      return true;
  return false;
}

inline static bool IsLayerAvailable(const std::vector<VkLayerProperties> &properties, const char *layer) {
  for (const VkLayerProperties &p : properties)
    if (strcmp(p.layerName, layer) == 0)
      return true;
  return false;
}

inline static Eigen::Matrix4f LookAt(const Eigen::Vector3f &eye, const Eigen::Vector3f &center, const Eigen::Vector3f &up) {
  Eigen::Vector3f f = (center - eye).normalized();
  Eigen::Vector3f u = up.normalized();
  Eigen::Vector3f s = f.cross(u).normalized();
  u = s.cross(f);

  Eigen::Matrix4f view = Eigen::Matrix4f::Identity();
  view(0, 0) = s.x();
  view(0, 1) = s.y();
  view(0, 2) = s.z();
  view(1, 0) = u.x();
  view(1, 1) = u.y();
  view(1, 2) = u.z();
  view(2, 0) = -f.x();
  view(2, 1) = -f.y();
  view(2, 2) = -f.z();
  view(0, 3) = -s.dot(eye);
  view(1, 3) = -u.dot(eye);
  view(2, 3) = f.dot(eye);

  return view;
}

inline static Eigen::Matrix4f Perspective(float fov, float aspectRatio, float zNear, float zFar) {
  float tanHalfFov = tan(fov / 2.0f);

  Eigen::Matrix4f proj = Eigen::Matrix4f::Zero();
  proj(0, 0) = 1.0f / (aspectRatio * tanHalfFov);
  proj(1, 1) = 1.0f / tanHalfFov;
  proj(2, 2) = -(zFar + zNear) / (zFar - zNear);
  proj(2, 3) = -1.0f;
  proj(3, 2) = -(2.0f * zFar * zNear) / (zFar - zNear);

  return proj;
}

std::array<float, 4> ColorU32ToF32(uint32_t color) {
  std::array<float, 4> rgba;
  rgba[0] = ((color >> 24) & 0xFF) / 255.0f; // 红通道
  rgba[1] = ((color >> 16) & 0xFF) / 255.0f; // 绿通道
  rgba[2] = ((color >> 8) & 0xFF) / 255.0f;  // 蓝通道
  rgba[3] = ((color >> 0) & 0xFF) / 255.0f;  // Alpha 通道
  return rgba;
}

} // namespace helper

struct Vertex {
  Eigen::Vector2f pos;
  Eigen::Vector3f color;

  static std::pair<VkVertexInputBindingDescription, std::array<VkVertexInputAttributeDescription, 2>> GetBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    return {bindingDescription, attributeDescriptions};
  }
};

struct Uniform {
  alignas(16) Eigen::Matrix4f model;
  alignas(16) Eigen::Matrix4f view;
  alignas(16) Eigen::Matrix4f proj;
};

const std::vector<Vertex> vertices = {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                      {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                                      {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                                      {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};

const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};

struct Frame {
  VkFence fence;
  VkCommandPool command_pool;
  VkCommandBuffer command_buffer;
};

class Pipeline {
public:
  virtual ~Pipeline() {}
};

class Renderer {
public:
  virtual ~Renderer() {}
  virtual void Render(VkCommandBuffer command_buffer, uint32_t image_index) = 0;
};

class Window {
public:
  SDL_Window *sdl_window{};
  std::vector<const char *> vulkan_extensions;

public:
  Window(const std::string &title, int width, int height) {
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
    uint32_t extensions_count = 0;
    SDL_Vulkan_GetInstanceExtensions(sdl_window, &extensions_count, nullptr);
    std::vector<const char *> extensions(extensions_count);
    SDL_Vulkan_GetInstanceExtensions(sdl_window, &extensions_count, extensions.data());
    vulkan_extensions = std::move(extensions);
  }

  VkSurfaceKHR CreateVulkanSurface(VkInstance instance) {
    VkSurfaceKHR surface;
    if (SDL_Vulkan_CreateSurface(sdl_window, instance, &surface) == 0) {
      throw std::runtime_error("SDL_Vulkan_CreateSurface failed, " + std::string(SDL_GetError()));
    }

    return surface;
  }

  using Event = SDL_Event;
  bool PollEvent(Event *event) { return SDL_PollEvent(event); }

  std::pair<int, int> GetSize() {
    int width, height;
    SDL_GetWindowSize(sdl_window, &width, &height);
    return {width, height};
  }

  uint32_t GetWindowId() { return SDL_GetWindowID(sdl_window); }
};

struct GpuContext {
  VkExtent2D extent;
  VkInstance instance{};
  VkSurfaceKHR surface{};
  VkSurfaceFormatKHR surface_format{};
  VkPhysicalDevice physical_device{};
  VkDevice device{};
  uint32_t graphics_family_index;
  VkQueue graphics_queue{};
  uint32_t present_family_index;
  VkQueue present_queue{};
  VkPresentModeKHR present_mode{};

  VkSwapchainKHR swapchain{};
  VkFormat swapchain_image_format{};
  uint32_t swapchain_image_count{};
  std::vector<VkImage> swapchain_images;
  std::vector<VkImageView> swapchain_image_views;

  VkCommandPool command_pool{};
  std::vector<VkCommandBuffer> command_buffers;

  VkFence in_flight_fence;
  VkSemaphore image_available_semaphore;
  VkSemaphore render_finished_semaphore;
};

class Gpu {
  Window *window{};
  VkAllocationCallbacks *allocator{};
  VkDebugReportCallbackEXT debug_report_callback{};
  bool enable_debug_report{true};
  bool enable_fifo{true};

  VkClearValue clear_color = {{0.0f, 0.0f, 0.0f, 1.0f}};

  bool swapchain_rebuild{false};

  std::shared_ptr<GpuContext> context_;

public:
  Gpu(Window *_window) : window(_window) {
    context_ = std::make_shared<GpuContext>();
    context_->extent = {800, 600};

#ifdef WITH_VOLK
    volkInitialize();
    CreateInstance();
    volkLoadInstance(instance);
#else
    CreateInstance();
#endif
    CreateSurface();
    PickPhysicalDevice();
    CreateDevice();
    CreateSwapChain();
    CreateSyncObjects();

    context_->command_pool = CreateCommandPool();

    for (int i = 0; i < context_->swapchain_image_count; ++i) {
      auto command_buffer = CreateCommandBuffer(context_->command_pool);
      context_->command_buffers.push_back(command_buffer);
    }
  }

  uint32_t width() { return context_->extent.width; }
  uint32_t height() { return context_->extent.height; };
  VkFormat image_format() { return context_->swapchain_image_format; }
  uint32_t image_count() { return context_->swapchain_image_count; }

  std::shared_ptr<GpuContext> context() { return context_; }

  VkCommandPool CreateCommandPool() {
    auto graphics_family_index = context_->graphics_family_index;
    auto device = context_->device;

    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = graphics_family_index;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkCommandPool command_pool{};
    auto err = vkCreateCommandPool(device, &pool_info, nullptr, &command_pool);
    if (err != VK_SUCCESS)
      throw std::runtime_error("Failed to create command pool");

    return command_pool;
  }

  VkCommandBuffer CreateCommandBuffer(VkCommandPool command_pool) {
    auto device = context_->device;

    // Allocate command buffer
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer{};
    auto err = vkAllocateCommandBuffers(device, &alloc_info, &command_buffer);
    if (err != VK_SUCCESS)
      throw std::runtime_error("Failed to allocate command buffers");

    return command_buffer;
  }

  VkFramebuffer CreateFramebuffer(VkRenderPass render_pass, uint32_t image_index) {
    auto device = context_->device;
    auto extent = context_->extent;
    auto image_View = context_->swapchain_image_views[image_index];

    VkImageView attachments[] = {image_View};
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = render_pass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;
    VkFramebuffer framebuffer{};
    if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
      throw std::runtime_error("failed to create framebuffer!");

    return framebuffer;
  }

  void Render(std::function<void(VkCommandBuffer, uint32_t)> &&render_func) {
    const auto &instance = context_->instance;
    const auto &device = context_->device;
    const auto &swapchain = context_->swapchain;
    const auto &graphics_queue = context_->graphics_queue;
    const auto &present_queue = context_->present_queue;
    const auto &in_flight_fence = context_->in_flight_fence;
    const auto &image_available_semaphore = context_->image_available_semaphore;
    const auto &render_finished_semaphore = context_->render_finished_semaphore;
    const auto &command_pool = context_->command_pool;

    // 等待CPU同步
    VkResult err = vkWaitForFences(device, 1, &in_flight_fence, VK_TRUE, UINT64_MAX);
    if (err != VK_SUCCESS)
      throw std::runtime_error("vkWaitForFences failed " + helper::ToStr(err));
    err = vkResetFences(device, 1, &in_flight_fence);
    if (err != VK_SUCCESS)
      throw std::runtime_error("vkResetFences failed " + helper::ToStr(err));

    // 请求帧
    uint32_t image_index{};
    err = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, image_available_semaphore, VK_NULL_HANDLE, &image_index);
    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
      swapchain_rebuild = true;
      return;
    } else if (err != VK_SUCCESS) {
      throw std::runtime_error("vkAcquireNextImageKHR failed " + helper::ToStr(err));
    }

    const auto &command_buffer = context_->command_buffers[image_index];

    // 开始录制指令
    VkCommandBufferBeginInfo cmd_info{};
    cmd_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    err = vkBeginCommandBuffer(command_buffer, &cmd_info);
    if (err != VK_SUCCESS)
      throw std::runtime_error("vkBeginCommandBuffer failed " + helper::ToStr(err));

    // 录制指令
    if (render_func)
      render_func(command_buffer, image_index);

    // 结束录制指令
    err = vkEndCommandBuffer(command_buffer);
    if (err != VK_SUCCESS)
      throw std::runtime_error("vkEndCommandBuffer failed " + helper::ToStr(err));

    // 提交渲染
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {image_available_semaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &command_buffer;

    VkSemaphore signalSemaphores[] = {render_finished_semaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphics_queue, 1, &submitInfo, in_flight_fence) != VK_SUCCESS) {
      throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &image_index;

    vkQueuePresentKHR(present_queue, &presentInfo);
  }

  void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory) {
    const auto &device = context_->device;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
      throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
      throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
  }

  void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    auto &device = context_->device;
    auto &commandPool = context_->command_pool;
    auto &graphicsQueue = context_->graphics_queue;

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
  }

private: // Setup
  static VkBool32 debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location,
                               int32_t messageCode, const char *pLayerPrefix, const char *pMessage, void *pUserData) {
    std::cerr << "Debug Report: " << pMessage << std::endl;
    return VK_FALSE;
  }

  void CreateInstance() {
    // 列出可用的扩展
    uint32_t extension_count;
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    std::vector<VkExtensionProperties> available_extensions(extension_count);
    VkResult err = vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, available_extensions.data());
    if (err != VK_SUCCESS)
      std::runtime_error("vkEnumerateInstanceExtensionProperties failed " + helper::ToStr(err));

    // 列出可用的层
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    // 选择需要的层和扩展
    std::vector<char *> extensions{"VK_KHR_surface", "VK_KHR_win32_surface"};
    std::vector<const char *> layers;
    // 添加调试扩展
    if (enable_debug_report) {
      bool has_debug_report_ext = helper::IsExtensionAvailable(available_extensions, VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
      bool has_debug_utils_ext = helper::IsExtensionAvailable(available_extensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
      bool has_validation_layer = helper::IsLayerAvailable(available_layers, "VK_LAYER_KHRONOS_validation");
      if (has_debug_report_ext && has_debug_utils_ext && has_validation_layer) {
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        // extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        layers.push_back("VK_LAYER_KHRONOS_validation");
      }
    }

    // 添加更丰富的设备信息扩展
    if (helper::IsExtensionAvailable(available_extensions, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
      extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

    // 创建实例
    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();
    create_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
    create_info.ppEnabledLayerNames = layers.data();
    VkInstance instance{};
    err = vkCreateInstance(&create_info, allocator, &instance);
    if (err != VK_SUCCESS)
      throw std::runtime_error("vkCreateInstance failed " + helper::ToStr(err));

    // // set debug report callback
    // if (enable_debug_report) {
    //   auto f_vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance,
    //   "vkCreateDebugReportCallbackEXT"); if (f_vkCreateDebugReportCallbackEXT != nullptr) throw std::runtime_error("vkGetInstanceProcAddr failed");

    //   VkDebugUtilsMessengerCreateInfoEXT create_info = {};
    //   create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    //   create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
    //                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    //   create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
    //                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    //   create_info.pfnUserCallback = debugCallback;
    //   create_info.pUserData = nullptr;  // Optional
    //   VkResult err = f_vkCreateDebugReportCallbackEXT(instance, &debug_report_ci, allocator, &debug_report_callback);
    //   if (err != VK_SUCCESS) throw std::runtime_error("vkCreateDebugReportCallbackEXT failed " + helper::ToStr(err));
    // }

    context_->instance = instance;
  }

  void CreateSurface() { context_->surface = window->CreateVulkanSurface(context_->instance); }

  void PickPhysicalDevice() {
    auto instance = context_->instance;

    uint32_t gpu_count;
    VkResult err = vkEnumeratePhysicalDevices(instance, &gpu_count, nullptr);
    if (err != VK_SUCCESS)
      throw std::runtime_error("vkEnumeratePhysicalDevices failed " + helper::ToStr(err));
    if (gpu_count <= 0)
      throw std::runtime_error("No GPU found");

    std::vector<VkPhysicalDevice> gpus(gpu_count);
    err = vkEnumeratePhysicalDevices(instance, &gpu_count, gpus.data());
    if (err != VK_SUCCESS)
      throw std::runtime_error("vkEnumeratePhysicalDevices failed " + helper::ToStr(err));

    // perferring discrete gpu
    VkPhysicalDevice physical_device{};
    for (VkPhysicalDevice &device : gpus) {
      VkPhysicalDeviceProperties properties;
      vkGetPhysicalDeviceProperties(device, &properties);
      if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        physical_device = device;
      }
    }
    if (physical_device == VK_NULL_HANDLE)
      physical_device = gpus[0];

    context_->physical_device = physical_device;
  }

  void CreateDevice() {
    auto surface = context_->surface;
    auto physical_device = context_->physical_device;

    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    auto [graphics_family_index, present_family_index] = FindQueueFamilies(physical_device, surface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {graphics_family_index, present_family_index};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
      VkDeviceQueueCreateInfo queueCreateInfo{};
      queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueCreateInfo.queueFamilyIndex = queueFamily;
      queueCreateInfo.queueCount = 1;
      queueCreateInfo.pQueuePriorities = &queuePriority;
      queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    VkDevice device{};
    if (vkCreateDevice(physical_device, &createInfo, nullptr, &device) != VK_SUCCESS) {
      throw std::runtime_error("failed to create logical device!");
    }

    VkQueue graphics_queue{};
    vkGetDeviceQueue(device, graphics_family_index, 0, &graphics_queue);

    VkQueue present_queue{};
    vkGetDeviceQueue(device, present_family_index, 0, &present_queue);

    context_->device = device;
    context_->graphics_family_index = graphics_family_index;
    context_->present_family_index = present_family_index;
    context_->graphics_queue = graphics_queue;
    context_->present_queue = present_queue;
  }

  void CreateSwapChain() {
    auto surface = context_->surface;
    auto physical_device = context_->physical_device;
    auto device = context_->device;

    // Check for WSI support
    VkBool32 res;
    vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, 0, surface, &res);
    if (res != VK_TRUE)
      throw std::runtime_error("Error: No WSI support on physical device");

    // Select Surface Format
    VkSurfaceFormatKHR surface_format{};
    {
      uint32_t format_count;
      vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr);
      if (format_count == 0)
        throw std::runtime_error("Error: No surface formats available");

      std::vector<VkSurfaceFormatKHR> formats(format_count);
      vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, formats.data());

      bool found = false;
      for (const auto &format : formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
          surface_format = format;
          found = true;
          break;
        }
      }
      if (!found) {
        // Choose a default format if desired format is not available
        surface_format = formats[0];
      }
    }

    // Select Present Mode
    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR; // Default present mode
    {
      uint32_t present_mode_count;
      vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr);
      if (present_mode_count == 0) {
        throw std::runtime_error("Error: No present modes available");
      }
      std::vector<VkPresentModeKHR> present_modes(present_mode_count);
      vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, present_modes.data());

      for (const auto &mode : present_modes) {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
          present_mode = mode;
          break;
        }
      }
    }

    // Get Surface Capabilities
    VkExtent2D extent{context_->extent};
    uint32_t image_count{};
    VkSurfaceTransformFlagBitsKHR pre_transform{};
    {
      VkSurfaceCapabilitiesKHR capabilities;
      vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &capabilities);

      if (capabilities.currentExtent.width != UINT_MAX)
        extent = capabilities.currentExtent;

      image_count = capabilities.minImageCount + 1;
      if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount) {
        image_count = capabilities.maxImageCount;
      }

      pre_transform = capabilities.currentTransform;
    }

    // Create SwapChain
    VkSwapchainKHR swapchain{};
    {
      VkSwapchainCreateInfoKHR swapchain_info = {};
      swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
      swapchain_info.surface = surface;
      swapchain_info.minImageCount = image_count;
      swapchain_info.imageFormat = surface_format.format;
      swapchain_info.imageColorSpace = surface_format.colorSpace;
      swapchain_info.imageExtent = extent;
      swapchain_info.imageArrayLayers = 1;
      swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
      swapchain_info.preTransform = pre_transform;
      swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
      swapchain_info.presentMode = present_mode;
      swapchain_info.clipped = VK_TRUE;
      swapchain_info.oldSwapchain = VK_NULL_HANDLE;

      VkResult err = vkCreateSwapchainKHR(device, &swapchain_info, nullptr, &swapchain);
      if (err != VK_SUCCESS)
        throw std::runtime_error("Failed to create swap chain");
    }

    // Get SwapChain Images
    VkFormat swapchain_image_format{surface_format.format};
    uint32_t swapchain_image_count{};
    std::vector<VkImage> swapchain_images;
    {
      vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, nullptr);
      swapchain_images.resize(swapchain_image_count);
      vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, swapchain_images.data());
    }

    // Create image views
    std::vector<VkImageView> swapchain_image_views(swapchain_images.size());
    {
      for (int i = 0; i < swapchain_images.size(); i++) {
        VkImageViewCreateInfo view_info = {};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = swapchain_images[i];
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = swapchain_image_format;
        view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        VkResult err = vkCreateImageView(device, &view_info, nullptr, &swapchain_image_views[i]);
        if (err != VK_SUCCESS)
          throw std::runtime_error("Failed to create image view");
      }
    }

    context_->extent = extent;
    context_->swapchain = swapchain;
    context_->swapchain_image_format = swapchain_image_format;
    context_->swapchain_image_count = swapchain_image_count;
    context_->swapchain_images = std::move(swapchain_images);
    context_->swapchain_image_views = std::move(swapchain_image_views);
  }

  void CreateSyncObjects() {
    auto device = context_->device;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    VkFence in_flight_fence;
    auto err = vkCreateFence(device, &fenceInfo, nullptr, &in_flight_fence);
    if (err != VK_SUCCESS)
      throw std::runtime_error("Failed to create fence");

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkSemaphore image_available_semaphore;
    err = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &image_available_semaphore);
    if (err != VK_SUCCESS)
      throw std::runtime_error("Failed to create semaphore");
    VkSemaphore render_finished_semaphore;
    err = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &render_finished_semaphore);
    if (err != VK_SUCCESS)
      throw std::runtime_error("Failed to create semaphore");

    context_->in_flight_fence = in_flight_fence;
    context_->image_available_semaphore = image_available_semaphore;
    context_->render_finished_semaphore = render_finished_semaphore;
  }

private:
  std::pair<uint32_t, uint32_t> FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
    uint32_t graphicsFamily{UINT32_MAX};
    uint32_t presentFamily{UINT32_MAX};

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily : queueFamilies) {
      if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        graphicsFamily = i;
      }

      VkBool32 presentSupport = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

      if (presentSupport) {
        presentFamily = i;
      }

      if (graphicsFamily != UINT32_MAX && presentFamily != UINT32_MAX) {
        break;
      }

      i++;
    }

    return {graphicsFamily, presentFamily};
  }

  uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    auto &physicalDevice = context_->physical_device;

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
      if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
        return i;
      }
    }

    throw std::runtime_error("failed to find suitable memory type!");
  }
};

class PointsPipeline : public Pipeline {};

class LinesPipeline : public Pipeline {};

class TrianglesPipeline : public Pipeline {
public:
  VkPipelineLayout pipeline_layout;
  VkPipeline pipeline;

  TrianglesPipeline(VkDevice device, VkRenderPass render_pass, VkDescriptorSetLayout descriptor_set_layout) {
    auto vertShaderCode = helper::ReadFile("base.vert.spv");
    auto fragShaderCode = helper::ReadFile("base.frag.spv");

    VkShaderModule vertShaderModule = helper::CreateShaderModule(device, vertShaderCode);
    VkShaderModule fragShaderModule = helper::CreateShaderModule(device, fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto [bindingDescription, attributeDescriptions] = Vertex::GetBindingDescription();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptor_set_layout;

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipeline_layout) != VK_SUCCESS) {
      throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipeline_layout;
    pipelineInfo.renderPass = render_pass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
      throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
  }

  ~TrianglesPipeline() {}
};

class SceneRenderer : public Renderer {
  std::shared_ptr<Gpu> gpu_;

public:
  VkDevice device{};
  uint32_t width{};
  uint32_t height{};
  VkFormat image_format{};
  uint32_t image_count{};

  VkRenderPass render_pass{};
  std::vector<VkFramebuffer> framebuffers;

  // ubo
  VkDescriptorPool descriptor_pool{};
  VkDescriptorSetLayout descriptor_set_layout{};
  std::vector<VkBuffer> uniformBuffers;
  std::vector<VkDeviceMemory> uniformBuffersMemory;
  std::vector<void *> uniformBuffersMapped;
  std::vector<VkDescriptorSet> descriptor_sets;

  // pipelines
  std::shared_ptr<TrianglesPipeline> triangles_pipeline;

  // vertice
  VkBuffer vertexBuffer{};
  VkDeviceMemory vertexBufferMemory{};
  VkBuffer indexBuffer{};
  VkDeviceMemory indexBufferMemory{};

  SceneRenderer(std::shared_ptr<Gpu> gpu) : gpu_(gpu) {
    device = gpu_->context()->device;
    width = gpu_->width();
    height = gpu_->height();
    image_format = gpu_->image_format();
    image_count = gpu_->image_count();

    CreateRenderPass();
    for (int i = 0; i < image_count; i++)
      framebuffers.push_back(gpu_->CreateFramebuffer(render_pass, i));

    // Uniform
    CreateDescriptorPool();
    CreateDescriptorSetLayout();
    CreateUniformBuffers();
    CreateDescriptorSets();

    // Pipelines
    triangles_pipeline = std::make_shared<TrianglesPipeline>(device, render_pass, descriptor_set_layout);

    CreateVertexBuffer();
    CreateIndexBuffer();
  }

  ~SceneRenderer() {}

  virtual void Render(VkCommandBuffer command_buffer, uint32_t image_index) override {
    const auto &framebuffer = framebuffers[image_index];
    const auto &descriptor_set = descriptor_sets[image_index]; // ubo

    auto color = helper::ColorU32ToF32(0xF3F5FAFF);
    VkClearValue clearColor{};

    UpdateUniformBuffer(image_index);

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = render_pass;
    renderPassInfo.framebuffer = framebuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = {width, height};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(command_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    {
      VkViewport viewport{};
      viewport.x = 0.0f;
      viewport.y = 0.0f;
      viewport.width = (float)width;
      viewport.height = (float)height;
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;
      vkCmdSetViewport(command_buffer, 0, 1, &viewport);

      VkRect2D scissor{};
      scissor.offset = {0, 0};
      scissor.extent = {width, height};
      vkCmdSetScissor(command_buffer, 0, 1, &scissor);

      Draw(command_buffer, triangles_pipeline->pipeline, descriptor_set, vertexBuffer, indexBuffer, 0, indices.size());
    }
    vkCmdEndRenderPass(command_buffer);
  }

private:
  void CreateUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(Uniform);

    uniformBuffers.resize(image_count);
    uniformBuffersMemory.resize(image_count);
    uniformBuffersMapped.resize(image_count);

    for (size_t i = 0; i < image_count; i++) {
      gpu_->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         uniformBuffers[i], uniformBuffersMemory[i]);

      vkMapMemory(device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
    }
  }

  void UpdateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    // 使用 Eigen 创建和操作矩阵

    Eigen::Vector3f translate = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    // Eigen::Vector3f scale = Eigen::Vector3f(1.0f, 1.0f, 1.0f);
    Eigen::Matrix3f rotation = Eigen::AngleAxisf(time * 90.0f * M_PI / 180.0f, Eigen::Vector3f(0.0f, 0.0f, 1.0f)).toRotationMatrix();
    Eigen::Isometry3f model = Eigen::Isometry3f::Identity();
    model.translate(translate);
    model.rotate(rotation);

    Eigen::Matrix4f view = helper::LookAt(Eigen::Vector3f(2.0f, 2.0f, 2.0f), Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector3f(0.0f, 0.0f, 1.0f));
    Eigen::Matrix4f proj = helper::Perspective(45.0f * M_PI / 180.0f, static_cast<float>(width) / height, 0.1f, 10.0f);
    proj(1, 1) *= -1;

    Uniform ubo{};
    ubo.model = Eigen::Matrix4f::Identity();
    ubo.view = Eigen::Matrix4f::Identity();
    ubo.proj = Eigen::Matrix4f::Identity();

    // 假设 uniformBuffersMapped 是一个指向 UBO 内存的指针数组
    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
  }

  void CreateVertexBuffer() {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    gpu_->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                       stagingBuffer, stagingBufferMemory);

    void *data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    gpu_->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                       vertexBuffer, vertexBufferMemory);

    gpu_->CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
  }

  void CreateIndexBuffer() {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    gpu_->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                       stagingBuffer, stagingBufferMemory);

    void *data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    gpu_->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                       indexBuffer, indexBufferMemory);

    gpu_->CopyBuffer(stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
  }

  void CreateRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = image_format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &render_pass) != VK_SUCCESS) {
      throw std::runtime_error("failed to create render pass!");
    }
  }

  void CreateDescriptorPool() {
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = image_count;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = image_count;

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptor_pool) != VK_SUCCESS) {
      throw std::runtime_error("failed to create descriptor pool!");
    }
  }

  void CreateDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptor_set_layout) != VK_SUCCESS) {
      throw std::runtime_error("failed to create descriptor set layout!");
    }
  }

  void CreateDescriptorSets() {
    std::vector<VkDescriptorSetLayout> layouts(image_count, descriptor_set_layout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptor_pool;
    allocInfo.descriptorSetCount = image_count;
    allocInfo.pSetLayouts = layouts.data();

    descriptor_sets.resize(image_count);
    if (vkAllocateDescriptorSets(device, &allocInfo, descriptor_sets.data()) != VK_SUCCESS) {
      throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < image_count; i++) {
      VkDescriptorBufferInfo bufferInfo{};
      bufferInfo.buffer = uniformBuffers[i];
      bufferInfo.offset = 0;
      bufferInfo.range = sizeof(Uniform);

      VkWriteDescriptorSet descriptorWrite{};
      descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrite.dstSet = descriptor_sets[i];
      descriptorWrite.dstBinding = 0;
      descriptorWrite.dstArrayElement = 0;
      descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorWrite.descriptorCount = 1;
      descriptorWrite.pBufferInfo = &bufferInfo;

      vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
    }
  }

  void Draw(VkCommandBuffer command_buffer, VkPipeline pipeline, VkDescriptorSet descriptor_set, VkBuffer vertex_buffer, VkBuffer index_buffer,
            VkDeviceSize offset, uint32_t count) {
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, triangles_pipeline->pipeline);
    vkCmdBindVertexBuffers(command_buffer, 0, 1, &vertex_buffer, &offset);
    vkCmdBindIndexBuffer(command_buffer, index_buffer, 0, VK_INDEX_TYPE_UINT16);
    vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, triangles_pipeline->pipeline_layout, 0, 1, &descriptor_set, 0, nullptr);
    vkCmdDrawIndexed(command_buffer, count, 1, 0, 0, 0);
  }
};

class UiRenderer : public Renderer {
public:
  UiRenderer() {}
  ~UiRenderer() {}
  virtual void Render(VkCommandBuffer command_buffer, uint32_t image_index) override {}
};

class Engine {
  Window *window_{};
  std::shared_ptr<Gpu> gpu_{};
  SceneRenderer *scene_renderer_{};
  UiRenderer *ui_renderer_{};

  std::function<void(void)> user_render_func;

public:
  Engine() {
    window_ = new Window("e3d", 1280, 720);
    gpu_ = std::make_shared<Gpu>(window_);
    scene_renderer_ = new SceneRenderer(gpu_);
    ui_renderer_ = new UiRenderer();
  }

  ~Engine() {}

  void Run() {
    bool quit = false;
    while (!quit) {
      Window::Event event;
      while (window_->PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          quit = true;
        } else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == window_->GetWindowId()) {
          quit = true;
        }
      }

      gpu_->Render([this](VkCommandBuffer command_buffer, uint32_t image_index) { scene_renderer_->Render(command_buffer, image_index); });
    }
  }

  void SetUserRenderFunction(std::function<void(void)> &&func) { user_render_func = func; }
};

} // namespace e3d
