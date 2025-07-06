#pragma once
#include "phisi_vulkan.hpp"
#include "fluid_screen.hpp"

namespace phisi_app {
  class Application {
    VulkanContext m_vk_context;
    phisi::fluid::FluidScreen m_fluid_screen;
    
    float m_frame_time = 0.01;
    int m_screen_mode;
    float m_paint_color[3] = {1.0, 0.0, 0.0};
    float m_paint_size = 5.5;
    
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


