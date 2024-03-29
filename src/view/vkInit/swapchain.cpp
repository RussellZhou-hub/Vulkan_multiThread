#include"swapchain.h"

vkInit::SwapChainSupportDetails vkInit::query_swapchain_support(vk::PhysicalDevice device,vk::SurfaceKHR surface){
		SwapChainSupportDetails support;
		support.capabilities = device.getSurfaceCapabilitiesKHR(surface);
		if(support.capabilities.maxImageCount<support.capabilities.minImageCount) support.capabilities.maxImageCount=support.capabilities.minImageCount;
		#ifdef DEBUG_MESSAGE
			std::cout << "Swapchain can support the following surface capabilities:\n";

			std::cout << "\tminimum image count: " << support.capabilities.minImageCount << '\n';
			std::cout << "\tmaximum image count: " << support.capabilities.maxImageCount << '\n';

			std::cout << "\tcurrent extent: \n";
			/*typedef struct VkExtent2D {
				uint32_t    width;
				uint32_t    height;
			} VkExtent2D;
			*/
			std::cout << "\t\twidth: " << support.capabilities.currentExtent.width << '\n';
			std::cout << "\t\theight: " << support.capabilities.currentExtent.height << '\n';

			std::cout << "\tminimum supported extent: \n";
			std::cout << "\t\twidth: " << support.capabilities.minImageExtent.width << '\n';
			std::cout << "\t\theight: " << support.capabilities.minImageExtent.height << '\n';

			std::cout << "\tmaximum supported extent: \n";
			std::cout << "\t\twidth: " << support.capabilities.maxImageExtent.width << '\n';
			std::cout << "\t\theight: " << support.capabilities.maxImageExtent.height << '\n';

			std::cout << "\tmaximum image array layers: " << support.capabilities.maxImageArrayLayers << '\n';

			
			std::cout << "\tsupported transforms:\n";
			std::vector<std::string> stringList = vkLogging::log_transform_bits(support.capabilities.supportedTransforms);
			for (std::string line : stringList) {
				std::cout << "\t\t" << line << '\n';
			}

			std::cout << "\tcurrent transform:\n";
			stringList = vkLogging::log_transform_bits(support.capabilities.currentTransform);
			for (std::string line : stringList) {
				std::cout << "\t\t" << line << '\n';
			}

			std::cout << "\tsupported alpha operations:\n";
			stringList = vkLogging::log_alpha_composite_bits(support.capabilities.supportedCompositeAlpha);
			for (std::string line : stringList) {
				std::cout << "\t\t" << line << '\n';
			}

			std::cout << "\tsupported image usage:\n";
			stringList = vkLogging::log_image_usage_bits(support.capabilities.supportedUsageFlags);
			for (std::string line : stringList) {
				std::cout << "\t\t" << line << '\n';
			}
		#endif

		support.formats = device.getSurfaceFormatsKHR(surface);

		#ifdef DEBUG_MESSAGE

			for (vk::SurfaceFormatKHR supportedFormat : support.formats) {
				/*
				* typedef struct VkSurfaceFormatKHR {
					VkFormat           format;
					VkColorSpaceKHR    colorSpace;
				} VkSurfaceFormatKHR;
				*/

				std::cout << "supported pixel format: " << vk::to_string(supportedFormat.format) << '\n';
				std::cout << "supported color space: " << vk::to_string(supportedFormat.colorSpace) << '\n';
			}
		#endif

		support.presentModes = device.getSurfacePresentModesKHR(surface);

		for (vk::PresentModeKHR presentMode : support.presentModes) {
			std::cout << '\t' << vkLogging::log_present_mode(presentMode) << '\n';
		}
		return support;
}

vk::SurfaceFormatKHR vkInit::choose_swapchain_surface_format(std::vector<vk::SurfaceFormatKHR> formats){
		for (vk::SurfaceFormatKHR format : formats) {
			if (format.format == vk::Format::eB8G8R8A8Unorm
				&& format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				return format;
			}
		}

		return formats[0];
}

/**
		Choose a present mode.
		\param presentModes a vector of present modes supported by the device
		\returns the chosen present mode
	*/
vk::PresentModeKHR vkInit::choose_swapchain_present_mode(std::vector<vk::PresentModeKHR> presentModes) {
		
		for (vk::PresentModeKHR presentMode : presentModes) {
			if (presentMode == vk::PresentModeKHR::eMailbox) {
				return presentMode;
			}
		}

		return vk::PresentModeKHR::eFifo;
}

