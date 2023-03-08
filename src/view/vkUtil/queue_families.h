#pragma once
#include "../../config.h"
#include"../logging.h"

namespace vkUtil{
    class QueueFamilyIndices{
		public:
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete();
	};
    
    QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device,vk::SurfaceKHR surface);
}