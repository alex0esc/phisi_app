#pragma once
#include <vulkan/vulkan.hpp>

namespace phisi_app {
  
  class TextureData {
    vk::PhysicalDevice m_physical_device;
    vk::Device m_device;
    uint32_t m_queue_family_index;
    vk::detail::DispatchLoaderDynamic m_dldi;
    
    vk::DescriptorSet m_descriptor_set;
    vk::ImageView m_image_view;
    vk::Image m_image;
    vk::DeviceMemory m_image_memory;
    vk::Sampler m_sampler;
    vk::Buffer m_staging_buffer;
    vk::DeviceMemory m_staging_memory;

    TextureData(const TextureData& other) = delete;
    TextureData operator=(const TextureData& other) = delete;
    TextureData(
      vk::Device device, 
      vk::PhysicalDevice physical_device, 
      uint32_t queue_family_index,
      vk::detail::DispatchLoaderDynamic dldi) 
    : m_physical_device(physical_device), 
      m_device(device), 
      m_queue_family_index(queue_family_index),
      m_dldi(dldi) {}    
    
    void loadTextureFromMemoryRGBA(const uint8_t* memory, uint32_t width, uint32_t height);
  };   
  
}