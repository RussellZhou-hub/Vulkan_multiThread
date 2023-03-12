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

RenderThreadResource::RenderThreadResource(const RenderThreadResource& res){
    //device-related variable
	physicalDevice = res.physicalDevice;
	device = res.device;
    
    graphicsQueue = res.graphicsQueue;
	presentQueue = res.presentQueue;

    //descriptor-related variables
	frameSetLayout = res.frameSetLayout;
	frameDescriptorPool = res.frameDescriptorPool;
	meshSetLayout = res.meshSetLayout;
	meshDescriptorPool = res.meshDescriptorPool;

    swapchain = res.swapchain;
	swapchainFrames = res.swapchainFrames;
	swapchainFormat = res.swapchainFormat;
	swapchainExtent = res.swapchainExtent;

    //pipeline-related variables
	pipelineLayout = res.pipelineLayout;
	renderpass = res.renderpass;
	pipeline = res.pipeline;

	//Command-related variables
	commandPool=res.commandPool;
	threadCommandBuffer=res.threadCommandBuffer;

	// vertex buffer
	vertices=res.vertices;
	indices=res.indices;
	vertexBuffer=res.vertexBuffer;
	indexBuffer=res.indexBuffer;

	//Synchronization objects
	maxFramesInFlight=res.maxFramesInFlight;
    frameNumber=res.frameNumber;

	_threadDeletionQueue=res._threadDeletionQueue;
}

void RenderThreadResource::create_device(){
    physicalDevice = vkInit::choose_physical_device(instance);
	device = vkInit::create_logical_device(physicalDevice, surface);
	std::array<vk::Queue,2> queues = vkInit::get_queues(physicalDevice, device, surface);
	graphicsQueue = queues[0];
	presentQueue = queues[1];
}
