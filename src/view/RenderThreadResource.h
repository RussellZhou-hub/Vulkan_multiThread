#pragma once
#include "../config.h"
#include "logging.h"
#include "vkInit/instance.h"
#include "vkInit/device.h"

// every tread use its own RenderThreadResource object
class RenderThreadResource
{
public:

    RenderThreadResource();
    RenderThreadResource(vk::Instance instance,vk::SurfaceKHR surface);
    VkCommandBuffer mainCommandBuffer;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore finishedRenderingSemaphore;
    VkFence fence;
    std::vector<VkCommandPool> drawingCommandPools;
    std::vector<VkCommandBuffer> commandBuffersForDrawing;

    vk::Instance instance;
    vk::SurfaceKHR surface;
    
    //device-related variable
	vk::PhysicalDevice physicalDevice{nullptr};
	vk::Device device{ nullptr };
    
    vk::Queue graphicsQueue{ nullptr };
	vk::Queue presentQueue{ nullptr };

    void create_device();
};