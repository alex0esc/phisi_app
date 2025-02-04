#include "phisi_vulkan.hpp"

int main() {        
    phisi_app::VulkanContext context;  
    context.init();    
    context.render();
}