#include "RenderThreadResource.h"

RenderThreadResource::RenderThreadResource(){
            threadCommandBuffer=nullptr;
            imageAvailableSemaphore=nullptr;
            finishedRenderingSemaphore=nullptr;
            fence=nullptr;
}

RenderThreadResource::RenderThreadResource(vk::Instance instance,vk::SurfaceKHR surface){
    this->instance=instance;
    this->surface=surface;
}

void RenderThreadResource::create_device(){
    physicalDevice = vkInit::choose_physical_device(instance);
	device = vkInit::create_logical_device(physicalDevice, surface);
	std::array<vk::Queue,2> queues = vkInit::get_queues(physicalDevice, device, surface);
	graphicsQueue = queues[0];
	presentQueue = queues[1];
}
