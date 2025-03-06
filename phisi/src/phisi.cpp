#include "phisi.hpp"

namespace phisi {

  void FluidPixel::calculateTouching(uint32_t screen_width, uint32_t screen_height, uint32_t pixel_index) {
    //left
    if (pixel_index % screen_width != 0)
      m_touch_left = pixel_index - 1;     
    //bottom
    if (pixel_index >= screen_width)
      m_touch_bottom = pixel_index - screen_width;
    //right
    if ((pixel_index - 1) % screen_width != 0)
      m_touch_right = pixel_index + 1;
    //top
    if (pixel_index < screen_height * screen_width)
      m_touch_top = pixel_index + screen_width;
  }      

  
  FluidScreen::FluidScreen(uint32_t width, uint32_t height) {
    m_width = width;        
    m_height = height;
  }

  void FluidScreen::init() {
    if (m_pixeles.size() > 0)
      return;
    m_pixeles.resize(m_width * m_height);
    for(int i = 0; i < m_pixeles.size(); i++) {
      m_pixeles[i].calculateTouching(m_width, m_height, i);
    }    
  }

  
  
}


