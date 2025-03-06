#include "phisi_texture.hpp"
#include "phisi_util.hpp"
#include <cstdint>

namespace phisi_app {
  
  void TextureData::loadTextureFromMemoryRGBA(const uint8_t* memory, uint32_t width, uint32_t height) {
    size_t image_size = width * height * 4;    
    
    vk::ImageCreateInfo image_info(
      vk::ImageCreateFlags(),
      vk::ImageType::e2D, 
      vk::Format::eB8G8R8A8Unorm, 
      vk::Extent3D(width, height, 1), 
      1, 1, 
      vk::SampleCountFlagBits::e1, 
      vk::ImageTiling::eOptimal,
      vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc,
      vk::SharingMode::eExclusive,
      1, &m_queue_family_index,
      vk::ImageLayout::eUndefined,
      nullptr);
    m_image = m_device.createImage(image_info, nullptr, m_dldi);
    vk::MemoryRequirements mem_requirements = m_device.getImageMemoryRequirements(m_image);
    vk::MemoryAllocateInfo alloc_info(
      mem_requirements.size, 
      findMemoryType(m_physical_device, mem_requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, m_dldi));
    m_image_memory = m_device.allocateMemory(alloc_info, nullptr, m_dldi);

    
    
  }   
}