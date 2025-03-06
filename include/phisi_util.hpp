#pragma once
#include <stdlib.h>
#include <vulkan/vulkan.hpp>
#include <logger.hpp>


namespace phisi_app {
  
  
  inline void checkVkResult(VkResult err) {
    if (err == 0)
        return;
    LOG_ERROR("VkResult caused an error: " << err);
    if (err < 0)
        abort();
  }

  inline void checkVkResult(vk::Result err) {
    if (err == vk::Result::eSuccess)
        return;
    LOG_ERROR("VkResult caused an error: " << err);
    abort();
  }
  
  inline uint32_t findMemoryType(vk::PhysicalDevice physical_device, uint32_t type_filter, vk::MemoryPropertyFlags flags, vk::detail::DispatchLoaderDynamic& dldi) {
    vk::PhysicalDeviceMemoryProperties mem_properties = physical_device.getMemoryProperties(dldi);
    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
      if (((type_filter) & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & flags) == flags) 
        return i;
    }
    return UINT32_MAX;
  }
}