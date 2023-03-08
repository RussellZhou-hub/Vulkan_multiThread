#pragma once
#include"../../config.h"
#include"../logging.h"
#include"../vkUtil/queue_families.h"

/*
* Vulkan separates the concept of physical and logical devices. 
* 
  A physical device usually represents a single complete implementation of Vulkan 
  (excluding instance-level functionality) available to the host, 
  of which there are a finite number. 
  
  A logical device represents an instance of that implementation 
  with its own state and resources independent of other logical devices.
*/

namespace vkInit
{
    vk::PhysicalDevice choose_physical_device( vk::Instance& instance);
    bool isSuitable(const vk::PhysicalDevice& device);
    bool checkDeviceExtensionSupport(
		const vk::PhysicalDevice& device,
		const std::vector<const char*>& requestedExtensions
	);
    vk::Device create_logical_device(vk::PhysicalDevice physicalDevice,vk::SurfaceKHR surface);
	std::array<vk::Queue,2> get_queues(vk::PhysicalDevice physicalDevice, vk::Device device, vk::SurfaceKHR surface);
}