#pragma once
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <vulkan/vulkan.hpp>
#include <string>

namespace phisi_app {

  class PhisiWindow {
  public:
    GLFWwindow* m_window;
    vk::SurfaceKHR m_surface;
    
    PhisiWindow() = default;
    
    void createWindow(std::string title);
    void createSurface(vk::Instance instance);
    
    static void initGlfw();
    std::pair<int, int> getFrameBufferSize();
    bool shouldClose();
    bool minimized();
    void destory();
  };
  
}
