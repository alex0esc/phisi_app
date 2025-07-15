#pragma once
#include "imgui_impl_vulkan.h"
#include "phisi_window.hpp"
#include <vulkan/vulkan.hpp>
#include "gpu_fluid_screen.hpp"

namespace phisi_app {
   
  class VulkanContext {
    static constexpr int c_min_image_count = 2;
    static constexpr const char* c_window_title = "Phisi";
    static constexpr vk::ClearValue c_background_color = { vk::ClearColorValue(1.0f, 1.0f, 1.0f, 1.0f) };
    static constexpr uint32_t c_max_descriptors = 10;
    static constexpr uint32_t c_max_descriptor_sets = 5;
    
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
    int m_frame_limit = 100;
    float m_frame_time = 0.01;
    PhisiWindow m_window;
    TextureData m_texture;
    phisi::fluid::GpuFluidScreen m_fluid_screen;
    
    VulkanContext() = default;
    VulkanContext(const VulkanContext& other) = delete;  
    VulkanContext& operator=(const VulkanContext& other) = delete;
    
    void init(); 
    bool newFrame();
    void render();
    void destroy();
  };
    
}
