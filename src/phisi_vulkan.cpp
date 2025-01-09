#include "phisi_vulkan.hpp"
#include "GLFW/glfw3.h"
#include <vector>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace phisiApp {
  
  vk::UniqueInstance createVkInstance() {
    vk::ApplicationInfo app_info(
      "Phisi App", VK_MAKE_VERSION(1, 0, 0), 
      "None", VK_MAKE_VERSION(1, 0, 0), 
      VK_API_VERSION_1_3);       
    
    std::vector<const char*> layers;
    std::vector<const char*> extensions;
    
    uint32_t ext_count = 0;
    const char** exts = glfwGetRequiredInstanceExtensions(&ext_count);
    extensions = std::vector(exts, exts + ext_count);
    
    #ifdef DEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);   
    layers.push_back("VK_LAYER_KHRONOS_validation");
    #endif
    
    vk::InstanceCreateInfo create_info({}, &app_info, layers.size(), layers.data(), extensions.size(), extensions.data());
    return vk::createInstanceUnique(create_info);
  } 

  VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity, 
    VkDebugUtilsMessageTypeFlagBitsEXT msg_type, 
    const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, 
    void* p_user_data) {
    
    return VK_FALSE;
  }
  
    
  vk::DebugUtilsMessengerEXT createDebugMessenger() {
    vk::DebugUtilsMessengerCreateInfoEXT create_info(
      vk::DebugUtilsMessengerCreateFlagsEXT(), 
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | 
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eError, 
      vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
      vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
      vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
      debugCallback, nullptr);
  }
  
}