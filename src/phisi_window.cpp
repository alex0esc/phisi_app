#include "phisi_window.hpp"
#include "GLFW/glfw3.h"
#include "logger.hpp"


namespace phisi_app {

  void glfw_error_callback(int error, const char* description) {
    LOG_ERROR("Glfw error " << error << ": " << description); 
  }

  void Window::initGlfw() {
    if(!glfwInit()) {
      LOG_ERROR("Failed to initialize GLFW.");
    }
    if(!glfwVulkanSupported()) {
      LOG_ERROR("GLFW does not support Vulkan.");
    }
    glfwSetErrorCallback(glfw_error_callback);
  }


  void Window::setInstance(vk::Instance instance) {
    m_instance = instance;
  }

  
  void Window::createWindow(std::string title) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
    
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);
    m_window = glfwCreateWindow(vidmode->width / 2, vidmode->height / 2, title.c_str(), nullptr, nullptr);
    LOG_TRACE("GLFW window has been created.");
  }


  void Window::createSurface() {
    VkSurfaceKHR surface;
    VkResult result = glfwCreateWindowSurface(m_instance, m_window, nullptr, &surface);
    if (result != 0) {
      LOG_ERROR("Could not create VkSurface: " << result << ".");
    } else {
      m_surface = vk::SurfaceKHR(surface);
    }
    LOG_TRACE("VkSurfaceKHR has been created.");
  }


  void Window::create(std::string title) {
    createWindow(title);  
    createSurface();
  }

  std::pair<int, int> Window::getFrameBufferSize() {
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    return std::pair{width, height};
  }

  bool Window::shouldClose() {
    return glfwWindowShouldClose(m_window);
  }

  bool Window::minimized() {
    return glfwGetWindowAttrib(m_window, GLFW_ICONIFIED);
  }

  Window::~Window() {
    glfwDestroyWindow(m_window);
  }
}