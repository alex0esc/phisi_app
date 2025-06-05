#pragma once
#include "phisi_vulkan.hpp"
#include "phisi.hpp"

namespace phisi_app {
  class Application {
    VulkanContext m_vk_context;
    phisi::FluidScreen m_fluid_screen;
    
    float m_frame_time = 0.0;
    bool m_show_pressure = false;
    ImVec4 m_paint_color = {1.0, 0.0, 0.0, 1.0};
    float m_paint_size = 5.5;
    bool color_mode;
    bool velocity_mode;
    
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


