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
    //manage user input
    static bool button_press = false;
    int button_state = glfwGetMouseButton(m_vk_context.m_window.m_window, GLFW_MOUSE_BUTTON_LEFT);
    static double last_cursor_x, last_cursor_y = 0.0;
    double cursor_x, cursor_y;
    glfwGetCursorPos(m_vk_context.m_window.m_window, &cursor_x, &cursor_y); 
    if (button_state == GLFW_PRESS && !button_press) {
      button_press = true;  
      if (m_screen_mode == 0) {  
        m_fluid_screen.add_color(m_paint_size, phisi::Vector2D(cursor_x / 8.0, cursor_y / 8.0), m_paint_color);
      } else if(m_screen_mode == 1) {
        phisi::Vector2D direction = {0.0, 0.0};
        direction.m_x = cursor_x - last_cursor_x;
        direction.m_y = cursor_y - last_cursor_y;
        direction /= m_frame_time * 20;
        m_fluid_screen.add_velocity(m_paint_size, direction, phisi::Vector2D(cursor_x / 8.0, cursor_y / 8.0));
      }
    } else if (button_state == GLFW_RELEASE && button_press) {
      button_press = false;
    }
    last_cursor_x = cursor_x;
    last_cursor_y = cursor_y;
    
    //update simultaion
    m_fluid_screen.simulate(m_frame_time);
    
    //load color field or pressure field
    if (m_screen_mode == 2) {
      m_fluid_screen.get_pressure_color(m_vk_context.m_texture.get_memory());  
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
    ImGui::End();

    //create cursor action window
    ImGui::Begin("Cursor action");
    ImGui::ColorEdit3("Paint color", (float*) &m_paint_color);
    ImGui::SliderFloat("Paint size", &m_paint_size, 1.0, 20.0);
    if(ImGui::RadioButton("Color Mode", m_screen_mode == 0)) m_screen_mode = 0;
    if(ImGui::RadioButton("Velocity Mode", m_screen_mode == 1)) m_screen_mode = 1;
    if(ImGui::RadioButton("Pressure Field", m_screen_mode == 2)) m_screen_mode = 2;
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