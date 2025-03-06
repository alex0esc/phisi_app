#include "phisi_app.hpp"
#include "imgui.h"

namespace phisi_app {

  void Application::init() {
    m_vk_context.init();      
    ImGui::GetIO().Fonts -> AddFontFromFileTTF("Cousine-Regular.ttf", 18.0);
    
  }

  void Application::imGuiLayoutSetup() {
    ImGui::Begin("App Info");
    ImGui::Text("This is some test text");
    ImGui::End();
    
    // Get the main viewport
    ImGuiViewport* viewport = ImGui::GetMainViewport();
     
    viewport->DpiScale = 0.1;
    ImDrawList* drawList = ImGui::GetForegroundDrawList(viewport); // Get draw list for viewport
    
    // Define rectangle positions
    
    ImVec2 topLeft = ImVec2(viewport->Pos.x + 100, viewport->Pos.y + 100);
    ImVec2 bottomRight = ImVec2(viewport->Pos.x + 2000, viewport->Pos.y + 200);
    
    // Draw a filled rectangle
    drawList->AddRectFilled(topLeft, bottomRight, IM_COL32(255, 0, 0, 200));
  }

  void Application::run() {
    while(!m_vk_context.m_window.shouldClose()) {          
      //window events
      glfwPollEvents();
      //update app
      
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