/**
	Choose an extent for the swapchain.
	\param width the requested width
	\param height the requested height
	\param capabilities a struct describing the supported capabilities of the device
	\returns the chosen extent
*/
vk::Extent2D vkInit::choose_swapchain_extent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR capabilities) {

		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		}
		else {
			vk::Extent2D extent = { width, height };

			extent.width = std::min(
				capabilities.maxImageExtent.width, 
				std::max(capabilities.minImageExtent.width, extent.width)
			);

			extent.height = std::min(
				capabilities.maxImageExtent.height,
				std::max(capabilities.minImageExtent.height, extent.height)
			);

			return extent;
		}
}

vkInit::SwapChainBundle vkInit::create_swapchain(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, int width, int height) {

		vkInit::SwapChainSupportDetails support = query_swapchain_support(physicalDevice, surface);

		vk::SurfaceFormatKHR format = choose_swapchain_surface_format(support.formats);

		vk::PresentModeKHR presentMode = choose_swapchain_present_mode(support.presentModes);

		vk::Extent2D extent = choose_swapchain_extent(width, height, support.capabilities);

		uint32_t imageCount = std::min(
			support.capabilities.maxImageCount,
			support.capabilities.minImageCount + 1
		);

		std::cout<<"imageCount: "<<imageCount<<" \n";
		std::cout<<"support.capabilities.maxImageCount： "<<support.capabilities.maxImageCount<<" \n";
		

		/*
		* VULKAN_HPP_CONSTEXPR SwapchainCreateInfoKHR(
      VULKAN_HPP_NAMESPACE::SwapchainCreateFlagsKHR flags_         = {},
      VULKAN_HPP_NAMESPACE::SurfaceKHR              surface_       = {},
      uint32_t                                      minImageCount_ = {},
      VULKAN_HPP_NAMESPACE::Format                  imageFormat_   = VULKAN_HPP_NAMESPACE::Format::eUndefined,
      VULKAN_HPP_NAMESPACE::ColorSpaceKHR   imageColorSpace_  = VULKAN_HPP_NAMESPACE::ColorSpaceKHR::eSrgbNonlinear,
      VULKAN_HPP_NAMESPACE::Extent2D        imageExtent_      = {},
      uint32_t                              imageArrayLayers_ = {},
      VULKAN_HPP_NAMESPACE::ImageUsageFlags imageUsage_       = {},
      VULKAN_HPP_NAMESPACE::SharingMode     imageSharingMode_ = VULKAN_HPP_NAMESPACE::SharingMode::eExclusive,
      uint32_t                              queueFamilyIndexCount_ = {},
      const uint32_t *                      pQueueFamilyIndices_   = {},
      VULKAN_HPP_NAMESPACE::SurfaceTransformFlagBitsKHR preTransform_ =
        VULKAN_HPP_NAMESPACE::SurfaceTransformFlagBitsKHR::eIdentity,
      VULKAN_HPP_NAMESPACE::CompositeAlphaFlagBitsKHR compositeAlpha_ =
        VULKAN_HPP_NAMESPACE::CompositeAlphaFlagBitsKHR::eOpaque,
      VULKAN_HPP_NAMESPACE::PresentModeKHR presentMode_  = VULKAN_HPP_NAMESPACE::PresentModeKHR::eImmediate,
      VULKAN_HPP_NAMESPACE::Bool32         clipped_      = {},
      VULKAN_HPP_NAMESPACE::SwapchainKHR   oldSwapchain_ = {} ) VULKAN_HPP_NOEXCEPT
		*/
		vk::SwapchainCreateInfoKHR createInfo = vk::SwapchainCreateInfoKHR(
			vk::SwapchainCreateFlagsKHR(), surface, imageCount, format.format, format.colorSpace,
			extent, 1, vk::ImageUsageFlagBits::eColorAttachment
		);


		vkUtil::QueueFamilyIndices indices = vkUtil::findQueueFamilies(physicalDevice, surface);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = vk::SharingMode::eExclusive;
		}

		createInfo.preTransform = support.capabilities.currentTransform;
		createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

		SwapChainBundle bundle{};
		try {
			bundle.swapchain = logicalDevice.createSwapchainKHR(createInfo);
		}
		catch (vk::SystemError err) {
			throw std::runtime_error("failed to create swap chain!");
		}

        std::vector<vk::Image> images = logicalDevice.getSwapchainImagesKHR(bundle.swapchain);
        bundle.frames.resize(images.size());

        for(size_t i=0;i<images.size();++i){

			bundle.frames[i].image = images[i];
			bundle.frames[i].imageView = vkImage::make_image_view(logicalDevice, images[i], format.format,vk::ImageAspectFlagBits::eColor);
        }
		bundle.format = format.format;
		bundle.extent = extent;

		return bundle;
}