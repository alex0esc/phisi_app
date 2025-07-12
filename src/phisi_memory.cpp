#include "phisi_memory.hpp"
#include "phisi_util.hpp"

namespace phisi_app {
  
  void PhisiMemory::allocate(vk::DeviceSize size, 
    vk::MemoryPropertyFlagBits properties, vk::Flags<vk::BufferUsageFlagBits> usage) {
    m_size = size;
    vk::BufferCreateInfo buffer_info(
      vk::BufferCreateFlags(), 
      m_size, 
      usage, 
      vk::SharingMode::eExclusive);
    m_buffer = m_device.createBuffer(buffer_info, nullptr);
    vk::MemoryRequirements mem_requirements2 = m_device.getBufferMemoryRequirements(m_buffer);
    vk::MemoryAllocateInfo alloc_info2(
      mem_requirements2.size, 
      findMemoryType(m_physical_device, mem_requirements2.memoryTypeBits, properties, m_dldi)); 
    m_memory = m_device.allocateMemory(alloc_info2, nullptr, m_dldi);
    m_device.bindBufferMemory(m_buffer, m_memory, 0, m_dldi);
  }

  void PhisiMemory::allocateStaging() {
    vk::BufferCreateInfo buffer_info(
      vk::BufferCreateFlags(), 
      m_size, 
      vk::BufferUsageFlagBits::eTransferSrc, 
      vk::SharingMode::eExclusive);
    m_staging_buffer = m_device.createBuffer(buffer_info, nullptr);
    vk::MemoryRequirements mem_requirements2 = m_device.getBufferMemoryRequirements(m_staging_buffer);
    vk::MemoryAllocateInfo alloc_info2(
      mem_requirements2.size, 
      findMemoryType(m_physical_device, mem_requirements2.memoryTypeBits, 
        vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostCoherent, m_dldi)); 
    m_staging_memory = m_device.allocateMemory(alloc_info2, nullptr, m_dldi);
    m_device.bindBufferMemory(m_staging_buffer, m_staging_memory, 0, m_dldi);        
    m_staging = true;
  }

  void PhisiMemory::map() {
    if(m_staging)
      m_mapped_memory = m_device.mapMemory(m_staging_memory, 0, m_size, vk::MemoryMapFlags(), m_dldi);
    else
      m_mapped_memory = m_device.mapMemory(m_memory, 0, m_size, vk::MemoryMapFlags(), m_dldi);
  }
  
  void PhisiMemory::uploadStaging(vk::CommandBuffer cmd_buffer) {            
      vk::BufferCopy region(0, 0, m_size);
      cmd_buffer.copyBuffer(
        m_staging_buffer, 
        m_buffer,  
        1, &region, 
        m_dldi);
  }

  
  void PhisiMemory::uploadStaging(vk::CommandPool cmd_pool, vk::Queue queue) {            
    vk::CommandBufferAllocateInfo alc_info(
      cmd_pool, 
      vk::CommandBufferLevel::ePrimary, 
      1);
    vk::CommandBuffer cmd_buffer;
    checkVkResult(m_device.allocateCommandBuffers(&alc_info, &cmd_buffer));
    vk::CommandBufferBeginInfo begin_info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    cmd_buffer.begin(begin_info, m_dldi);

    uploadStaging(cmd_buffer);

    cmd_buffer.end(m_dldi);

    vk::SubmitInfo submit_info(0, nullptr, nullptr, 1, &cmd_buffer);
    queue.submit(submit_info);
    queue.waitIdle(m_dldi);
    m_device.freeCommandBuffers(cmd_pool, 1 , &cmd_buffer, m_dldi);
  }
  
  void PhisiMemory::unmap() {
    if(m_staging)
      m_device.unmapMemory(m_staging_memory, m_dldi);
    else
      m_device.unmapMemory(m_memory, m_dldi);
  }
  
  
  void PhisiMemory::destoryStaging() {
    m_device.freeMemory(m_staging_memory);
    m_device.destroyBuffer(m_staging_buffer);
  }    

  void PhisiMemory::destroy() {
    m_device.freeMemory(m_memory);
    m_device.destroyBuffer(m_buffer);
  }
}