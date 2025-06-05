#include "phisi_app.hpp"
#include "imgui.h"
#include "logger.hpp"
#include <chrono>

namespace phisi_app {

  void Application::init() {
    //init vulkan and imgui
    m_vk_context.init();
    ImGui::GetIO().Fonts -> AddFontFromFileTTF("Cousine-Regular.ttf", 18.0);  
    
    //init texture
    std::pair size = m_vk_context.m_window.getFrameBufferSize();
    m_vk_context.m_texture.init(size.first / 8, size.second / 8);
    
    //allocate FluidScreen
    m_fluid_screen.allocate(m_vk_context.m_texture.m_width, m_vk_context.m_texture.m_height);
    LOG_INFO("Created FluidScreen of size " << m_vk_context.m_texture.m_width << " * " << m_vk_context.m_texture.m_height);
  }



  void Application::update() {
    //manage drawing
    if (glfwGetMouseButton(m_vk_context.m_window.m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
      double cursor_x, cursor_y;
      glfwGetCursorPos(m_vk_context.m_window.m_window, &cursor_x, &cursor_y);
      uint32_t pixel_x = cursor_x / 8.0;
      uint32_t pixel_y = cursor_y / 8.0;
      uint8_t color[4] = {
        static_cast<uint8_t>(m_paint_color.x * 255), 
        static_cast<uint8_t>(m_paint_color.y * 255),
        static_cast<uint8_t>(m_paint_color.z * 255),
        static_cast<uint8_t>(m_paint_color.w * 255)};
      m_fluid_screen.add_color(m_paint_size, color, pixel_x, pixel_y);
    }
    //update simultaion
    m_fluid_screen.simulate(m_frame_time);
    //load color field or pressure field
    if (m_show_pressure) {
      m_fluid_screen.calculate_pressure_color(m_vk_context.m_texture.get_memory());  
    } else {
      m_fluid_screen.get_color_field(m_vk_context.m_texture.get_memory());    
    }
  }

  

  void Application::imGuiLayoutSetup() {
    //calculate fps
    static auto now = std::chrono::high_resolution_clock::now();
    m_frame_time = std::chrono::duration_cast<std::chrono::microseconds>
      (std::chrono::high_resolution_clock::now() - now).count() / 1000000.0;
    now = std::chrono::high_resolution_clock::now();
    
    //create info window
    ImGui::Begin("General info");
    ImGui::Text("FPS: %f", 1.0 / m_frame_time);
    ImGui::Checkbox("Show pressure", &m_show_pressure);
    ImGui::End();

    //create cursor action window
    ImGui::Begin("Cursor action");
    ImGui::ColorEdit4("Paint color", (float*) &m_paint_color);
    ImGui::SliderFloat("Paint size", &m_paint_size, 1.0, 20.0);
    ImGui::Checkbox("Color mode", &color_mode);
    ImGui::SameLine();
    ImGui::Checkbox("Velocity mode: ", &velocity_mode);
    ImGui::End();
    
    //draw texture
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImDrawList* draw_list = ImGui::GetBackgroundDrawList(viewport);
    ImVec2 pos = viewport->Pos;
    ImVec2 size = viewport->Size;

    draw_list->AddImage(
      (ImTextureID) m_vk_context.m_texture.get_descriptor(), 
      pos,
      ImVec2(pos.x + size.x, pos.y + size.y));    
  }

  void Application::run() {
    while(!m_vk_context.m_window.shouldClose()) {          
      //window events
      glfwPollEvents();
      
      //update app
      update();
            
      //render frame
      if(m_vk_context.newFrame()) {
        imGuiLayoutSetup();
        m_vk_context.render();
      } 
    }  
  }

  Application::~Application() {
  }
}