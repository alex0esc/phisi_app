#include <cstdint>
#include <cstring>
#include "phisi_texture.hpp"
#include "phisi_memory.hpp"

namespace phisi_app {

  struct PushConstantData {
    static constexpr float density = 998.0; //for water
    static constexpr float grid_spacing = 1.0; //1 meter
    
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t buffer_state = true;
    uint32_t divergence_state = true;
    uint32_t rk_steps = 10;
    float gravity = 9.81;
    float overrelaxation = 1.975; 
    float frame_time = 0.01;
    float pressure_constant = 0;
    float saturation = 1.0;
    
    //pencil
    uint32_t pencil_mode = 0;    
    float pencil_radius = 30;
    float pencil_x = 0;
    float pencil_y = 0;
    float pencil_data[3];

    //stream
    float stream_color[3] = {1.0, 0.0, 0.0};
    float stream_velocity = 100;
  };
  

  class GpuFluidScreen {     
    uint32_t m_width;
    uint32_t m_height;
    
    bool m_clear_color = false;
    bool m_clear_velocity = false;
    
    VulkanContext* m_context;
    TextureData* m_texture;
    PhisiMemory m_memory;

    vk::Fence m_fence;
    vk::CommandPool m_cmd_pool;
    
    vk::DescriptorSetLayout m_descriptor_layout;
    vk::DescriptorSet m_descriptor_set;
    vk::PipelineLayout m_pipeline_layout;
        
    vk::ShaderModule m_module_force;
    vk::ShaderModule m_module_velocity;
    vk::ShaderModule m_module_divergence;
    vk::ShaderModule m_module_color;
    
    vk::Pipeline m_pipeline_force;
    vk::Pipeline m_pipeline_velocity;
    vk::Pipeline m_pipeline_divergence;    
    vk::Pipeline m_pipeline_color;
    
    void compute(vk::CommandBuffer cmd_buffer);
    
  public:
    PushConstantData m_push_constant;
    uint32_t m_div_iters = 100;
    bool m_run_simulation = true;
    
    GpuFluidScreen() = default;
    GpuFluidScreen(const GpuFluidScreen& other) = delete;  
    GpuFluidScreen& operator=(const GpuFluidScreen& other) = delete;    

    void initVk(VulkanContext* context, TextureData* texture) {
      m_memory.initVk(context);
      m_context = context;
      m_texture = texture;
    }      
    
    void allocate();
    void initBuffer();
    void simulate();
    void destroy();

    void setPencilRadius(float radius) {
      m_push_constant.pencil_radius = radius;
    }
    
    void setPencilPosition(float x, float y) {
      m_push_constant.pencil_x = x;
      m_push_constant.pencil_y = y;
    }
    
    void setPencilColor(float color[3]) {  
      m_push_constant.pencil_mode = 1;
      memcpy(m_push_constant.pencil_data, color, 12);
    }
    
    void setPencilVelocity(float direction[2]) {
      m_push_constant.pencil_mode = 2;
      memcpy(m_push_constant.pencil_data, direction, 8);
    }
    
    void setPencilDivergence(float strength) {
      m_push_constant.pencil_data[0] = strength;
      m_push_constant.pencil_mode = 3;
    }
    
    void removePencil() { m_push_constant.pencil_mode = 0; }  
    
    void clear_color() { m_clear_color = true; }
    void clear_velocity() { m_clear_velocity = true; }
  };
}
