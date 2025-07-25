#pragma once
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <vulkan/vulkan.hpp>
#include <string>

namespace phisi_app {

  class PhisiWindow {
    vk::Instance m_instance;
    
    void createWindow(std::string title);
    void createSurface();
    
  public:
    GLFWwindow* m_window;
    vk::SurfaceKHR m_surface;
    
    PhisiWindow(const PhisiWindow&) = delete;
    PhisiWindow& operator=(const PhisiWindow&) = delete;
    PhisiWindow() {}
    static void initGlfw();
    void setInstance(vk::Instance instance);
    void create(std::string title);
    std::pair<int, int> getFrameBufferSize();
    bool shouldClose();
    bool minimized();
    void destory();
  };
  
}
