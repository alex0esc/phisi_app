#include <vulkan/vulkan.hpp>

namespace phisi_app {
  
  class PhisiMemory {
    vk::PhysicalDevice m_physical_device;
    vk::Device m_device;
    vk::detail::DispatchLoaderDynamic m_dldi;
    bool m_staging = false;
    vk::Buffer m_staging_buffer;
    vk::DeviceMemory m_staging_memory;
    
  public:
    vk::DeviceSize m_size = -1;
    vk::Buffer m_buffer;
    vk::DeviceMemory m_memory;
    void* m_mapped_memory = nullptr;
  
    PhisiMemory() = default;
    PhisiMemory(const PhisiMemory& other) = delete;  
    PhisiMemory& operator=(const PhisiMemory& other) = delete;    

     void initVk(vk::PhysicalDevice physical_device, vk::Device device, vk::detail::DispatchLoaderDynamic dldi) {
      m_physical_device = physical_device;
      m_device = device;
      m_dldi = dldi;
    } 
    void allocate(
      vk::DeviceSize size, 
      vk::MemoryPropertyFlagBits properties, 
      vk::Flags<vk::BufferUsageFlagBits> usage
    );
    void allocateStaging();
    void map();
    void uploadStaging(vk::CommandBuffer cmd_buffer);
    void uploadStaging(vk::CommandPool cmd_pool, vk::Queue queue);
    void unmap();
    void destoryStaging();
    void destroy();
  };
  
}
