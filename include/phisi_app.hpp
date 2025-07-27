#pragma once
#include "phisi_texture.hpp"
#include "gpu_fluid_screen.hpp"

namespace phisi_app {
  class Application {
    VulkanContext m_vk_context;
    TextureData m_texture;
    GpuFluidScreen m_fluid_screen;
    
    uint32_t m_pencil_mode = 0;
    float m_vel_strength = 50;
    float m_pencil_radius = 30.0;
    float m_pencil_color[3] = {0.0, 0.0, 1.0};
    
    void handleMouseInput();
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


