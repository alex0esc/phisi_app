#pragma once
#include "phisi_vulkan.hpp"
#include "fluid_screen.hpp"

namespace phisi_app {
  class Application {
    VulkanContext m_vk_context;
    
    uint32_t m_pencil_mode = 0;
    float m_pencil_radius = 1.0;
    float m_pencil_color[3] = {1.0, 0.0, 0.0};
    
    void update();
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


