#pragma once
#include <vulkan/vulkan.hpp>

namespace phisi_app {
  
  class TextureData {
    vk::PhysicalDevice m_physical_device;
    vk::Device m_device;
    vk::detail::DispatchLoaderDynamic m_dldi;
    
  public:
    vk::DescriptorSet m_descriptor_set;
    vk::ImageView m_image_view;
    vk::Image m_image;
    vk::DeviceMemory m_image_memory;
    vk::Sampler m_sampler;
    vk::Buffer m_staging_buffer;
    vk::DeviceMemory m_staging_memory;
    void* m_mapped_memory;

    uint32_t m_width;
    uint32_t m_height;
    vk::DeviceSize m_image_size;
    
    TextureData() = default;
    TextureData(const TextureData& other) = delete;  
    TextureData& operator=(const TextureData& other) = delete;    
    
    void initVk(vk::PhysicalDevice physical_device, vk::Device device, vk::detail::DispatchLoaderDynamic dldi) {
      m_physical_device = physical_device;
      m_device = device;
      m_dldi = dldi;
    }
    void initTransfer(uint32_t width, uint32_t height);
    void initGpuOnly(uint32_t width, uint32_t height);
    void allocateStagingBuffer();
    void loadMemoryStaging(const uint8_t* memory);
    uint8_t* getMemoryStaging();
    void flushMemoryStaging();
    void copyStagingBufferToImage(vk::CommandBuffer cmd_buffer);
    VkDescriptorSet getDescriptor();
    void destroyTransfer();
    void destroyGpuOnly();
  };   
  
}
