#include "phisi_vulkan.hpp"
#include "GLFW/glfw3.h"
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_enums.hpp>

namespace phisi_app {
    
  void VulkanContext::createVkInstance() {
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
    
    vk::InstanceCreateInfo create_info(
      vk::InstanceCreateFlags(), &app_info, 
      layers.size(), layers.data(), 
      extensions.size(), extensions.data());
    m_instance = vk::createInstanceUnique(create_info);
  } 

  #ifdef DEBUG
  VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallbackFunc(
    VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity, 
    VkDebugUtilsMessageTypeFlagsEXT msg_type, 
    const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, 
    void* p_user_data) {
    
    return false;
  }
   
  void VulkanContext::createDebugMessenger() {    
    vk::DebugUtilsMessengerCreateInfoEXT create_info(
      vk::DebugUtilsMessengerCreateFlagsEXT(), 
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | 
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eError, 
      vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
      vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
      vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
      (vk::PFN_DebugUtilsMessengerCallbackEXT) debugCallbackFunc);
    vk::detail::DispatchLoaderDynamic dldi(m_instance.get(), vkGetInstanceProcAddr);
    m_debug_messenger = m_instance.get().createDebugUtilsMessengerEXTUnique(create_info, nullptr, dldi);
  }
  #endif
  
  
  bool VulkanContext::init() {
    createVkInstance();
    #ifdef DEBUG 
    createDebugMessenger();
    #endif
    
    return true;
  }  
}