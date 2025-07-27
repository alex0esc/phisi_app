#include "phisi_app.hpp"
#include "imgui.h"
#include "logger.hpp"
#include <cmath>

extern int sim_pixel_ratio;

namespace phisi_app {

  void Application::init() {
    //init vulkan and imgui
    m_vk_context.init();
    
    //init texture
    m_texture.initVk(&m_vk_context);
    std::pair size = m_vk_context.m_window.getFrameBufferSize();
    m_texture.allocate(size.first / sim_pixel_ratio, size.second / sim_pixel_ratio);
    
    //init gpu FluidScreen
    m_fluid_screen.initVk(&m_vk_context, &m_texture);
    m_fluid_screen.allocate();
    m_fluid_screen.initBuffer();
    LOG_INFO("Created FluidScreen of size " << m_texture.m_width << " * " << m_texture.m_height << ".");
  }


  void Application::handleMouseInput() {
    if(ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
      return;
    static ImVec2 last_cursor_pos = {0, 0}; 
    ImVec2 cursor_pos = ImGui::GetMousePosOnOpeningCurrentPopup();
    m_fluid_screen.setPencilRadius(m_pencil_radius);
    m_fluid_screen.setPencilPosition(cursor_pos.x / sim_pixel_ratio, cursor_pos.y / sim_pixel_ratio);
    
    if (glfwGetMouseButton(m_vk_context.m_window.m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {  
      if (m_pencil_mode == 0) {
        m_fluid_screen.setPencilColor(m_pencil_color);
      } else if(m_pencil_mode == 1) {
        float direction[2] = {0.0, 0.0};
        direction[0] = (cursor_pos.x - last_cursor_pos.x) / m_vk_context.m_frame_time * m_vel_strength / 100;
        direction[1] = (cursor_pos.y - last_cursor_pos.y) / m_vk_context.m_frame_time * m_vel_strength / 100;
        m_fluid_screen.setPencilVelocity(direction);
      } else if(m_pencil_mode == 2) {
        m_fluid_screen.setPencilDivergence(m_vel_strength);
      } 
    } else 
      m_fluid_screen.removePencil();
      
    last_cursor_pos = cursor_pos;
  }

  void Application::update() {
    handleMouseInput();
    m_fluid_screen.simulate();
  }
  
  void Application::imGuiLayoutSetup() {
    //general settings
    ImGui::Begin("General");
    if (ImGui::Button("Pause/Play")) 
      m_fluid_screen.m_run_simulation ^= true;
    ImGui::Text("FPS: %f", 1.0 / m_vk_context.m_frame_time);
    ImGui::SliderInt("Frame Limit", &m_vk_context.m_frame_limit, 1.0, 480.0);
    ImGui::SliderFloat("Gravity", &m_fluid_screen.m_push_constant.gravity, -100.0, 100.0);
    int value = m_fluid_screen.m_div_iters;
    ImGui::SliderInt("Divergence Iterations", &value, 10, 300);
    m_fluid_screen.m_div_iters = value;
    ImGui::SliderFloat("Overrelaxation", &m_fluid_screen.m_push_constant.overrelaxation, 1.0, 2.0);
    value = m_fluid_screen.m_push_constant.rk_steps;
    ImGui::SliderInt("RK Steps", &value, 1, 30);
    m_fluid_screen.m_push_constant.rk_steps = value;
    static float f_value = m_fluid_screen.m_push_constant.saturation;
    ImGui::SliderFloat("Saturation", &f_value, 0.0, 10.0);
    m_fluid_screen.m_push_constant.saturation = std::pow(f_value, 2);
    ImGui::End();

    //cursor action
    ImGui::Begin("Cursor");
    ImGui::ColorEdit3("Pencil color", m_pencil_color);
    ImGui::SliderFloat("Pencil size", &m_pencil_radius, 1.0, 200);
    ImGui::SliderFloat("Velocity Strength", &m_vel_strength, -100.0, 100.0);
    if(ImGui::RadioButton("Color Mode", m_pencil_mode == 0))
      m_pencil_mode = 0;
    if(ImGui::RadioButton("Velocity Mode", m_pencil_mode  == 1))
      m_pencil_mode = 1;
    if(ImGui::RadioButton("Divergence", m_pencil_mode  == 2))
      m_pencil_mode = 2;
    ImGui::End();
    
    //draw simulation
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImDrawList* draw_list = ImGui::GetBackgroundDrawList(viewport);
    ImVec2 pos = viewport->Pos;
    ImVec2 size = viewport->Size;

    draw_list->AddImage(
      (ImTextureID) m_texture.getDescriptor(), 
      pos,
      ImVec2(pos.x + size.x, pos.y + size.y));    
  }
  

  void Application::run() {    
    while(!m_vk_context.m_window.shouldClose()) {          
      //window events
      glfwPollEvents();
            
      //new frame
      if(!m_vk_context.newFrame())
        continue;

      //GUI
      imGuiLayoutSetup();

      //update Game state
      update();

      //render frame
      m_vk_context.render();
       
    }  
  }

  Application::~Application() {
    m_fluid_screen.destroy();
    m_texture.destroy();
    m_vk_context.destroy();
  }
}
