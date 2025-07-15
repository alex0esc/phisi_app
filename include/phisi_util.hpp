#pragma once
#ifdef _WIN32
#include <windows.h>
#undef MemoryBarrier
#endif
#include <chrono>
#include <fstream>
#include <thread>
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

  inline void sleep_for(uint64_t micros) {
    #ifdef _WIN32
      timeBeginPeriod(4);
    #endif
    auto target_time = std::chrono::high_resolution_clock::now() + + std::chrono::microseconds(micros);
    while(target_time > std::chrono::high_resolution_clock::now()) {
      auto time_left = target_time - std::chrono::high_resolution_clock::now();
      if(time_left > std::chrono::milliseconds(4))
        std::this_thread::sleep_for(std::chrono::milliseconds(4));  
    }
    #ifdef _WIN32
      timeEndPeriod(4);
    #endif
  }
  
  /*inline std::string handleInclude(const std::filesystem::path& file_path) {
    std::ifstream file(file_path, std::ios::binary); 
    if(!file.is_open()) { 
      LOG_ERROR("Failed to open file " << file_path.filename() << "!");
      std::terminate();
    }
    std::stringstream str_buffer;
    str_buffer << file.rdbuf();
    file.close();
    
    std::string shader_code = str_buffer.str();
    
    while(true) {
      size_t index_start = shader_code.find("#include");
      size_t index = index_start;
      if(index == std::string::npos)
        break;
      index += 8;
      while(std::isspace(shader_code[index])) {
        index += 1;
      }
      if(shader_code[index] != '"') {
        LOG_ERROR("Wrong include declaration in file " << file_path.filename() << ".");
        std::terminate();
      }
      index += 1;
      std::string inc_filename = ""; 
      while(shader_code[index] != '"') {
        inc_filename.push_back(shader_code[index]);
        index += 1;
      }
      index += 1;
      std::filesystem::path inc_path = file_path.parent_path() / inc_filename;
      std::string inc_shader_code = handleInclude(inc_path); 
      
      std::string before_inc = shader_code.substr(0, index_start);
      std::string after_inc = shader_code.substr(index, shader_code.length());
      shader_code = before_inc.append(inc_shader_code).append(after_inc);
    }
    return shader_code;
  }*/


  /*inline std::vector<uint32_t> readFile(const std::filesystem::path& file_path) {
    //handle includes 
    std::string shader_code = handleInclude(file_path);
    std::vector<uint32_t> buffer(shader_code.length() / sizeof(uint32_t), 0);
    memcpy(buffer.data(), shader_code.data(), shader_code.size());
    
    return buffer;
  }*/
}
