#include <cstdint>
#include <cstring>
#include <vulkan/vulkan.hpp>
#include "phisi_memory.hpp"
#include "phisi_texture.hpp"


namespace phisi::fluid {

  class GpuFluidScreen {     
    static constexpr float m_density = 998.0; //for water
    static constexpr float m_grid_spacing = 1.0; //1 meter
    uint32_t m_width = 0;
    uint32_t m_height = 0;    
    bool m_buffer_state = true;

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
        
    uint32_t m_pencil_mode = 0;
    float m_pencil_radius = 10.0f;
    float m_pencil_coords[2] = {0.0f, 0.0f};
    float m_pencil_data[3] = {0.0f, 0.0f, 0.0f}; 
    
  public:
    uint32_t m_div_iters = 100;
    uint32_t m_rk_steps = 10;
    float m_gravity = 9.81;
    float m_overrelaxation = 1.9;
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
      m_pencil_radius = radius;
    }
    void setPencilPosition(float x, float y) {
      m_pencil_coords[0] = x;
      m_pencil_coords[1] = y;
    }
    void setPencilColor(float color[3]) {  
      m_pencil_mode = 1;
      memcpy(m_pencil_data, color, 12);
    }
    void setPencilVelocity(float direction[2]) {
      m_pencil_mode = 2;
      memcpy(m_pencil_data, direction, 8);
    }

    void setPencilNegativDivergence(float strength) {
      m_pencil_data[0] = strength;
      m_pencil_mode = 3;
    }
    
    void setPencilPositivDivergence(float strength) {
      m_pencil_data[0] = strength;
      m_pencil_mode = 4;
    }
    
    void removePencil() { m_pencil_mode = 0; }
    
    void compute(vk::CommandBuffer cmd_buffer, float frame_time);
    void destroy();
  };
  
}
