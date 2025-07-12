#include "fluid_screen.hpp"
#include <cmath>
#include <cstdint>
#include <limits>

namespace phisi::fluid {

  void FluidScreen::allocate(uint32_t width, uint32_t height) {
    m_width = width + 2;
    m_height = height + 2;
    m_vel_h.resize(m_width * m_height, 0.0);    
    m_vel_h_buffer.resize(m_width * m_height, 0.0);
    m_vel_v.resize(m_height * m_width, 0.0);
    m_vel_v_buffer.resize(m_height * m_width, 0.0);
    m_color_field.resize(m_width * m_height * 3, 0.0);
    m_color_field_buffer.resize(m_width * m_height * 3, 0.0);
    m_pressure.resize(m_width * m_height);
    m_wall.resize(m_width * m_height, 0.0f);
    for(uint32_t y = 1; y < m_height - 1; y++) { for(uint32_t x = 1; x < m_width - 1; x++) {
      m_wall[y * m_width + x] = 1.0; 
    }}
  }  

  
  ///x and y from zero to 1 and radius in pixles
  void FluidScreen::add_color(float radius, Vector2D location, float color[3]) {
    for (uint32_t y = 1; y < m_height - 1; y++) { for(uint32_t x = 1; x < m_width - 1; x++) { 
      //check distance to center
      uint32_t vec_x = x - (location.m_x + 0.5);
      uint32_t vec_y = y - (location.m_y + 0.5);
      float vec_length = sqrt(vec_x * vec_x + vec_y * vec_y);
      //paint pixles
      if (vec_length <= radius) {  
        int index = (y * m_width + x) * 3;
        m_color_field[index] = color[0];
        m_color_field[index + 1] = color[1];
        m_color_field[index + 2] = color[2];
      }
    }}
  }

  void FluidScreen::add_velocity(float radius, Vector2D direction, Vector2D location) {
    for (uint32_t y = 1; y < m_height - 1; y++) { for(uint32_t x = 1; x < m_width - 1; x++) { 
      //check distance to center
      uint32_t vec_x = x - (location.m_x + 0.5);
      uint32_t vec_y = y - (location.m_y + 0.5);
      float vec_length = sqrt(vec_x * vec_x + vec_y * vec_y);
      //paint pixles
      if (vec_length <= radius) {  
        int index = y * m_width + x;
        if(x != 1)
          m_vel_h[index] = direction.m_x;
        if(y != 1)
          m_vel_v[index] = direction.m_y;
      }
    }}
  }

  
  void FluidScreen::add_gravity(float frame_time) {
    for(uint32_t y = 2; y < m_height - 1; y++) { for(uint32_t x = 1; x < m_width - 1; x++) {
      uint32_t index = y * m_width + x;                  
      m_vel_v[index] += frame_time * m_gravity;
    }}    
  }

  void FluidScreen::clear_divergence(float frame_time) {
    m_pressure.assign(m_pressure.size(), 0.0f);
    float pc = (m_density * m_grid_spacing) / frame_time;
    for(uint32_t iter = 0; iter < m_div_iters; iter++) {
      bool first_cell = iter % 2;
      for(uint32_t y = 1; y < m_height - 1; y++) { 
        first_cell = !first_cell;
        for(uint32_t x = first_cell + 1; x < m_width - 1; x += 2) {
          uint32_t index = y * m_width + x;
          float divergence = -m_vel_h[index] + m_vel_h[index + 1] - m_vel_v[index] + m_vel_v[index + m_width];
          float sides = m_wall[index - 1] + m_wall[index + 1] + m_wall[index - m_width] + m_wall[index + m_width];
          divergence = (divergence / sides) * m_overrelaxation;
          m_vel_h[index] += divergence * m_wall[index - 1]; 
          m_vel_h[index + 1] -= divergence * m_wall[index + 1];
          m_vel_v[index] += divergence * m_wall[index - m_width];
          m_vel_v[index + m_width] -= divergence * m_wall[index + m_width];
          m_pressure[index] -= divergence * pc;
        }
      }
    }
  }
  
