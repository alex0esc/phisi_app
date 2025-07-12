#pragma once
#include <fstream>
#include <vulkan/vulkan.hpp>
#include <logger.hpp>


namespace phisi_app {
  
  inline void checkVkResult(VkResult err) {
    if (err == 0)
        return;
    LOG_ERROR("VkResult caused an error: " << err);
    std::terminate();
  }

  inline void checkVkResult(vk::Result err) {
    if (err == vk::Result::eSuccess)
        return;
    LOG_ERROR("VkResult caused an error: " << err);
    std::terminate();
  }

  template<typename T>
  inline T checkVkResult(vk::ResultValue<T> err) {
    if (err.result == vk::Result::eSuccess)
        return err.value;
    LOG_ERROR("VkResult caused an error: " << err.result);
    std::terminate();
  
  }
  
  inline uint32_t findMemoryType(vk::PhysicalDevice physical_device, uint32_t type_filter, vk::MemoryPropertyFlags flags, vk::detail::DispatchLoaderDynamic& dldi) {
    vk::PhysicalDeviceMemoryProperties mem_properties = physical_device.getMemoryProperties(dldi);
    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
      if (((type_filter) & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & flags) == flags) 
        return i;
    }
    return UINT32_MAX;
  }

  inline std::vector<uint32_t> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary); 
    if(!file.is_open()) 
      throw std::runtime_error("Failed to open file!");
  
    size_t file_size = (size_t) file.tellg();
    std::vector<uint32_t> buffer(file_size / sizeof(uint32_t), 0);

    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), file_size);
    file.close();
    return buffer;
  }
}
