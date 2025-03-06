#include "phisi_vulkan.hpp"
#include "imgui_impl_glfw.h"
#include "logger.hpp"
#include "phisi_util.hpp"
#include <cstdint>
#include <vector>


namespace phisi_app {
    
  void VulkanContext::createVkInstance() {
    vk::ApplicationInfo app_info(
      "Phisi App", VK_MAKE_VERSION(1, 0, 0), 
      "None", VK_MAKE_VERSION(1, 0, 0), 
      VK_API_VERSION_1_4);       
    
    std::vector<const char*> layers;
    
    uint32_t ext_count = 0;
    const char** exts = glfwGetRequiredInstanceExtensions(&ext_count);
    std::vector<const char*> extensions = std::vector(exts, exts + ext_count);
        
    #ifdef BUILD_DEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);   
    layers.push_back("VK_LAYER_KHRONOS_validation");
    #endif
    
    vk::InstanceCreateInfo create_info(
      vk::InstanceCreateFlags(), &app_info, 
      layers.size(), layers.data(), 
      extensions.size(), extensions.data());
    m_instance = vk::createInstanceUnique(create_info);
    LOG_TRACE("VkInstance created.");
    vk::detail::DispatchLoaderDynamic dldi(m_instance.get(), vkGetInstanceProcAddr);
    m_dldi = dldi;
    LOG_TRACE("DispatchLoaderDynamic created.");
  } 
  

