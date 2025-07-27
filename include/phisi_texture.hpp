#pragma once
#include "phisi_vulkan.hpp"
#include <vulkan/vulkan.hpp>

namespace phisi_app {
  
  class TextureData {
    VulkanContext* m_context = nullptr;
    
  public:
    vk::DescriptorSet m_descriptor_set;
    vk::ImageView m_image_view;
    vk::Image m_image;
    vk::DeviceMemory m_image_memory;
    vk::Sampler m_sampler;

    uint32_t m_width;
    uint32_t m_height;
    vk::DeviceSize m_image_size;
    
    TextureData() = default;
    TextureData(const TextureData& other) = delete;  
    TextureData& operator=(const TextureData& other) = delete;    

    void initVk(VulkanContext* context) { m_context = context; }
    void allocate(uint32_t width, uint32_t height);
    VkDescriptorSet getDescriptor() { return m_descriptor_set; }
    void destroy();
  };   
  
}
