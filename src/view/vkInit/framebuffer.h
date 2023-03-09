#pragma once
#include "../../config.h"
#include"../logging.h"
#include"../vkUtil/frame.h"

namespace vkInit{
    /**
		Data structures involved in making framebuffers for the
		swapchain.
	*/
	struct framebufferInput {
		vk::Device device;
		vk::RenderPass renderpass;
		vk::Extent2D swapchainExtent;
	};

	/**
		Make framebuffers for the swapchain
		\param inputChunk required input for creation
		\param frames the vector to be populated with the created framebuffers
		\param debug whether the system is running in debug mode.
	*/
	void make_framebuffers(framebufferInput inputChunk, std::vector<vkUtil::SwapChainFrame>& frames);
}