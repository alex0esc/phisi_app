#include "gpu_fluid_screen.hpp"
#include "logger.hpp"
#include "phisi_util.hpp"
#include <vulkan/vulkan_enums.hpp>

namespace phisi_app {
  
  vk::ShaderModule create_module(vk::Device device, vk::detail::DispatchLoaderDynamic dldi, const std::string& path) {
    std::vector<uint32_t> shader_data = phisi_app::readFile(path);
    vk::ShaderModuleCreateInfo module_info(
      vk::ShaderModuleCreateFlags(), 
      shader_data.size() * sizeof(uint32_t), 
      shader_data.data());    
    return device.createShaderModule(module_info, nullptr, dldi);
  }



  vk::Pipeline create_pipeline(vk::Device device, vk::detail::DispatchLoaderDynamic dldi, vk::ShaderModule module, vk::PipelineLayout layout) {
    vk::PipelineShaderStageCreateInfo stage_info(
      vk::PipelineShaderStageCreateFlags(),
      vk::ShaderStageFlagBits::eCompute,
      module, "main");
    vk::ComputePipelineCreateInfo pipeline_info(vk::PipelineCreateFlags(), stage_info, layout);
    std::vector pipeline_infos = {pipeline_info};
    return phisi_app::checkVkResult(device.createComputePipelines(VK_NULL_HANDLE, pipeline_infos, nullptr, dldi)).front();
  }
  


  void GpuFluidScreen::allocate() {
    m_width = m_texture->m_width + 2;
    m_height = m_texture->m_height + 2;
    m_push_constant.width = m_width;
    m_push_constant.height = m_height;    
    
    vk::FenceCreateInfo fence_info(vk::FenceCreateFlagBits::eSignaled);
    m_fence = m_context->m_device.createFence(fence_info, nullptr, m_context->m_dldi);
    
    
    vk::CommandPoolCreateInfo cmd_pool_info(vk::CommandPoolCreateFlagBits::eTransient, m_context->m_compute_queue_family_index);
    m_cmd_pool = m_context->m_device.createCommandPool(cmd_pool_info, nullptr, m_context->m_dldi);

    m_memory.allocate(m_width * m_height * 4 * 12, 
      vk::MemoryPropertyFlagBits::eDeviceLocal, 
      vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst);  
    m_module_force = create_module(m_context->m_device, m_context->m_dldi, "shaders/force.spv");
    m_module_velocity = create_module(m_context->m_device, m_context->m_dldi, "shaders/velocity.spv");
    m_module_divergence = create_module(m_context->m_device, m_context->m_dldi, "shaders/divergence.spv");
    m_module_color = create_module(m_context->m_device, m_context->m_dldi, "shaders/color.spv");
         
    vk::DescriptorSetLayoutBinding bindings[2];
    bindings[0] = vk::DescriptorSetLayoutBinding(
      0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute);
    bindings[1] = vk::DescriptorSetLayoutBinding(
      1, vk::DescriptorType::eStorageImage, 1, vk::ShaderStageFlagBits::eCompute);

    vk::DescriptorSetLayoutCreateInfo descriptor_layout_info(
      vk::DescriptorSetLayoutCreateFlags(), 2, bindings);
    m_descriptor_layout = m_context->m_device.createDescriptorSetLayout(descriptor_layout_info, nullptr, m_context->m_dldi);
    vk::PushConstantRange range = vk::PushConstantRange(vk::ShaderStageFlagBits::eCompute, 0, 128);
    vk::PipelineLayoutCreateInfo layout_info(vk::PipelineLayoutCreateFlags(), 1, &m_descriptor_layout, 1, &range);
    m_pipeline_layout = m_context->m_device.createPipelineLayout(layout_info, nullptr, m_context->m_dldi);
    
    m_pipeline_force = create_pipeline(m_context->m_device, m_context->m_dldi, m_module_force, m_pipeline_layout);
    m_pipeline_velocity = create_pipeline(m_context->m_device, m_context->m_dldi, m_module_velocity, m_pipeline_layout);
    m_pipeline_divergence = create_pipeline(m_context->m_device, m_context->m_dldi, m_module_divergence, m_pipeline_layout);
    m_pipeline_color = create_pipeline(m_context->m_device, m_context->m_dldi, m_module_color, m_pipeline_layout);
    
    vk::DescriptorSetAllocateInfo alc_info(m_context->m_descriptor_pool, 1, &m_descriptor_layout);
    m_descriptor_set = m_context->m_device.allocateDescriptorSets(alc_info, m_context->m_dldi).front();
  
    vk::DescriptorBufferInfo info_gpu_data(m_memory.m_buffer, 0, vk::WholeSize);    
    vk::DescriptorImageInfo info_image(m_texture->m_sampler, m_texture->m_image_view, vk::ImageLayout::eGeneral);
    
    vk::WriteDescriptorSet write_gpu_data(
      m_descriptor_set, 
      0, 
      0,
      1, 
      vk::DescriptorType::eStorageBuffer, 
      nullptr,
      &info_gpu_data);
    
    vk::WriteDescriptorSet write_image(
      m_descriptor_set, 
      1, 
      0,
      1, 
      vk::DescriptorType::eStorageImage, 
      &info_image);
    vk::WriteDescriptorSet descriptor_writes[2] = {write_gpu_data, write_image};
    m_context->m_device.updateDescriptorSets(2, descriptor_writes, 0, nullptr, m_context->m_dldi);               
    LOG_TRACE("Fluid screen has been allocated."); 
  }



