#include "phisi_app.hpp"
#include "phisi_vulkan.hpp"
#include <logger/logger.hpp>


int main() {    
    LOG_ERROR("test");
    
    phisi_app::VulkanContext context;  
    return context.init();
}