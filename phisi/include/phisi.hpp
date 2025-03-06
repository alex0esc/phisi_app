#include <cstdint>
#include <vector>
#include "vector.hpp"

namespace phisi {

  class FluidPixel {
    float m_pressure = 0;
    Vector2D m_velocity = {0, 0};
    
    uint32_t m_touch_top = UINT32_MAX;
    uint32_t m_touch_right = UINT32_MAX;
    uint32_t m_touch_bottom = UINT32_MAX;
    uint32_t m_touch_left = UINT32_MAX;
    
  public:
    FluidPixel() = default;
    void calculateTouching(uint32_t screen_width, uint32_t screen_height, uint32_t pixel_index);
  };


  class FluidScreen {
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    std::vector<FluidPixel> m_pixeles = std::vector<FluidPixel>(0); 
    
  public:
    FluidScreen(uint32_t width, uint32_t height);
    void init();
  };
}