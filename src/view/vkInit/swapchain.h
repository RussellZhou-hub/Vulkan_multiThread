#pragma once
#include "../../config.h"
#include "../logging.h"
#include"../vkUtil/queue_families.h"
#include"../vkUtil/frame.h"
#include "../vkImage/image.h"

namespace vkInit {
    struct SwapChainSupportDetails{
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};

    

	/**
		Various data structures associated with the swapchain.
	*/
	struct SwapChainBundle {
		vk::SwapchainKHR swapchain;
		std::vector<vkUtil::SwapChainFrame> frames;
		vk::Format format;
		vk::Extent2D extent;
	};

    SwapChainSupportDetails query_swapchain_support(vk::PhysicalDevice device,vk::SurfaceKHR surface);

	vk::SurfaceFormatKHR choose_swapchain_surface_format(std::vector<vk::SurfaceFormatKHR> formats);

	vk::PresentModeKHR choose_swapchain_present_mode(std::vector<vk::PresentModeKHR> presentModes);

	vk::Extent2D choose_swapchain_extent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR capabilities);

	SwapChainBundle create_swapchain(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, int width, int height);
}