#include "phisi_app.hpp"
#include "imgui.h"
#include "logger.hpp"
#include <cmath>


extern int sim_pixel_ratio;

namespace phisi_app {

  void Application::init() {
    //init vulkan and imgui
    m_vk_context.init();
    ImGuiIO& imgui_io = ImGui::GetIO();
    imgui_io.IniFilename = "config/imgui.ini";
    imgui_io.Fonts -> AddFontFromFileTTF("assets/Cousine-Regular.ttf", 18.0);
    
    //init texture
    std::pair size = m_vk_context.m_window.getFrameBufferSize();
    m_vk_context.m_texture.initGpuOnly(size.first / sim_pixel_ratio, size.second / sim_pixel_ratio);
    //m_vk_context.m_texture.allocateStagingBuffer();
    
    //init gpu FluidScreen
    m_vk_context.m_fluid_screen.setImage(m_vk_context.m_texture);
    m_vk_context.m_fluid_screen.allocate(m_vk_context.m_texture.m_width, m_vk_context.m_texture.m_height);
    m_vk_context.m_fluid_screen.initBuffer();
    LOG_INFO("Created FluidScreen of size " << m_vk_context.m_texture.m_width << " * " << m_vk_context.m_texture.m_height << ".");
  }


  void Application::update() {
    //manage user input
    if(ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
      return;
    static double last_cursor_x, last_cursor_y = 0.0;
    double cursor_x, cursor_y;
    glfwGetCursorPos(m_vk_context.m_window.m_window, &cursor_x, &cursor_y); 
    m_vk_context.m_fluid_screen.setPencilRadius(m_pencil_radius);
    m_vk_context.m_fluid_screen.setPencilPosition(cursor_x / sim_pixel_ratio, cursor_y / sim_pixel_ratio);
    if (glfwGetMouseButton(m_vk_context.m_window.m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {  
      if (m_pencil_mode == 0) {
        m_vk_context.m_fluid_screen.setPencilColor(m_pencil_color);
      } else if(m_pencil_mode == 1) {
        float direction[2] = {0.0, 0.0};
        direction[0] = (cursor_x - last_cursor_x) / m_vk_context.m_frame_time * m_vel_strength / 100;
        direction[1] = (cursor_y - last_cursor_y) / m_vk_context.m_frame_time * m_vel_strength / 100;
        m_vk_context.m_fluid_screen.setPencilVelocity(direction);
      } else if(m_pencil_mode == 2) {
        m_vk_context.m_fluid_screen.setPencilNegativDivergence(m_vel_strength);
      } else if(m_pencil_mode == 3) {
        m_vk_context.m_fluid_screen.setPencilPositivDivergence(m_vel_strength);
      }
    } else 
      m_vk_context.m_fluid_screen.removePencil();
    last_cursor_x = cursor_x;
    last_cursor_y = cursor_y;
  }

  
  void Application::imGuiLayoutSetup() {
    //general settings
    ImGui::Begin("General");
    if (ImGui::Button("Pause/Play")) 
      m_vk_context.m_fluid_screen.m_run_simulation ^= true;
    ImGui::Text("FPS: %f", 1.0 / m_vk_context.m_frame_time);
    ImGui::SliderInt("Frame Limit", &m_vk_context.m_frame_limit, 1.0, 480.0);
    ImGui::SliderFloat("Gravity", &m_vk_context.m_fluid_screen.m_gravity, 0.0, 100.0);
    int value = m_vk_context.m_fluid_screen.m_div_iters;
    ImGui::SliderInt("Divergence Iterations", &value, 10, 300);
    m_vk_context.m_fluid_screen.m_div_iters = value;
    ImGui::SliderFloat("Overrelaxation", &m_vk_context.m_fluid_screen.m_overrelaxation, 1.0, 2.0);
    value = m_vk_context.m_fluid_screen.m_rk_steps;
    ImGui::SliderInt("RK Steps", &value, 1, 30);
    m_vk_context.m_fluid_screen.m_rk_steps = value;
    ImGui::End();

    
    //cursor action
    ImGui::Begin("Cursor");
    
    ImGui::ColorEdit3("Pencil color", m_pencil_color);
    ImGui::SliderFloat("Pencil size", &m_pencil_radius, 1.0, std::round(m_vk_context.m_texture.m_width / 6.0));
    ImGui::SliderFloat("Velocity Strength", &m_vel_strength, 0.0, 100.0);
    if(ImGui::RadioButton("Color Mode", m_pencil_mode == 0))
      m_pencil_mode = 0;
    if(ImGui::RadioButton("Velocity Mode", m_pencil_mode  == 1))
      m_pencil_mode = 1;
    
    if(ImGui::RadioButton("Negativ Divergence", m_pencil_mode  == 2))
      m_pencil_mode = 2;
    if(ImGui::RadioButton("Positiv Divergence", m_pencil_mode  == 3))
      m_pencil_mode = 3;
    
    
    
    ImGui::End();
    
    
    //draw simulation
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImDrawList* draw_list = ImGui::GetBackgroundDrawList(viewport);
    ImVec2 pos = viewport->Pos;
    ImVec2 size = viewport->Size;

    draw_list->AddImage(
      (ImTextureID) m_vk_context.m_texture.getDescriptor(), 
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
    m_vk_context.destroy();
  }
}