  void FluidScreen::advect_color(float frame_time) {
    for(uint32_t y = 1; y < m_height - 1; y++) { for(uint32_t x = 1; x < m_width - 1; x++) {
      //define variables
      uint32_t index = y * m_width + x;
      float multiply_h = 2.0 / (m_wall[index - 1] + m_wall[index + 1]);
      float vel_h = m_vel_h[index] * m_wall[index - 1] + m_vel_h[index + 1] * m_wall[index + 1];
      float multiply_v = 2.0 / (m_wall[index - m_width] + m_wall[index + m_width]);
      float vel_v = m_vel_v[index] * m_wall[index - m_width] + m_vel_v[index + m_width] * m_wall[index + m_width];
      Vector2D cell_vec = Vector2D{vel_h * multiply_h, vel_v * multiply_v} 
        * -frame_time; 
      Vector2D cell_loc = {static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f};
      //trace back and reflect on walls
      while(true) {
        float factors[4];
        factors[0] = intersection(cell_loc, cell_vec, {0.0, 1.0}, {1.0, 0.0}); 
        factors[1] = intersection(cell_loc, cell_vec, {1.0, 0.0}, {0.0, 1.0});
        factors[2] = intersection(cell_loc, cell_vec, {0.0, m_height - 1.0f}, {1.0, 0.0});     
        factors[3] = intersection(cell_loc, cell_vec, {m_width - 1.0f, 0.0}, {0.0, 1.0});
        std::pair smallest = {std::numeric_limits<float>::max(), -1};
        for (uint32_t i = 0; i < 4; i++) {
          if (factors[i] > 1.0 || factors[i] <= 0.0 || factors[i] > smallest.first)
            continue;
          smallest = {factors[i], i};
        }
        if(smallest.second == -1) {   
          cell_loc += cell_vec;
          break;
        }
        cell_loc += cell_vec * smallest.first * 0.9999;        
        if (smallest.second == 0 || smallest.second == 2) {  
          cell_vec = reflect(cell_vec, {1.0, 0.0}) * (1.0 - smallest.first);
        } else if (smallest.second == 1 || smallest.second == 3) {
          cell_vec = reflect(cell_vec, {0.0, 1.0}) * (1.0 - smallest.first);
        }
      }  
      //interpolate and advect color
      cell_loc -= {0.5 , 0.5};
      uint32_t x_traced = static_cast<uint32_t>(cell_loc.m_x);
      uint32_t y_traced = static_cast<uint32_t>(cell_loc.m_y);
      float x_inner = cell_loc.m_x - x_traced;
      float y_inner = cell_loc.m_y - y_traced;
      uint32_t index_1 = (y_traced * m_width + x_traced);
      uint32_t index_2 = (y_traced * m_width + (x_traced + 1));      
      uint32_t index_3 = ((y_traced + 1) * m_width + (x_traced + 1));
      uint32_t index_4 = ((y_traced + 1) * m_width + x_traced);
      float weight_1 = (1.0 - x_inner) * (1.0 - y_inner) * m_wall[index_1];
      float weight_2 = x_inner * (1.0 - y_inner) * m_wall[index_2];
      float weight_3 = x_inner * y_inner * m_wall[index_3];
      float weight_4 = (1.0 - x_inner) * y_inner * m_wall[index_4];
      float wall_add = 1.0 / (weight_1 + weight_2 + weight_3 + weight_4);
      for(int i = 0; i < 3; i++) {        
        m_color_field_buffer[index * 3 + i] = wall_add * 
          (m_color_field[index_1 * 3 + i] * weight_1 + 
          m_color_field[index_2 * 3 + i] * weight_2 + 
          m_color_field[index_3 * 3 + i] * weight_3 + 
          m_color_field[index_4 * 3 + i] * weight_4);
      }
    }}
    std::swap(m_color_field, m_color_field_buffer);
  }
  
  
  void FluidScreen::advect_velocity(float frame_time) {
    //vertical
    for(uint32_t y = 2; y < m_height - 1; y++) { for(uint32_t x = 1; x < m_width - 1; x++) {
      //define variables
      uint32_t index = y * m_width + x;
      float vel_h = (m_vel_h[index] + m_vel_h[index + 1] + m_vel_h[index - m_width] + m_vel_h[index - m_width + 1]) / 4.0;
      Vector2D cell_vec = Vector2D{vel_h, m_vel_v[index]} * -frame_time;
      Vector2D cell_loc = {static_cast<float>(x) + 0.5f, static_cast<float>(y)};
      //trace back and reflect on walls
      while(true) {
        float factors[4];
        factors[0] = intersection(cell_loc, cell_vec, {0.0, 1.0}, {1.0, 0.0}); 
        factors[1] = intersection(cell_loc, cell_vec, {1.0, 0.0}, {0.0, 1.0});
        factors[2] = intersection(cell_loc, cell_vec, {0.0, m_height - 1.0f}, {1.0, 0.0});     
        factors[3] = intersection(cell_loc, cell_vec, {m_width - 1.0f, 0.0}, {0.0, 1.0});
        std::pair smallest = {std::numeric_limits<float>::max(), -1};
        for (uint32_t i = 0; i < 4; i++) {
          if (factors[i] > 1.0 || factors[i] <= 0.0 || factors[i] > smallest.first)
            continue;
          smallest = {factors[i], i};
        }
        if(smallest.second == -1) {   
          cell_loc += cell_vec;
          break;
        }
        cell_loc += cell_vec * smallest.first * 0.9999;        
        if (smallest.second == 0 || smallest.second == 2) {  
          cell_vec = reflect(cell_vec, {1.0, 0.0}) * (1.0 - smallest.first);
        } else if (smallest.second == 1 || smallest.second == 3) {
          cell_vec = reflect(cell_vec, {0.0, 1.0}) * (1.0 - smallest.first);
        }
      }  
      //interpolate and calculate velocity
      cell_loc -= {0.5, 0.0};
      uint32_t x_traced = static_cast<uint32_t>(cell_loc.m_x);
      uint32_t y_traced = static_cast<uint32_t>(cell_loc.m_y);
      float x_inner = cell_loc.m_x - x_traced;
      float y_inner = cell_loc.m_y - y_traced;
      uint32_t index_1 = (y_traced * m_width + x_traced);
      uint32_t index_2 = (y_traced * m_width + (x_traced + 1));      
      uint32_t index_3 = ((y_traced + 1) * m_width + (x_traced + 1));
      uint32_t index_4 = ((y_traced + 1) * m_width + x_traced);
      float weight_1 = (1.0 - x_inner) * (1.0 - y_inner) * m_wall[index_1];
      float weight_2 = x_inner * (1.0 - y_inner) * m_wall[index_2];
      float weight_3 = x_inner * y_inner * m_wall[index_3];
      float weight_4 = (1.0 - x_inner) * y_inner * m_wall[index_4];
      float wall_add = 1.0 / (weight_1 + weight_2 + weight_3 + weight_4);
      m_vel_v_buffer[index] = wall_add *
        (m_vel_v[index_1] * weight_1 + 
        m_vel_v[index_2] * weight_2 + 
        m_vel_v[index_3] * weight_3 + 
        m_vel_v[index_4] * weight_4); 
    }} 
    //horizontal
    for(uint32_t y = 1; y < m_height - 1; y++) { for(uint32_t x = 2; x < m_width - 1; x++) {
      //define variables
      uint32_t index = y * m_width + x;
      float vel_v = (m_vel_v[index - 1] + m_vel_v[index] + m_vel_v[index + m_width] + m_vel_v[index + m_width - 1]) / 4.0;
      Vector2D cell_vec = Vector2D{m_vel_h[index], vel_v} * -frame_time;
      Vector2D cell_loc = {static_cast<float>(x), static_cast<float>(y) + 0.5f};
      //trace back and reflect on walls
      while(true) {
        float factors[4];
        factors[0] = intersection(cell_loc, cell_vec, {0.0, 1.0}, {1.0, 0.0}); 
        factors[1] = intersection(cell_loc, cell_vec, {1.0, 0.0}, {0.0, 1.0});
        factors[2] = intersection(cell_loc, cell_vec, {0.0, m_height - 1.0f}, {1.0, 0.0});     
        factors[3] = intersection(cell_loc, cell_vec, {m_width - 1.0f, 0.0}, {0.0, 1.0});
        std::pair smallest = {std::numeric_limits<float>::max(), -1};
        for (uint32_t i = 0; i < 4; i++) {
          if (factors[i] > 1.0 || factors[i] <= 0.0 || factors[i] > smallest.first)
            continue;
          smallest = {factors[i], i};
        }
        if(smallest.second == -1) {   
          cell_loc += cell_vec;
          break;
        }
        cell_loc += cell_vec * smallest.first * 0.9999;        
        if (smallest.second == 0 || smallest.second == 2) {  
          cell_vec = reflect(cell_vec, {1.0, 0.0}) * (1.0 - smallest.first);
        } else if (smallest.second == 1 || smallest.second == 3) {
          cell_vec = reflect(cell_vec, {0.0, 1.0}) * (1.0 - smallest.first);
        }
      }  
      //interpolate and calculate velocity
      cell_loc -= {0.0, 0.5};
      uint32_t x_traced = static_cast<uint32_t>(cell_loc.m_x);
      uint32_t y_traced = static_cast<uint32_t>(cell_loc.m_y);
      float x_inner = cell_loc.m_x - x_traced;
      float y_inner = cell_loc.m_y - y_traced;
      uint32_t index_1 = (y_traced * m_width + x_traced);
      uint32_t index_2 = (y_traced * m_width + (x_traced + 1));      
      uint32_t index_3 = ((y_traced + 1) * m_width + (x_traced + 1));
      uint32_t index_4 = ((y_traced + 1) * m_width + x_traced);
      float weight_1 = (1.0 - x_inner) * (1.0 - y_inner) * m_wall[index_1];
      float weight_2 = x_inner * (1.0 - y_inner) * m_wall[index_2];
      float weight_3 = x_inner * y_inner * m_wall[index_3];
      float weight_4 = (1.0 - x_inner) * y_inner * m_wall[index_4];
      float wall_add = 1.0 / (weight_1 + weight_2 + weight_3 + weight_4);
      m_vel_h_buffer[index] = wall_add *
        (m_vel_h[index_1] * weight_1 + 
        m_vel_h[index_2] * weight_2 + 
        m_vel_h[index_3] * weight_3 + 
        m_vel_h[index_4] * weight_4); 
    }}
    std::swap(m_vel_h, m_vel_h_buffer);
    std::swap(m_vel_v, m_vel_v_buffer);
  }
  
    
  void FluidScreen::simulate(float frame_time, float stream_color[3]) {
     //TODO Test Code
    for(int i = -5; i < 5; i++) {
        m_vel_v[m_width * 1.5 + i] = 20.0;
        m_color_field[(m_width * 1.5 + i) * 3] = stream_color[0];
        m_color_field[(m_width * 1.5 + i) * 3 + 1] = stream_color[1];
        m_color_field[(m_width * 1.5 + i) * 3 + 2] = stream_color[2];
    } 
    frame_time = frame_time > 1.0 ? 1.0 : frame_time;
    add_gravity(frame_time);
    advect_velocity(frame_time);
    clear_divergence(frame_time);
    advect_color(frame_time);
  }

  void FluidScreen::get_color_field(uint8_t* memory) {
    for(uint32_t y = 1; y < m_height - 1; y++) { for(uint32_t x = 1; x < m_width - 1; x++) {      
      uint32_t index_src = (y * m_width + x) * 3;
      uint32_t index_target = ((y - 1) * (m_width - 2) + (x - 1)) * 4;
      memory[index_target] = m_color_field[index_src] * 255;
      memory[index_target + 1] = m_color_field[index_src + 1] * 255;
      memory[index_target + 2] = m_color_field[index_src + 2] * 255;
      memory[index_target + 3] = 255; 
    }}
  }

  void FluidScreen::get_pressure_color(uint8_t* memory) {
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


