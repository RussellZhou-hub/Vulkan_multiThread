#pragma once
#include "../../config.h"
#include "../vkUtil/queue_families.h"
#include "../vkUtil/frame.h"
#include "../logging.h"

namespace vkInit {

	/**
		Data structures used in creating command buffers
	*/
	struct commandBufferInputChunk {
		vk::Device device; 
		vk::CommandPool commandPool;
		std::vector<vkUtil::SwapChainFrame>& frames;
	};

	/**
		Make a command pool.
		\param device the logical device
		\param physicalDevice the physical device
		\param the windows surface (used for getting the queue families)
		\param debug whether the system is running in debug mode
		\returns the created command pool
	*/
	vk::CommandPool make_command_pool(
		vk::Device device, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface);
	
	/**
		Make a command buffer for each swapchain frame and return a main command buffer.
		\param inputChunk the required input info
		\param debug whether the system is running in debug mode
		\returns the main command buffer
	*/
	vk::CommandBuffer make_command_buffer(commandBufferInputChunk inputChunk);

	void make_frame_command_buffers(commandBufferInputChunk inputChunk);
}