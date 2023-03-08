#include "single_time_commands.h"

void vkUtil::startJob(vk::CommandBuffer commandBuffer) {

#ifdef VK_MAKE_VERSION
	VULKAN_HPP_NAMESPACE::CommandBufferResetFlags flags={};
	VULKAN_HPP_NAMESPACE::DispatchLoaderStatic d VULKAN_HPP_DEFAULT_DISPATCHER_ASSIGNMENT ;
	commandBuffer.reset(flags,d);
#else 
	commandBuffer.reset();
#endif

	vk::CommandBufferBeginInfo beginInfo;
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
	commandBuffer.begin(beginInfo);
}

void vkUtil::endJob(vk::CommandBuffer commandBuffer, vk::Queue submissionQueue) {

	commandBuffer.end();

	vk::SubmitInfo submitInfo;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	submissionQueue.submit(1, &submitInfo, nullptr);
	submissionQueue.waitIdle();
}