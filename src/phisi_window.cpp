#include "phisi_window.hpp"
#include "GLFW/glfw3.h"
#include "logger.hpp"


namespace phisi_app {

  void glfw_error_callback(int error, const char* description) {
    LOG_ERROR("Glfw error " << error << ": " << description); 
  }

  void PhisiWindow::initGlfw() {
    if(!glfwInit()) {
      LOG_ERROR("Failed to initialize GLFW.");
    }
    if(!glfwVulkanSupported()) {
      LOG_ERROR("GLFW does not support Vulkan.");
    }
    glfwSetErrorCallback(glfw_error_callback);
  }


  void PhisiWindow::setInstance(vk::Instance instance) {
    m_instance = instance;
  }

  
  void PhisiWindow::createWindow(std::string title) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
    
    GLFWmonitor* const monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* const vid_mode = glfwGetVideoMode(monitor);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_RED_BITS, vid_mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, vid_mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, vid_mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, vid_mode->refreshRate);
    m_window = glfwCreateWindow(vid_mode->width, vid_mode->height, title.c_str(), nullptr, nullptr);
    LOG_TRACE("GLFW window has been created.");
  }


  void PhisiWindow::createSurface() {
    VkSurfaceKHR surface;
    VkResult result = glfwCreateWindowSurface(m_instance, m_window, nullptr, &surface);
    if (result != 0) {
      LOG_ERROR("Could not create VkSurface: " << result << ".");
    } else {
      m_surface = vk::SurfaceKHR(surface);
    }
    LOG_TRACE("VkSurfaceKHR has been created.");
  }


  void PhisiWindow::create(std::string title) {
    createWindow(title);  
    createSurface();
  }

  std::pair<int, int> PhisiWindow::getFrameBufferSize() {
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    return std::pair{width, height};
  }

  bool PhisiWindow::shouldClose() {
    return glfwWindowShouldClose(m_window);
  }

  bool PhisiWindow::minimized() {
    return glfwGetWindowAttrib(m_window, GLFW_ICONIFIED);
  }

  void PhisiWindow::destory() {
    glfwDestroyWindow(m_window);
  }
}
