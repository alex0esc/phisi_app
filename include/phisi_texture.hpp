#pragma once
#include <vulkan/vulkan.hpp>

namespace phisi_app {
  
  class TextureData {
    vk::PhysicalDevice m_physical_device;
    vk::Device m_device;
    vk::Queue m_queue;
    vk::detail::DispatchLoaderDynamic m_dldi;
    
    vk::DescriptorSet m_descriptor_set;
    vk::ImageView m_image_view;
    vk::Image m_image;
    vk::DeviceMemory m_image_memory;
    vk::Sampler m_sampler;
    vk::Buffer m_staging_buffer;
    vk::DeviceMemory m_staging_memory;
    void* m_mapped_memory;

  public:
    uint32_t m_width;
    uint32_t m_height;
    vk::DeviceSize m_image_size;
    
    TextureData() = default;
    
    TextureData(
      vk::Device device, 
      vk::PhysicalDevice physical_device, 
      vk::Queue queue,
      vk::detail::DispatchLoaderDynamic dldi) 
    : m_physical_device(physical_device), 
      m_device(device), 
      m_queue(queue),
      m_dldi(dldi) {}    
    
    void init(uint32_t width, uint32_t height);
    void load_memory(const uint8_t* memory);
    uint8_t* get_memory();
    void flush_memory();
    void copy_buffer_to_image(vk::CommandBuffer cmd_buffer);
    VkDescriptorSet get_descriptor();
    void destroy();
  };   
  
}