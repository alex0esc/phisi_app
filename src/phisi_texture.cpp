#include "phisi_texture.hpp"
#include "imgui_impl_vulkan.h"
#include "logger.hpp"
#include "phisi_util.hpp"
#include <cstdint>


namespace phisi_app {
  
  void TextureData::initTransfer(uint32_t width, uint32_t height) {
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
      vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
      vk::SharingMode::eExclusive,
      {}, {},
      vk::ImageLayout::eUndefined,
      nullptr);
    m_image = m_device.createImage(image_info, nullptr, m_dldi);

    
    vk::MemoryRequirements mem_requirements = m_device.getImageMemoryRequirements(m_image);
    vk::MemoryAllocateInfo alloc_info(
      mem_requirements.size, 
      findMemoryType(m_physical_device, mem_requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, m_dldi));
    m_image_memory = m_device.allocateMemory(alloc_info, nullptr, m_dldi);
    m_device.bindImageMemory(m_image, m_image_memory, 0);
    
    
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
    m_image_view = m_device.createImageView(image_view_info, nullptr, m_dldi);

    vk::SamplerCreateInfo sampler_info(
      vk::SamplerCreateFlags(),
      vk::Filter::eNearest,
      vk::Filter::eNearest,
      vk::SamplerMipmapMode::eLinear,
      vk::SamplerAddressMode::eRepeat,
      vk::SamplerAddressMode::eRepeat,
      vk::SamplerAddressMode::eRepeat,
      {}, 
      false,
      1.0f,
      false,
      vk::CompareOp::eNever,
      -1000,
      1000);
    m_sampler = m_device.createSampler(sampler_info, nullptr, m_dldi);  
    
    m_descriptor_set = ImGui_ImplVulkan_AddTexture(m_sampler, m_image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);     
    LOG_TRACE("Texture initialized.");    
    
    vk::BufferCreateInfo buffer_info(
      vk::BufferCreateFlags(), 
      m_image_size, 
      vk::BufferUsageFlagBits::eTransferSrc, 
      vk::SharingMode::eExclusive);
    m_staging_buffer = m_device.createBuffer(buffer_info, nullptr);
    vk::MemoryRequirements mem_requirements2 = m_device.getBufferMemoryRequirements(m_staging_buffer);
    vk::MemoryAllocateInfo alloc_info2(
      mem_requirements2.size, 
      findMemoryType(m_physical_device, mem_requirements2.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible, m_dldi)); 
    m_staging_memory = m_device.allocateMemory(alloc_info2, nullptr, m_dldi);
    m_device.bindBufferMemory(m_staging_buffer, m_staging_memory, 0, m_dldi);
    
    m_mapped_memory = m_device.mapMemory(m_staging_memory, 0, m_image_size, vk::MemoryMapFlags(), m_dldi);
    LOG_TRACE("Staging buffer for texture allocated.");
  }
  
  
  void TextureData::initGpuOnly(uint32_t width, uint32_t height) {
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
    m_image = m_device.createImage(image_info, nullptr, m_dldi);

    
    vk::MemoryRequirements mem_requirements = m_device.getImageMemoryRequirements(m_image);
    vk::MemoryAllocateInfo alloc_info(
      mem_requirements.size, 
      findMemoryType(m_physical_device, mem_requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, m_dldi));
    m_image_memory = m_device.allocateMemory(alloc_info, nullptr, m_dldi);
    m_device.bindImageMemory(m_image, m_image_memory, 0);
    
    
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
    m_image_view = m_device.createImageView(image_view_info, nullptr, m_dldi);

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
    m_sampler = m_device.createSampler(sampler_info, nullptr, m_dldi);  
    
    m_descriptor_set = ImGui_ImplVulkan_AddTexture(m_sampler, m_image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);     
    LOG_TRACE("Texture initialized.");    
  }  

  
  void TextureData::loadMemoryStaging(const uint8_t* memory) {
    memcpy(m_mapped_memory, memory, m_image_size);
    flushMemoryStaging();          
  }   

  uint8_t* TextureData::getMemoryStaging() {
    return (uint8_t*) m_mapped_memory;
  }

  void TextureData::flushMemoryStaging() {
    vk::MappedMemoryRange range(
      m_staging_memory,
      0, m_image_size);
    checkVkResult(m_device.flushMappedMemoryRanges(1, &range)); 
  }

  void TextureData::copyStagingBufferToImage(vk::CommandBuffer cmd_buffer) {
      vk::ImageSubresourceRange subresource_range(
        vk::ImageAspectFlagBits::eColor, 
        0, 1, 0, 1);
      vk::ImageMemoryBarrier copy_barrier(
        {}, 
        vk::AccessFlagBits::eTransferWrite, 
        vk::ImageLayout::eUndefined, 
        vk::ImageLayout::eTransferDstOptimal,
        vk::QueueFamilyIgnored,
        vk::QueueFamilyIgnored,
        m_image,
        subresource_range,
        nullptr);
      cmd_buffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe, 
        vk::PipelineStageFlagBits::eTransfer, 
        {}, 
        0, nullptr, 
        0, nullptr, 
        1, &copy_barrier, m_dldi);


      vk::ImageSubresourceLayers subresource_layers(
        vk::ImageAspectFlagBits::eColor, 
        0, 0, 1);
      vk::BufferImageCopy region(
        0, 0, 0, 
        subresource_layers, 
        0, 
        vk::Extent3D(m_width, m_height, 1));
      cmd_buffer.copyBufferToImage(
        m_staging_buffer, 
        m_image, 
        vk::ImageLayout::eTransferDstOptimal, 
        1, &region, 
      m_dldi);

      vk::ImageMemoryBarrier use_barrier(
        vk::AccessFlagBits::eTransferWrite, 
        vk::AccessFlagBits::eShaderRead, 
        vk::ImageLayout::eTransferDstOptimal, 
        vk::ImageLayout::eShaderReadOnlyOptimal,
        vk::QueueFamilyIgnored,
        vk::QueueFamilyIgnored,
        m_image,
        subresource_range,
        nullptr);
      cmd_buffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTransfer, 
        vk::PipelineStageFlagBits::eFragmentShader, 
        {}, 
        0, nullptr, 
        0, nullptr, 
        1, &use_barrier, 
      m_dldi);   
  }

  
  VkDescriptorSet TextureData::getDescriptor() {
    return m_descriptor_set;
  }

  void TextureData::destroyTransfer() {
    m_device.unmapMemory(m_staging_memory, m_dldi);    
    m_device.freeMemory(m_staging_memory);
    m_device.destroyBuffer(m_staging_buffer);
    ImGui_ImplVulkan_RemoveTexture(m_descriptor_set);
    m_device.destroySampler(m_sampler);
    m_device.destroyImageView(m_image_view);
    m_device.freeMemory(m_image_memory);
    m_device.destroyImage(m_image);
  }
  
  void TextureData::destroyGpuOnly() {
    ImGui_ImplVulkan_RemoveTexture(m_descriptor_set);
    m_device.destroySampler(m_sampler);
    m_device.destroyImageView(m_image_view);
    m_device.freeMemory(m_image_memory);
    m_device.destroyImage(m_image);
  }

}
