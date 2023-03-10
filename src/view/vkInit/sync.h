#pragma once
#include "../../config.h"
#include "../logging.h"

namespace vkInit {

	/**
		Make a semaphore.
		\param device the logical device
		\param debug whether the system is running in debug mode
		\returns the created semaphore
	*/
	vk::Semaphore make_semaphore(vk::Device device);

	/**
		Make a fence.
		\param device the logical device
		\param debug whether the system is running in debug mode
		\returns the created fence
	*/
	vk::Fence make_fence(vk::Device device);
}