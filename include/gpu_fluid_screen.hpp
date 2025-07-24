#include <cstdint>
#include <cstring>
#include <vulkan/vulkan.hpp>
#include "phisi_memory.hpp"
#include "phisi_texture.hpp"

namespace phisi::fluid {

  
  struct PushConstantData {
    static constexpr float density = 998.0; //for water
    static constexpr float grid_spacing = 1.0; //1 meter
    
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t buffer_state = true;
    uint32_t divergence_state = true;
    uint32_t rk_steps = 10;
    float gravity = 9.81;
    float overrelaxation = 1.9; 
    float frame_time = 0.01;
    float pressure_constant = 0;
    float saturation = 1.0;
    
    //pencil
    uint32_t pencil_mode = 0;    
    float pencil_radius = 30;
    float pencil_x = 0;
    float pencil_y = 0;
    float pencil_data[3];
  };
  

  class GpuFluidScreen {     
    uint32_t m_width;
    uint32_t m_height;
    
    bool m_clear_color = false;
    bool m_clear_velocity = false;
    
    vk::Device m_device;
    vk::DescriptorPool m_descriptor_pool;
    vk::detail::DispatchLoaderDynamic m_dldi;
    vk::Image m_image;
    vk::ImageView m_image_view;
    vk::Sampler m_sampler;
    vk::CommandPool m_command_pool;
    vk::Queue m_queue;

    phisi_app::PhisiMemory m_gpu_data;
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
    
  public:
    PushConstantData m_push_constant;
    uint32_t m_div_iters = 100;
    bool m_run_simulation = true;
    
    GpuFluidScreen() = default;
    GpuFluidScreen(const GpuFluidScreen& other) = delete;  
    GpuFluidScreen& operator=(const GpuFluidScreen& other) = delete;    

    void initVk(vk::PhysicalDevice physical_device, vk::Device device, 
      vk::DescriptorPool descirptor_pool, vk::detail::DispatchLoaderDynamic dldi,
      vk::CommandPool cmd_pool, vk::Queue queue) {
      m_device = device;
      m_descriptor_pool = descirptor_pool;
      m_dldi = dldi;
      m_command_pool = cmd_pool;
      m_queue = queue;
      m_gpu_data.initVk(physical_device, device, dldi);
    }      
    
    ///Image it is rendering to.
    void setImage(phisi_app::TextureData& texture_data) {
      m_image = texture_data.m_image;
      m_sampler = texture_data.m_sampler;
      m_image_view = texture_data.m_image_view;
    }
    
    void allocate(uint32_t width, uint32_t height);
    void initBuffer();

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

    void setPencilNegativDivergence(float strength) {
      m_push_constant.pencil_data[0] = strength;
      m_push_constant.pencil_mode = 3;
    }
    
    void setPencilPositivDivergence(float strength) {
      m_push_constant.pencil_data[0] = strength;
      m_push_constant.pencil_mode = 4;
    }
    
    void removePencil() { m_push_constant.pencil_mode = 0; }  
    
    void clear_color() { m_clear_color = true; }
    void clear_velocity() { m_clear_velocity = true; }
    
    void compute(vk::CommandBuffer cmd_buffer, float frame_time);
    void destroy();
  };
  
}
