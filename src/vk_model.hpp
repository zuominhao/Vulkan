/**
 * 当定义这个宏并包含 GLFW/glfw3.h 头文件时，GLFW 会自动包含 Vulkan
 * 相关的头文件（如 vulkan/vulkan.h）。这样，不需要手动包含 Vulkan 的头文件。
 */
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h> //使用 GLFW 库的功能，如创建窗口、处理输入事件等。

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>

// 设置长和高
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

// 设置验证层数组
const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

/**创建一个调试工具消息
     VkInstance instance,                             // Vulkan 实例
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, // 调试消息创建信息
    const VkAllocationCallbacks* pAllocator,  // 自定义分配器，可为nullptr
  VkDebugUtilsMessengerEXT* pDebugMessenger     // 返回的调试消息句柄
*/
VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {
  // 获取函数地址，转换为 PFN_vkCreateDebugUtilsMessengerEXT 类型
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");
  // 如果函数地址不为空，调用函数创建调试消息
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    // 如果函数地址为空，返回错误码，表示扩展不支持
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

/**
 *这个函数的作用是根据给定的 Vulkan 实例和调试消息回调句柄，使用 Vulkan API
 *提供的扩展函数 vkDestroyDebugUtilsMessengerEXT 来销毁特定的调试消息回调。
 */
void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks *pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily; // 存储用于图形操作的队列族的索引值
  // 成员函数，用来检查 QueueFamilyIndices 结构体的实例是否完整。
  bool isComplete() { return graphicsFamily.has_value(); }
};

class HelloTriangleApplication {
public:
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

private:
  GLFWwindow *window;
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;

  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice devices;

