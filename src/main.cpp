#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <cstring>  // 包含用于strcmp的头文件
#include <iostream>
#include <stdexcept>
#include <vector>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
const std ::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void DestoryDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks* pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}
class HelloTriangleApplication {
 public:
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

 private:
  GLFWwindow* window;
  VkInstance instance;  // 用来保存实例的句柄
  uint32_t extensionCount = 0;
  VkDebugUtilsMessengerEXT debugMessenger;

  void initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan1", nullptr, nullptr);
  }

  void initVulkan() {
    createInstance();
    setupDebugMessenger();
  }

  void popularDebugMessengerCreateinfo(
      VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
  }

  void setupDebugMessenger() {
    if (!enableValidationLayers) {
      return;
    };
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    popularDebugMessengerCreateinfo(createInfo);
    if(CreateDebugUtilsMessengerEXT(instance,&createInfo,nullptr,&debugMessenger) != VK_SUCCESS){
      throw std::runtime_error("failed to set up debug messenger!");
    }
  }

  void mainLoop() {
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
    }
  }

  void cleanup() {
    if (enableValidationLayers) {
      DestoryDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();
  }

  void createInstance() {
    if (enableValidationLayers && !checkValidationLayerSupport()) {
      throw std::runtime_error(
          "validation layers requested, but not available!");
    }

    // VkApplicationInfo结构体
    VkApplicationInfo appinfo{};
    appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appinfo.pApplicationName = "Hello Triangle";
    appinfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appinfo.pEngineName = "No Engine";
    appinfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appinfo.apiVersion = VK_API_VERSION_1_0;

    // VkInstanceCreateInfo结构体
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appinfo;
    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount = 0;

    // VkDebugUtilsMessengerCreateInfoEXT 结构体
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if(enableValidationLayers){
      createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
      createInfo.ppEnabledLayerNames = validationLayers.data();
      popularDebugMessengerCreateinfo(debugCreateInfo);
      createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    }else{
      createInfo.enabledLayerCount = 0;
      createInfo.pNext = nullptr;
    }

    if(vkCreateInstance(&createInfo,nullptr,&instance)!=VK_SUCCESS){
      throw std::runtime_error("failed to create instance!");
    }


    // VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{};
    // messengerCreateInfo.sType =
    //     VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    // messengerCreateInfo.messageSeverity =
    //     VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
    //     VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
    //     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    // messengerCreateInfo.messageType =
    //     VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
    //     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
    //     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    // messengerCreateInfo.pfnUserCallback = debugCallback;
    // messengerCreateInfo.pUserData = nullptr;

    // 第二个空指针为默认内存分配
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
      throw std ::runtime_error("failed to create instance!");
    };

    // // 检查拓展数量及其信息
    // vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
    // nullptr); std::vector<VkExtensionProperties> extensions(extensionCount);
    // vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
    //                                        extensions.data());
    // std::cout << "available extensions:" << std::endl;
    // std::cout << "extensionCount:" << extensionCount << std::endl;

    for (const auto& extensionPtr : extensions) {
      std::cout << extensionPtr << std::endl;  // 输出字符串
    }

    if (enableValidationLayers) {
      createInfo.enabledLayerCount =
          static_cast<uint32_t>(validationLayers.size());
      createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
      createInfo.enabledLayerCount = 0;
    }
    std::cout << "createInfo.enabledLayerCount:" << createInfo.enabledLayerCount
              << std::endl;
  }

  bool checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availablelayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availablelayers.data());
    for (const char* layerName : validationLayers) {
      bool layerFound = false;

      for (const auto& availablelayer : availablelayers) {
        if (strcmp(layerName, availablelayer.layerName) == 0) {
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

  std::vector<const char*> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> extensions(glfwExtensions,
                                        glfwExtensions + glfwExtensionCount);
    if (enableValidationLayers) {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    return extensions;
  }

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageServerity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void* pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
  }
};

int main() {
  HelloTriangleApplication app;

  try {
    app.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
