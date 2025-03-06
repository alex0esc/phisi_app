#pragma once
#include "imgui_impl_vulkan.h"
#include "phisi_window.hpp"
#include <vulkan/vulkan.hpp>

namespace phisi_app {
  
  class VulkanContext {
    static constexpr int c_min_image_count = 2;
    static constexpr const char* c_window_title = "Phisi";
    static constexpr vk::ClearValue c_background_color = { vk::ClearColorValue(0.45f, 0.55f, 0.60f, 1.00f) };
    
    vk::UniqueInstance m_instance;
    vk::detail::DispatchLoaderDynamic m_dldi;
    vk::PhysicalDevice m_device_physical;
    vk::UniqueDevice m_device;
    vk::Queue m_queue;
    uint32_t m_queue_family_index;
    vk::UniqueDescriptorPool m_descriptor_pool;
    ImGui_ImplVulkanH_Window m_window_data;
    bool m_swapchain_rebuild;
    
    void createVkInstance();  
    void createWindow();
    void chosePhysicalDevice();
    void createLogicalDevice();
    void createQueue();
    void createDescriptorPool();
    void setupVulkanWindow();
    void setupImGUI();
    void renderFrame(ImDrawData* draw_data);
    void presentFrame();
    
    #ifdef BUILD_DEBUG
    vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::detail::DispatchLoaderDynamic> m_debug_messenger; 
    void createDebugMessenger();
    #endif    
    
  public:
    Window m_window;
    VulkanContext(const VulkanContext& other) = delete;  
    VulkanContext operator=(const VulkanContext& other) = delete;
    VulkanContext() {};
    ~VulkanContext();
    void init();
    bool newFrame();
    void render();
  };
    
}