  void GpuFluidScreen::initBuffer() {
    m_memory.allocateStaging();    
    m_memory.map();
        
    memset(m_memory.m_mapped_memory, 0, m_memory.m_size);
    for(uint32_t y = 1; y < m_height - 1; y++) { for(uint32_t x = 1; x < m_width - 1; x++) {
      static_cast<float*>(m_memory.m_mapped_memory)[y * m_width + x] = 1.0; 
    }}

    vk::CommandBufferAllocateInfo cmd_alc_info(
      m_cmd_pool, 
      vk::CommandBufferLevel::ePrimary, 
      1);
    vk::CommandBuffer cmd_buffer;
    phisi_app::checkVkResult(m_context->m_device.allocateCommandBuffers(&cmd_alc_info, &cmd_buffer));
    vk::CommandBufferBeginInfo begin_info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    cmd_buffer.begin(begin_info, m_context->m_dldi);

    m_memory.uploadStaging(cmd_buffer);
    
    vk::ImageSubresourceRange subresource_range(
      vk::ImageAspectFlagBits::eColor, 
      0, 1, 0, 1);
    vk::ImageMemoryBarrier src_barrier( 
      {},
      vk::AccessFlagBits::eShaderRead, 
      vk::ImageLayout::eUndefined, 
      vk::ImageLayout::eShaderReadOnlyOptimal,
      vk::QueueFamilyIgnored,
      vk::QueueFamilyIgnored,
      m_texture->m_image,
      subresource_range,
      nullptr);
    cmd_buffer.pipelineBarrier(
      vk::PipelineStageFlagBits::eTopOfPipe, 
      vk::PipelineStageFlagBits::eComputeShader, 
      {}, 
      0, nullptr, 
      0, nullptr, 
      1, &src_barrier, m_context->m_dldi);

    cmd_buffer.end(m_context->m_dldi);

    vk::SubmitInfo submit_info(0, nullptr, nullptr, 1, &cmd_buffer);
    m_context->m_compute_queue.submit(submit_info);
    m_context->m_compute_queue.waitIdle(m_context->m_dldi);
    m_context->m_device.freeCommandBuffers(m_cmd_pool, 1, &cmd_buffer, m_context->m_dldi);
    
    m_memory.unmap();
    m_memory.destoryStaging();
    LOG_TRACE("Fluid buffer has been initialized.");
  }


  
  void GpuFluidScreen::simulate() {
    if(m_run_simulation == false)
      return;

    checkVkResult(m_context->m_device.waitForFences(1, &m_fence, true, UINT64_MAX, m_context->m_dldi));
    checkVkResult(m_context->m_device.resetFences(1, &m_fence, m_context->m_dldi));
    m_context->m_device.resetCommandPool(m_cmd_pool);

    
    
    vk::CommandBufferAllocateInfo alc_info(m_cmd_pool, vk::CommandBufferLevel::ePrimary, 1);
    vk::CommandBuffer cmd_buffer = m_context->m_device.allocateCommandBuffers(alc_info, m_context->m_dldi).front();
    
    vk::CommandBufferBeginInfo begin_info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    cmd_buffer.begin(begin_info, m_context->m_dldi);
    
    compute(cmd_buffer);
    
    cmd_buffer.end(m_context->m_dldi);
    
    vk::PipelineStageFlags stage = vk::PipelineStageFlagBits::eComputeShader;
    vk::SubmitInfo submit_info(0, nullptr, &stage, 1, &cmd_buffer);
    checkVkResult(m_context->m_compute_queue.submit(1, &submit_info, m_fence, m_context->m_dldi));
  }