  #ifdef BUILD_DEBUG
  VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallbackFunc(
    vk::DebugUtilsMessageSeverityFlagBitsEXT msg_severity, 
    vk::DebugUtilsMessageTypeFlagsEXT msg_type, 
    const vk::DebugUtilsMessengerCallbackDataEXT* callback_data, 
    void* user_data) {
    if (msg_severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
      LOG_ERROR(callback_data->pMessage);
    if (msg_severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
      LOG_WARN(callback_data->pMessage);
    if (msg_severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo)
      LOG_INFO(callback_data->pMessage);    
    if (msg_severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose)
      LOG_TRACE(callback_data->pMessage);
    (void)user_data;
    (void)msg_type;
    return false;
  }
   
  void VulkanContext::createDebugMessenger() {    
    vk::DebugUtilsMessengerCreateInfoEXT create_info(
      vk::DebugUtilsMessengerCreateFlagsEXT(), 
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | 
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eError, 
      vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
      vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
      vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
      debugCallbackFunc);
    m_debug_messenger = m_instance.get().createDebugUtilsMessengerEXTUnique(create_info, nullptr, m_dldi);
    LOG_TRACE("VkDebugUtilsMessenger created and set up.");
  }
  #endif

  void VulkanContext::createWindow() {
      m_window.setInstance(m_instance.get());   
      m_window.create(c_window_title);
  }
  
  void VulkanContext::chosePhysicalDevice() {
    vk::PhysicalDevice device = m_instance.get().enumeratePhysicalDevices(m_dldi).front();
    vk::PhysicalDeviceProperties properties = device.getProperties();
    LOG_INFO("Chose device " << properties.deviceName << " as VkPhysicalDevice.");
    m_device_physical = std::move(device);
  }


  void VulkanContext::createLogicalDevice() {
    std::vector properties = m_device_physical.getQueueFamilyProperties(m_dldi);
    uint32_t queue_family_index = 0;
    for (size_t i = 0; i < properties.size(); i++) {
      if(properties[i].queueCount > 0 && properties[i].queueFlags & vk::QueueFlagBits::eGraphics 
        && properties[i].queueFlags & vk::QueueFlagBits::eTransfer) {
        queue_family_index = i;
      }
    }   
    float queue_priority = 1.0f;
    vk::DeviceQueueCreateInfo queue_info(vk::DeviceQueueCreateFlags(), queue_family_index, 1, &queue_priority);
    
    std::vector<const char*> extensions; 
    extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    
    vk::DeviceCreateInfo create_info(vk::DeviceCreateFlags(), queue_info);
    create_info.setPEnabledExtensionNames(extensions);
    
    m_device = m_device_physical.createDeviceUnique(create_info);  
    m_queue_family_index = queue_family_index;
    LOG_TRACE("VkDevice has been created and queue fmily " << queue_family_index << " has been selected.");
  }

  void VulkanContext::createQueue() {
    m_device.get().getQueue(m_queue_family_index, 0, &m_queue, m_dldi);
    LOG_TRACE("Selected queue 0.");
  }

  void VulkanContext::createDescriptorPool() {
    vk::DescriptorPoolSize size(vk::DescriptorType::eCombinedImageSampler, 1);          
    vk::DescriptorPoolCreateInfo create_info(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 1, 1, &size);
    m_descriptor_pool = m_device.get().createDescriptorPoolUnique(create_info);
    LOG_TRACE("Created VkDescriptorPool.");
  }

  void VulkanContext::setupVulkanWindow() {
    m_window_data.Surface = m_window.m_surface;
    if (!m_device_physical.getSurfaceSupportKHR(m_queue_family_index, m_window_data.Surface, m_dldi)) {
      LOG_ERROR("Window surface has no Vulkan support");
      exit(-1);
    }

    const VkFormat requestSurfaceImageFormat[] = { 
      VK_FORMAT_B8G8R8A8_UNORM, 
      VK_FORMAT_R8G8B8A8_UNORM, 
      VK_FORMAT_B8G8R8_UNORM, 
      VK_FORMAT_R8G8B8_UNORM 
    };

    const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    m_window_data.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(
      m_device_physical, 
      m_window_data.Surface, 
      requestSurfaceImageFormat, 
      (size_t) IM_ARRAYSIZE(requestSurfaceImageFormat), 
      requestSurfaceColorSpace);

    VkPresentModeKHR present_modes[] = { 
      VK_PRESENT_MODE_MAILBOX_KHR, 
      VK_PRESENT_MODE_IMMEDIATE_KHR, 
      VK_PRESENT_MODE_FIFO_KHR 
    };    
    m_window_data.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(
      m_device_physical, 
      m_window_data.Surface, 
      &present_modes[0], 
      IM_ARRAYSIZE(present_modes));

    static_assert(c_min_image_count >= 2, "Min image count must at least be 2.");

    std::pair dimensions = m_window.getFrameBufferSize();
    ImGui_ImplVulkanH_CreateOrResizeWindow(
      m_instance.get(), 
      m_device_physical, 
      m_device.get(), 
      &m_window_data, 
      m_queue_family_index, 
      nullptr, 
      dimensions.first, 
      dimensions.second, 
      c_min_image_count);        
    LOG_TRACE("Vulkan window has been setup.");
  }  

  void VulkanContext::setupImGUI() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(m_window.m_window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = m_instance.get();
    init_info.PhysicalDevice = m_device_physical;
    init_info.Device = m_device.get();
    init_info.QueueFamily = m_queue_family_index;
    init_info.Queue = m_queue;
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = m_descriptor_pool.get();
    init_info.RenderPass = m_window_data.RenderPass;
    init_info.Subpass = 0;
    init_info.MinImageCount = c_min_image_count;
    init_info.ImageCount = m_window_data.ImageCount;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = nullptr;
    init_info.CheckVkResultFn = checkVkResult;
    ImGui_ImplVulkan_Init(&init_info);  
    LOG_TRACE("ImGUI has been setup.");
  }


  void VulkanContext::renderFrame(ImDrawData* draw_data) {    
    vk::Semaphore image_acquired_semaphore  = m_window_data.FrameSemaphores[m_window_data.SemaphoreIndex].ImageAcquiredSemaphore;
    vk::Semaphore render_complete_semaphore = m_window_data.FrameSemaphores[m_window_data.SemaphoreIndex].RenderCompleteSemaphore;

    vk::ResultValue<uint32_t> value = m_device.get().acquireNextImageKHR(m_window_data.Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, m_dldi);
    if (value.result == vk::Result::eErrorOutOfDateKHR || value.result == vk::Result::eSuboptimalKHR) {
        m_swapchain_rebuild = true;
        return;
    } else if (value.result != vk::Result::eSuccess) {
      LOG_ERROR("Aquireing the next image has caused an error.");
    }

    ImGui_ImplVulkanH_Frame frame_data = m_window_data.Frames[m_window_data.FrameIndex];
    vk::CommandBuffer com_buffer = vk::CommandBuffer(frame_data.CommandBuffer);
    
    vk::Fence fence = vk::Fence(frame_data.Fence);
    checkVkResult(m_device.get().waitForFences({ fence }, true, UINT64_MAX, m_dldi));
    m_device.get().resetFences({ fence }, m_dldi);
    
    m_device.get().resetCommandPool(frame_data.CommandPool); 
    vk::CommandBufferBeginInfo begin_info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    com_buffer.begin(begin_info, m_dldi);
    
    vk::RenderPassBeginInfo render_begin_info(
      m_window_data.RenderPass, 
      frame_data.Framebuffer, 
      vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(m_window_data.Width, m_window_data.Height)),
      1, &c_background_color);
    com_buffer.beginRenderPass(render_begin_info, vk::SubpassContents::eInline, m_dldi);

    // Record dear imgui primitives into command buffer
    ImGui_ImplVulkan_RenderDrawData(draw_data, frame_data.CommandBuffer);

    // Submit command buffer
    com_buffer.endRenderPass(m_dldi);
    vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::SubmitInfo submit_info(1, &image_acquired_semaphore, &wait_stage, 1, &com_buffer, 1, &render_complete_semaphore);
    com_buffer.end(m_dldi);
    m_queue.submit({ submit_info }, fence);
  }

  void VulkanContext::presentFrame() {
    if (m_swapchain_rebuild)
        return;
    vk::Semaphore render_complete_semaphore = m_window_data.FrameSemaphores[m_window_data.SemaphoreIndex].RenderCompleteSemaphore;
    vk::SwapchainKHR swapchain = m_window_data.Swapchain;
    vk::PresentInfoKHR present_info(1, &render_complete_semaphore, 1, &swapchain, &m_window_data.FrameIndex);
    vk::Result result = m_queue.presentKHR(&present_info);
    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
      m_swapchain_rebuild = true;
      return;
    }
    checkVkResult(result);
    m_window_data.SemaphoreIndex = (m_window_data.SemaphoreIndex + 1) % m_window_data.SemaphoreCount;
    m_window_data.FrameIndex = (m_window_data.FrameIndex + 1) % m_window_data.ImageCount;
  }
  

