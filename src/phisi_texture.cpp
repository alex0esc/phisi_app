#include "phisi_texture.hpp"
#include "imgui_impl_vulkan.h"
#include "logger.hpp"
#include "phisi_util.hpp"
#include <cstdint>


namespace phisi_app {
  
  void TextureData::allocate(uint32_t width, uint32_t height) {
    m_width = width;
    m_height = height;    
    m_image_size = width * height * 4;

    vk::ImageCreateInfo image_info(
      vk::ImageCreateFlags(),
      vk::ImageType::e2D, 
      vk::Format::eR8G8B8A8Unorm, 
      vk::Extent3D(width, height, 1), 
      1, 1, 
      vk::SampleCountFlagBits::e1, 
      vk::ImageTiling::eOptimal,
      vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage,
      vk::SharingMode::eExclusive,
      {}, {},
      vk::ImageLayout::eUndefined,
      nullptr);
    m_image = m_context->m_device.createImage(image_info, nullptr, m_context->m_dldi);

    
    vk::MemoryRequirements mem_requirements = m_context->m_device.getImageMemoryRequirements(m_image);
    vk::MemoryAllocateInfo alloc_info(
      mem_requirements.size, 
      findMemoryType(m_context->m_device_physical, mem_requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, m_context->m_dldi));
    m_image_memory = m_context->m_device.allocateMemory(alloc_info, nullptr, m_context->m_dldi);
    m_context->m_device.bindImageMemory(m_image, m_image_memory, 0);
    
    
    vk::ComponentMapping component_mapping(
      vk::ComponentSwizzle::eR, 
      vk::ComponentSwizzle::eG,
      vk::ComponentSwizzle::eB,
      vk::ComponentSwizzle::eA);
    vk::ImageSubresourceRange resource_range(
      vk::ImageAspectFlagBits::eColor,
      0, 1, 0, 1);
    vk::ImageViewCreateInfo image_view_info(
      vk::ImageViewCreateFlags(), 
      m_image, 
      vk::ImageViewType::e2D, 
      vk::Format::eR8G8B8A8Unorm, 
      component_mapping, 
      resource_range);
    m_image_view = m_context->m_device.createImageView(image_view_info, nullptr, m_context->m_dldi);

    vk::SamplerCreateInfo sampler_info(
      vk::SamplerCreateFlags(),
      vk::Filter::eLinear,
      vk::Filter::eLinear,
      vk::SamplerMipmapMode::eLinear,
      vk::SamplerAddressMode::eClampToEdge,
      vk::SamplerAddressMode::eClampToEdge,
      vk::SamplerAddressMode::eClampToEdge,
      {}, 
      false,
      1.0f,
      false,
      vk::CompareOp::eNever,
      -1000,
      1000);
    m_sampler = m_context->m_device.createSampler(sampler_info, nullptr, m_context->m_dldi);  
    
    m_descriptor_set = ImGui_ImplVulkan_AddTexture(m_sampler, m_image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);     
    LOG_TRACE("Texture initialized.");    
  }  
  
  void TextureData::destroy() {
    m_context->m_device.waitIdle(m_context->m_dldi);
    ImGui_ImplVulkan_RemoveTexture(m_descriptor_set);
    m_context->m_device.destroySampler(m_sampler);
    m_context->m_device.destroyImageView(m_image_view);
    m_context->m_device.freeMemory(m_image_memory);
    m_context->m_device.destroyImage(m_image);
  }
}
