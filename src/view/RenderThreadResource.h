#pragma once
#include "../config.h"
#include "logging.h"
#include "vkInit/instance.h"
#include "vkInit/device.h"
#include "vkUtil/frame.h"

// every tread use its own RenderThreadResource object
class RenderThreadResource
{
public:

    RenderThreadResource();
    RenderThreadResource(vk::Instance instance,vk::SurfaceKHR surface);
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

    //descriptor-related variables
	vk::DescriptorSetLayout frameSetLayout;
	vk::DescriptorPool frameDescriptorPool; //Descriptors bound on a "per frame" basis
	vk::DescriptorSetLayout meshSetLayout;
	vk::DescriptorPool meshDescriptorPool; //Descriptors bound on a "per mesh" basis

    vk::SwapchainKHR swapchain{ nullptr };
	std::vector<vkUtil::SwapChainFrame> swapchainFrames;
	vk::Format swapchainFormat;
	vk::Extent2D swapchainExtent;

    //pipeline-related variables
	vk::PipelineLayout pipelineLayout;
	vk::RenderPass renderpass;
	vk::Pipeline pipeline;

	//Command-related variables
	vk::CommandPool commandPool;
	vk::CommandBuffer mainCommandBuffer;

	//Synchronization objects
	int maxFramesInFlight,frameNumber;
	std::atomic<int> frameNumber_atomic; //for multiThread rendering
	std::atomic<int> frameTime_atomic; //for multiThread rendering

    void create_device();
};