  /** 设置窗口大小，名称
   * 第3个 nullptr 表示没有指定显示器，窗口将显示在主显示器上。第4个 nullptr 表示不需要与其他窗口共享资源。
   */
  void initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // GLFW_NO_API 表示不创建 OpenGL 或 OpenGL ES 上下文
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
  }

  void initVulkan() {
    createInstance();      // 创建一个 Vulkan 实例
    setupDebugMessenger(); // 设置调试信使。调试信使用于接收和处理 Vulkan的调试消息，如验证层发出的错误、警告和信息消息。
    pickPhysicalDevice();  // 选择一个物理设备（即 GPU）。Vulkan允许你选择系统中的一个或多个物理设备来执行渲染任务。
    createLogicalDevice(); // 创建一个逻辑设备。逻辑设备是应用程序与物理设备交互的接口，它封装了对物理设备的访问。
  }

  void mainLoop() {
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
    }
  }

  void cleanup() {
    // 如果启用了验证层，销毁调试信使
    if (enableValidationLayers) {
      DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
    vkDestroyInstance(instance, nullptr); // 销毁 Vulkan 实例，nullptr 作为分配回调参数，表示不使用自定义分配器，使用默认的vulkan自带的内存分配。
    glfwDestroyWindow(window);            // 销毁窗口
    glfwTerminate();                      // 终止 GLFW
  }

  void createLogicalDevice() {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice); // 查找物理设备的队列族索引
    // 配置队列创建信息
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value(); // 队列族索引
    queueCreateInfo.queueCount = 1;                                    // 队列数量
    float queuePriority = 1.0F;                                        // 队列优先级
    queueCreateInfo.pQueuePriorities = &queuePriority;                 // 指向优先级的指针

    VkPhysicalDeviceFeatures deviceFeatures{}; // 配置物理设备的特性

    // 配置逻辑设备创建信息
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo; // 指向队列创建信息的指针
    createInfo.queueCreateInfoCount = 1;             // 队列创建信息数量
    createInfo.pEnabledFeatures = &deviceFeatures;   // 指向物理设备特性的指针
  }

  void createInstance() {
    // 检查是否启用验证层，并检查验证层是否支持
    if (enableValidationLayers && !checkValidationLayerSupport()) {
      throw std::runtime_error(
          "validation layers requested, but not available! 请求了验证层但未找到支持的验证层");
    }
    // 配置应用程序信息 如名称、版本、引擎等。
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // 配置实例创建信息
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // 获取所需的扩展列表
    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    // 如果启用了验证层，配置验证层信息
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
      createInfo.enabledLayerCount =
          static_cast<uint32_t>(validationLayers.size());
      createInfo.ppEnabledLayerNames = validationLayers.data();

      populateDebugMessengerCreateInfo(debugCreateInfo);
      createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
    } else {
      createInfo.enabledLayerCount = 0;

      createInfo.pNext = nullptr;
    }

    // 创建 Vulkan 实例
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
      throw std::runtime_error("failed to create instance!");
    }
  }
  /**
   * 填充 VkDebugUtilsMessengerCreateInfoEXT 结构体对象 createInfo 的各个字段，以配置调试信使的创建信息。
   */
  void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    // messageSeverity 字段设置了调试消息的严重性级别
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    // messageType 字段设置了调试消息的类型
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    // pfnUserCallback 字段是一个函数指针，指向一个用户定义的回调函数 debugCallback。当 Vulkan 发送调试消息时，会调用该回调函数，允许应用程序进行自定义的处理和响应。
    createInfo.pfnUserCallback = debugCallback;
  }

  /**
   * 作用是设置 Vulkan 的调试信使，用于接收和处理调试消息，如验证层发出的错误、警告和信息消息
   */
  void setupDebugMessenger() {
    if (!enableValidationLayers)
      return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
      throw std::runtime_error("failed to set up debug messenger!");
    }
  }

  /**
   * 实现了选择 Vulkan 物理设备（通常是 GPU）的逻辑。
   */
  void pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr); // 用于获取当前 Vulkan 实例支持的物理设备数量。instance 是 Vulkan 实例的句柄，nullptr 表示暂时不需要获取具体的设备列表。

    if (deviceCount == 0) {
      throw std::runtime_error("failed to find GPUs with Vulkan support! 没有找到支持 Vulkan 的物理设备");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);                 // 创建一个大小为 deviceCount 的 std::vector，用于存储 Vulkan 物理设备的句柄。
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()); // 再次调用 vkEnumeratePhysicalDevices 函数来填充 devices 向量，获取具体的物理设备句柄。

    for (const auto &device : devices) {
      VkPhysicalDeviceProperties deviceProperties;
      vkGetPhysicalDeviceProperties(device, &deviceProperties);
      VkPhysicalDeviceType deviceType = deviceProperties.deviceType;
      const char *deviceTypeName = "";
      switch (deviceType) {
      case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        deviceTypeName = "Other";
        break;
      case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        deviceTypeName = "Integrated GPU";
        break;
      case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        deviceTypeName = "Discrete GPU";
        break;
      case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        deviceTypeName = "Virtual GPU";
        break;
      case VK_PHYSICAL_DEVICE_TYPE_CPU:
        deviceTypeName = "CPU";
        break;
      }

      std::cout << "Device Name: " << deviceProperties.deviceName << std::endl;
      std::cout << "Device Type: " << deviceTypeName << std::endl;

      // 检查设备是否符合要求，并在此添加其他属性的打印
      if (isDeviceSuitable(device)) {
        physicalDevice = device;
        std::cout << "Selected device!" << std::endl;
        break;
      }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
      throw std::runtime_error("failed to find a suitable GPU! 没有合适的GPU设备");
    }
  }

  // 判断给定的 Vulkan 物理设备是否适合应用程序的要求
  bool isDeviceSuitable(VkPhysicalDevice device) {
    QueueFamilyIndices indices = findQueueFamilies(device);
    return indices.isComplete();
  }

  /**
   *用于获取指定 Vulkan 物理设备支持的队列族信息，并将关键信息填充到 QueueFamilyIndices 结构体中
   */
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr); // 用于查询指定物理设备 device 支持的队列族属性。
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data()); // 再次调用获取具体的队列族属性到 queueFamilies 数组中。

    int i = 0;
    for (const auto &queueFamily : queueFamilies) {
      if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        indices.graphicsFamily = i;
      }

      if (indices.isComplete()) {
        break;
      }

      i++;
    }

    return indices;
  }

  /**
   * 用于获取需要加载的 Vulkan 实例扩展列表。
   */
  std::vector<const char *> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount); // 获取所需的 Vulkan 实例扩展名称，同时获取扩展数量 glfwExtensionCount。

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount); // 创建一个 std::vector 类型的 extensions，并初始化为从 glfwExtensions 数组中拷贝的内容。

    if (enableValidationLayers) {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
  }

  bool checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : validationLayers) {
      bool layerFound = false;

      for (const auto &layerProperties : availableLayers) {
        if (strcmp(layerName, layerProperties.layerName) == 0) {
          layerFound = true;
          break;
        }
      }

      if (!layerFound) {
        return false;
      }
    }

    return true;
  }

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
  }
};