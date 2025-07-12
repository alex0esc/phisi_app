#include <cstdint>
#include <vector>
#include "vector.hpp"

namespace phisi::fluid {

  class FluidScreen {
    float m_gravity = 9.81; //gravity on earth
    uint32_t m_div_iters = 100; //number of iteration to clear divergence 
    float m_overrelaxation = 1.9; //as close to 2.0 as possible
    float m_density = 998.0; //for water
    float m_grid_spacing = 1.0; //1 meter

    uint32_t m_width = 0;
    uint32_t m_height = 0;
     
    std::vector<float> m_wall;
    std::vector<float> m_vel_h;
    std::vector<float> m_vel_h_buffer;
    std::vector<float> m_vel_v;
    std::vector<float> m_vel_v_buffer;
    std::vector<float> m_pressure;    
    std::vector<float> m_color_field;
    std::vector<float> m_color_field_buffer;
    
    void add_gravity(float frame_time);
    void clear_divergence(float frame_time);
    void advect_color(float frame_time);
    void advect_velocity(float frame_time);
  public:
    FluidScreen() = default;
    void allocate(uint32_t width, uint32_t height);
    void simulate(float frame_time, float stream_color[3]);
    
    void add_color(float radius, Vector2D location, float color[3]);
    void add_velocity(float radius, Vector2D direction, Vector2D location);
    
    void get_color_field(uint8_t* memory);
    void get_pressure_color(uint8_t* memory);
  };
}