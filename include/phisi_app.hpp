#pragma once
#include "phisi_vulkan.hpp"

namespace phisi_app {
  class Application {
    VulkanContext m_vk_context;

    void imGuiLayoutSetup();
    
  public:  
    Application(const VulkanContext& other) = delete;  
    Application operator=(const VulkanContext& other) = delete;
    Application() {};
    
    void init(); 
    void run();
    
    
    ~Application();
  };
}


