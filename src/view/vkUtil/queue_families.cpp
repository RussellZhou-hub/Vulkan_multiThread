#include "../../config.h"
#include"queue_families.h"

bool vkUtil::QueueFamilyIndices::isComplete(){
	return vkUtil::QueueFamilyIndices::graphicsFamily.has_value() && vkUtil::QueueFamilyIndices::presentFamily.has_value();
}

vkUtil::QueueFamilyIndices vkUtil::findQueueFamilies(vk::PhysicalDevice device,vk::SurfaceKHR surface){
        QueueFamilyIndices indices;

	    std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

	    #ifdef DEBUG_MESSAGE
		    std::cout<<"System can support "<<queueFamilies.size()<<" queue families.\n";
	    #endif

	    int i=0;
	    for(vk::QueueFamilyProperties queueFamily:queueFamilies){


		/*
			* // Provided by VK_VERSION_1_0
				typedef struct VkQueueFamilyProperties {
				VkQueueFlags    queueFlags;
				uint32_t        queueCount;
				uint32_t        timestampValidBits;
				VkExtent3D      minImageTransferGranularity;
				} VkQueueFamilyProperties;
				queueFlags is a bitmask of VkQueueFlagBits indicating capabilities of the queues in this queue family.
				queueCount is the unsigned integer count of queues in this queue family. Each queue family must support 
				at least one queue.
				timestampValidBits is the unsigned integer count of meaningful bits in the timestamps written via 
				vkCmdWriteTimestamp. The valid range for the count is 36..64 bits, or a value of 0, 
				indicating no support for timestamps. Bits outside the valid range are guaranteed to be zeros.
				minImageTransferGranularity is the minimum granularity supported for image transfer 
				operations on the queues in this queue family.
			*/

			/*
			* // Provided by VK_VERSION_1_0
				typedef enum VkQueueFlagBits {
				VK_QUEUE_GRAPHICS_BIT = 0x00000001,
				VK_QUEUE_COMPUTE_BIT = 0x00000002,
				VK_QUEUE_TRANSFER_BIT = 0x00000004,
				VK_QUEUE_SPARSE_BINDING_BIT = 0x00000008,
				} VkQueueFlagBits;
			*/

		if(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics){
			indices.graphicsFamily=i;
			indices.presentFamily=i;

			#ifdef DEBUG_MESSAGE
				std::cout<<"Queue Family "<<i<<"is suitable for graphics and presenting.";
			#endif
		}

		if (device.getSurfaceSupportKHR(i, surface)) {
			indices.presentFamily = i;

			#ifdef DEBUG_MESSAGE
				std::cout << "Queue Family " << i << " is suitable for presenting\n";
			#endif
		}

		if(indices.isComplete()){
			break;
		}
	    }

	    return indices;
    }