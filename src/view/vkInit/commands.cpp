#include "commands.h"

using namespace vkInit;

vk::CommandPool vkInit::make_command_pool(
		vk::Device device, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) {

		vkUtil::QueueFamilyIndices queueFamilyIndices = vkUtil::findQueueFamilies(physicalDevice, surface);

		vk::CommandPoolCreateInfo poolInfo;
		poolInfo.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		try {
			return device.createCommandPool(poolInfo);
		}
		catch (vk::SystemError err) {

			#ifdef DEBUG_MESSAGE
				std::cout << "Failed to create Command Pool" << std::endl;
			#endif

			return nullptr;
		}
}

vk::CommandBuffer vkInit::make_command_buffer(commandBufferInputChunk inputChunk) {

		vk::CommandBufferAllocateInfo allocInfo = {};
		allocInfo.commandPool = inputChunk.commandPool;
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandBufferCount = 1;
		

		//Make a "main" command buffer for the engine
		try {
			vk::CommandBuffer commandBuffer = inputChunk.device.allocateCommandBuffers(allocInfo)[0];

			#ifdef DEBUG_MESSAGE
				std::cout << "Allocated main command buffer " << std::endl;
			#endif

			return commandBuffer;
		}
		catch (vk::SystemError err) {

			#ifdef DEBUG_MESSAGE
				std::cout << "Failed to allocate main command buffer " << std::endl;
			#endif

			return nullptr;
		}
}

void vkInit::make_frame_command_buffers(commandBufferInputChunk inputChunk){

		vk::CommandBufferAllocateInfo allocInfo = {};
		allocInfo.commandPool = inputChunk.commandPool;
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandBufferCount = 1;
		
		//Make a command buffer for each frame
		for (int i = 0; i < inputChunk.frames.size(); ++i) {
			try {
				inputChunk.frames[i].commandBuffer = inputChunk.device.allocateCommandBuffers(allocInfo)[0];
				
				#ifdef DEBUG_MESSAGE
					std::cout << "Allocated command buffer for frame " << i << std::endl;
				#endif
			}
			catch (vk::SystemError err) {

				#ifdef DEBUG_MESSAGE
					std::cout << "Failed to allocate command buffer for frame " << i << std::endl;
				#endif
			}
		}
}