  void VulkanContext::init() {
    Window::initGlfw();
    try {
      createVkInstance();
      #ifdef BUILD_DEBUG 
      createDebugMessenger();
      #endif
      createWindow();
      chosePhysicalDevice();
      createLogicalDevice();
      createQueue();
      createDescriptorPool();
      setupVulkanWindow();
      setupImGUI();
    } catch(const vk::Error& err) {
      LOG_ERROR(err.what());
    } catch(...) {
      LOG_ERROR("Unknown error was thrown by Vulkan.");
    }
  }  

  bool VulkanContext::newFrame() {
    std::pair fb_size = m_window.getFrameBufferSize();
    if (fb_size.first > 0 && fb_size.second > 0 && (m_swapchain_rebuild || m_window_data.Width != fb_size.first || m_window_data.Height != fb_size.second)) {
      ImGui_ImplVulkan_SetMinImageCount(c_min_image_count);
      ImGui_ImplVulkanH_CreateOrResizeWindow(m_instance.get(), m_device_physical, m_device.get(), &m_window_data, m_queue_family_index, nullptr, fb_size.first, fb_size.second, c_min_image_count);
      m_window_data.FrameIndex = 0;
      m_swapchain_rebuild = false;
    }
    if (m_window.minimized()) {
      ImGui_ImplGlfw_Sleep(10);
      return false;
    }

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    return true;
  }

  void VulkanContext::render() {
    ImGui::Render();
    ImDrawData* main_draw_data = ImGui::GetDrawData();
    const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
    if (!main_is_minimized)
      renderFrame(main_draw_data);


    // Update and Render additional Platform Windows
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    // Present Main Platform Window
    if (!main_is_minimized)
      presentFrame();
  }

  VulkanContext::~VulkanContext() {
    m_device.get().waitIdle();
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplVulkanH_DestroyWindow(m_instance.get(), m_device.get(), &m_window_data, nullptr);
  }
}