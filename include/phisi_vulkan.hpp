#include <vulkan/vulkan.hpp>

namespace phisi_app {
  
  class VulkanContext {
    vk::UniqueInstance m_instance;
    
    VulkanContext(const VulkanContext& other) = delete;  
    VulkanContext(VulkanContext&& other) = delete;

    void createVkInstance();  
    
    #ifdef DEBUG
    vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::detail::DispatchLoaderDynamic> m_debug_messenger; 
    void createDebugMessenger();
    #endif    
  public:
    VulkanContext() {};
    bool init();
  };
    
}