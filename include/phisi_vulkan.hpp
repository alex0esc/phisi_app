#include "imgui_impl_vulkan.h"
#include "phisi_window.hpp"
#include <vulkan/vulkan.hpp>

namespace phisi_app {
  
  class VulkanContext {
    static constexpr int c_min_image_count = 2;
    static constexpr const char* c_window_title = "Phisi";
    
    vk::UniqueInstance m_instance;
    vk::detail::DispatchLoaderDynamic m_dldi;
    Window m_window;
    vk::PhysicalDevice m_device_physical;
    vk::UniqueDevice m_device;
    vk::Queue m_queue;
    uint32_t m_queue_family_index;
    vk::UniqueDescriptorPool m_descriptor_pool;
    ImGui_ImplVulkanH_Window m_window_data;
    bool m_swapchain_rebuild;
    
    VulkanContext(const VulkanContext& other) = delete;  
    VulkanContext(VulkanContext&& other) = delete;

    void createVkInstance();  
    void createWindow();
    void chosePhysicalDevice();
    void createLogicalDevice();
    void createQueue();
    void createDescriptorPool();
    void setupVulkanWindow();
    void setupImGUI();
    void renderFrame(ImDrawData*);
    void presentFrame();
    
    #ifdef BUILD_DEBUG
    vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::detail::DispatchLoaderDynamic> m_debug_messenger; 
    void createDebugMessenger();
    #endif    
  public:
    VulkanContext() {};
    ~VulkanContext();
    void init();
    void render();
  };
    
}