#include"framebuffer.h"

using namespace vkInit;

void vkInit::make_framebuffers(framebufferInput inputChunk, std::vector<vkUtil::SwapChainFrame>& frames) {

	for (int i = 0; i < frames.size(); ++i) {

		std::vector<vk::ImageView> attachments = {
			frames[i].imageView,
			frames[i].depthBufferView
		};

		vk::FramebufferCreateInfo framebufferInfo;
		framebufferInfo.flags = vk::FramebufferCreateFlags();
		framebufferInfo.renderPass = inputChunk.renderpass;
		framebufferInfo.attachmentCount = attachments.size();
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = inputChunk.swapchainExtent.width;
		framebufferInfo.height = inputChunk.swapchainExtent.height;
		framebufferInfo.layers = 1;

		try {
			frames[i].framebuffer = inputChunk.device.createFramebuffer(framebufferInfo);

			#ifdef DEBUG_MESSAGE
				std::cout << "Created framebuffer for frame " << i << std::endl;
			#endif
		}
		catch (vk::SystemError err) {
			#ifdef DEBUG_MESSAGE
				std::cout << "Failed to create framebuffer for frame " << i << std::endl;
			#endif
		}

	}
}