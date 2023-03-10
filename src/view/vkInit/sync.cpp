#include "sync.h"

using namespace vkInit;

vk::Semaphore vkInit::make_semaphore(vk::Device device) {

		vk::SemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.flags = vk::SemaphoreCreateFlags();

		try {
			return device.createSemaphore(semaphoreInfo);
		}
		catch (vk::SystemError err) {
			#ifdef DEBUG_MESSAGE
				std::cout << "Failed to create semaphore " << std::endl;
			#endif
		}
}

vk::Fence vkInit::make_fence(vk::Device device) {

		vk::FenceCreateInfo fenceInfo = {};
		fenceInfo.flags = vk::FenceCreateFlags() | vk::FenceCreateFlagBits::eSignaled;

		try {
			return device.createFence(fenceInfo);
		}
		catch (vk::SystemError err) {
			#ifdef DEBUG_MESSAGE
				std::cout << "Failed to create fence " << std::endl;
			#endif
		}
}