  void GpuFluidScreen::compute(vk::CommandBuffer cmd_buffer) {     
    m_push_constant.buffer_state = !m_push_constant.buffer_state;
    m_push_constant.divergence_state = m_push_constant.buffer_state; 
    m_push_constant.frame_time = m_context->m_frame_time;     
    m_push_constant.pressure_constant = (m_push_constant.density * m_push_constant.grid_spacing) / m_context->m_frame_time;

    cmd_buffer.pushConstants(m_pipeline_layout, vk::ShaderStageFlagBits::eCompute, 0, sizeof(PushConstantData), &m_push_constant);

    vk::MemoryBarrier barrier(
      vk::AccessFlagBits::eMemoryWrite, 
      vk::AccessFlagBits::eMemoryWrite | vk::AccessFlagBits::eMemoryRead
    );
    
    cmd_buffer.bindDescriptorSets(
      vk::PipelineBindPoint::eCompute,
      m_pipeline_layout, 0, 
      m_descriptor_set, nullptr, m_context->m_dldi);   

    //apply force
    cmd_buffer.bindPipeline(vk::PipelineBindPoint::eCompute, m_pipeline_force, m_context->m_dldi);        
    cmd_buffer.dispatch((m_width + 15.0) / 16.0, (m_height + 15.0) / 16.0, 1, m_context->m_dldi);
    
    cmd_buffer.pipelineBarrier(
      vk::PipelineStageFlagBits::eComputeShader, 
      vk::PipelineStageFlagBits::eComputeShader,
      vk::DependencyFlags(), 1, &barrier,
      0, nullptr,
      0, nullptr, m_context->m_dldi);


    //advect velocity
    cmd_buffer.bindPipeline(vk::PipelineBindPoint::eCompute, m_pipeline_velocity, m_context->m_dldi);    
    cmd_buffer.dispatch((m_width + 15.0) / 16.0, (m_height + 15.0) / 16.0, 1, m_context->m_dldi);
    
    cmd_buffer.fillBuffer(m_memory.m_buffer, m_width * m_height * 5 * 4, m_width * m_height * 4, 0);
    cmd_buffer.bindPipeline(vk::PipelineBindPoint::eCompute, m_pipeline_divergence, m_context->m_dldi);
    for(uint32_t i = 0; i < m_div_iters; i++) {         
      cmd_buffer.pipelineBarrier(
      vk::PipelineStageFlagBits::eComputeShader, 
      vk::PipelineStageFlagBits::eComputeShader,
      vk::DependencyFlags(), 1, &barrier,
      0, nullptr,
      0, nullptr, m_context->m_dldi);
      
      //clear divergence
      cmd_buffer.dispatch(((m_width + 1) / 2 + 15) / 16, (m_height + 15) / 16, 1, m_context->m_dldi);
      m_push_constant.divergence_state = !m_push_constant.divergence_state;
      cmd_buffer.pushConstants(m_pipeline_layout, vk::ShaderStageFlagBits::eCompute, 0, sizeof(m_push_constant), &m_push_constant);
    }
    
    cmd_buffer.pipelineBarrier(
      vk::PipelineStageFlagBits::eComputeShader, 
      vk::PipelineStageFlagBits::eComputeShader,
      vk::DependencyFlags(), 1, &barrier,
      0, nullptr,
      0, nullptr, m_context->m_dldi);    


    vk::ImageSubresourceRange subresource_range(
        vk::ImageAspectFlagBits::eColor, 
        0, 1, 0, 1);
    vk::ImageMemoryBarrier src_barrier(
        vk::AccessFlagBits::eShaderRead, 
        vk::AccessFlagBits::eShaderWrite, 
        vk::ImageLayout::eShaderReadOnlyOptimal, 
        vk::ImageLayout::eGeneral,
        vk::QueueFamilyIgnored,
        vk::QueueFamilyIgnored,
        m_texture->m_image,
        subresource_range,
        nullptr);
    cmd_buffer.pipelineBarrier(
      vk::PipelineStageFlagBits::eComputeShader, 
      vk::PipelineStageFlagBits::eComputeShader, 
      {}, 
      0, nullptr, 
      0, nullptr, 
      1, &src_barrier, m_context->m_dldi);     

    //advect color
    cmd_buffer.bindPipeline(vk::PipelineBindPoint::eCompute, m_pipeline_color, m_context->m_dldi);    
    cmd_buffer.dispatch((m_width + 15.0) / 16.0, (m_height + 15.0) / 16.0, 1);
    
    vk::ImageMemoryBarrier dest_barrier(
        vk::AccessFlagBits::eShaderWrite, 
        vk::AccessFlagBits::eShaderRead, 
        vk::ImageLayout::eGeneral, 
        vk::ImageLayout::eShaderReadOnlyOptimal,
        vk::QueueFamilyIgnored,
        vk::QueueFamilyIgnored,
        m_texture->m_image,
        subresource_range,
        nullptr);
      cmd_buffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eComputeShader, 
        vk::PipelineStageFlagBits::eComputeShader, 
        {}, 
        0, nullptr, 
        0, nullptr, 
        1, &dest_barrier, 
      m_context->m_dldi);    
  }



  void GpuFluidScreen::destroy() {
    m_context->m_device.waitIdle(m_context->m_dldi);
    m_context->m_device.destroyPipeline(m_pipeline_force);
    m_context->m_device.destroyPipeline(m_pipeline_velocity);
    m_context->m_device.destroyPipeline(m_pipeline_divergence);
    m_context->m_device.destroyPipeline(m_pipeline_color);
    m_context->m_device.destroyPipelineLayout(m_pipeline_layout);
    m_context->m_device.destroyDescriptorSetLayout(m_descriptor_layout);
    m_context->m_device.destroyShaderModule(m_module_force);
    m_context->m_device.destroyShaderModule(m_module_velocity);
    m_context->m_device.destroyShaderModule(m_module_divergence);
    m_context->m_device.destroyShaderModule(m_module_color);
    m_memory.destroy();
    m_context->m_device.destroyCommandPool(m_cmd_pool);
    m_context->m_device.destroyFence(m_fence);
  }
}
