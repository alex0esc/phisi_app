#include "phisi.hpp"
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>

namespace phisi {

  void FluidScreen::allocate(uint32_t width, uint32_t height) {
    m_width = width + 2;
    m_height = height + 2;
    m_vel_h.resize(m_width * m_height, 0.0);    
    m_vel_v.resize(m_height * m_width, 0.0);
    m_color_field.resize(m_width * m_height * 4);
    m_pressure.resize(m_width * m_height);
    m_wall.resize(m_width * m_height);
    for(uint32_t y = 0; y < m_height; y++) { for(uint32_t x = 0; x < m_width; x++) {
      if(y > 0 && y < m_height - 1 && x > 0 && x < m_width - 1) {
        m_wall[y * m_width + x] = 1;
      } 
    }}
  }  

  
  
  ///x and y from zero to 1 and radius in pixles
  void FluidScreen::add_color(float radius, uint8_t color[4], uint32_t center_x, uint32_t center_y) {
    for (uint32_t y = 1; y < m_height - 1; y++) { for(uint32_t x = 1; x < m_width - 1; x++) { 
      //check distance to center
      uint32_t vec_x = x - (center_x + 1);
      uint32_t vec_y = y - (center_y + 1);
      float vec_length = sqrt(vec_x * vec_x + vec_y * vec_y);
      if (vec_length <= radius) {  
        int index = (y * m_width + x) * 4;
        m_color_field[index] = color[0];
        m_color_field[index + 1] = color[1];
        m_color_field[index + 2] = color[2];
        m_color_field[index + 3] = color[3];
      }
    }}
  }

  void FluidScreen::add_gravity(float frame_time) {
    for(uint32_t y = 0; y < m_height; y++) { for(uint32_t x = 0; x < m_width; x++) {
      uint32_t index = y * m_width + x;
      if(m_wall[index] != 1.0 || y == 1) 
        continue;                  
      m_vel_v[index] += frame_time * m_gravity;
    }}  
  }

  void FluidScreen::clear_divergence(float frame_time) {
    m_pressure.assign(m_pressure.size(), 0.0f);
    float pc = (m_density * m_grid_spacing) / frame_time;
    for(uint32_t iter = 0; iter < m_div_iters; iter++) {
      bool first_cell = iter % 2;
      for(uint32_t y = 0; y < m_height; y++) { 
        first_cell = !first_cell;
        for(uint32_t x = first_cell; x < m_width - 1; x += 2) {
          uint32_t index = y * m_width + x;
          if(m_wall[index] != 1.0)
            continue;
          float divergence = -m_vel_h[index] + m_vel_h[index + 1] - m_vel_v[index] + m_vel_v[index + m_width];
          float sides = m_wall[index - 1] + m_wall[index + 1] + m_wall[index - m_width] + m_wall[index + m_width];
          divergence = (divergence / sides) * m_overrelaxation;
          m_vel_h[index] += divergence * m_wall[index - 1]; 
          m_vel_h[index + 1] -= divergence * m_wall[index + 1];
          m_vel_v[index] += divergence * m_wall[index - m_width];
          m_vel_v[index + m_width] -= divergence * m_wall[index + m_width];
          m_pressure[index] += divergence * pc;
        }
      }
    }
  }

  void interpolate(float x, float y) {
    uint32_t x_index = x;
    uint32_t y_index = y;
    
  }

  void FluidScreen::advect_color(float frame_time) {
    for(uint32_t y = 1; y < m_height - 1; y++) { for(uint32_t x = 1; x < m_width - 1; x++) {
      uint32_t index = y * m_width + x;
      Vector2D cell_vec = Vector2D{m_vel_h[index] + m_vel_h[index + 1], m_vel_v[index] + m_vel_v[index + m_width]} * -100; 
      Vector2D cell_location = {static_cast<float>(x), static_cast<float>(y)};
      while(true) {
        float factors[4];
        factors[0] = intersection(cell_location, cell_vec, {0.0, 1.0}, {1.0, 0.0}); 
        factors[1] = intersection(cell_location, cell_vec, {1.0, 0.0}, {0.0, 1.0});
        factors[2] = intersection(cell_location, cell_vec, {0.0, m_height - 1.0f}, {1.0, 0.0});     
        factors[3] = intersection(cell_location, cell_vec, {m_width - 1.0f, 0.0}, {0.0, 1.0});
        std::pair smallest = {std::numeric_limits<float>::max(), -1};
        for (uint32_t i = 0; i < 4; i++) {
          if (factors[i] > 1.0 || factors[i] <= 0.0|| factors[i] > smallest.first)
            continue;
          smallest = {factors[i], i};
        }
        if(smallest.second == -1)
          break;  
        cell_location += cell_vec * smallest.first;        
        if (smallest.second == 0 || smallest.second == 2) {  
          cell_vec = reflect(cell_vec, {1.0, 0.0}) * (1.0 - smallest.first);
        } else if (smallest.second == 1 || smallest.second == 3) {
          cell_vec = reflect(cell_vec, {0.0, 1.0}) * (1.0 - smallest.first);
        }
      }  
    }}
  }

  void FluidScreen::simulate(float frame_time) {
    add_gravity(frame_time);
    clear_divergence(frame_time);
    advect_color(frame_time);
    /*
    uint32_t index = 6000 - 160 * 3;
    float divergence = -m_vel_h[index] + m_vel_h[index + 1] - m_vel_v[index] + m_vel_v[index + m_width];
    std::cout << "Divergence: " << divergence << std::endl;
    */
  }

  void FluidScreen::get_color_field(uint8_t* memory) {
    for(uint32_t y = 1; y < m_height - 1; y++) { for(uint32_t x = 1; x < m_width - 1; x++) {      
      uint32_t index_src = y * m_width + x;
      uint32_t index_target = (y - 1) * (m_width - 2) + (x - 1);
      reinterpret_cast<uint32_t*>(memory)[index_target] = reinterpret_cast<uint32_t*>(m_color_field.data())[index_src];
    }}
  }

  void FluidScreen::calculate_pressure_color(uint8_t* memory) {
    float biggest = std::numeric_limits<float>::lowest();
    float smallest = std::numeric_limits<float>::max();
    for(uint32_t y = 1; y < m_height - 1; y++) { for(uint32_t x = 1; x < m_width - 1; x++) {
      uint32_t index = y * m_width + x;
      if(m_pressure[index] > biggest)
        biggest = m_pressure[index];
      if(m_pressure[index] < smallest)
        smallest = m_pressure[index];
    }}
    float difference = biggest - smallest;
    for(uint32_t y = 1; y < m_height - 1; y++) { for(uint32_t x = 1; x < m_width - 1; x++) {      
      float p_norm = (m_pressure[y * m_width + x] - smallest) / difference;
      uint32_t index = ((y - 1) * (m_width - 2) + (x - 1)) * 4;
      memory[index] = static_cast<uint8_t>(p_norm * 255.0);
      memory[index + 1] = 0;
      memory[index + 2] = static_cast<uint8_t>((1.0 - p_norm) * 255.0);
      memory[index + 3] = 255;
    }}
  }
}


