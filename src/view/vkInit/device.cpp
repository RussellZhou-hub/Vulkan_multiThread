#include "device.h"

/*
* Vulkan separates the concept of physical and logical devices. 
* 
  A physical device usually represents a single complete implementation of Vulkan 
  (excluding instance-level functionality) available to the host, 
  of which there are a finite number. 
  
  A logical device represents an instance of that implementation 
  with its own state and resources independent of other logical devices.
*/

vk::PhysicalDevice vkInit::choose_physical_device( vk::Instance& instance){
    #ifdef DEBUG_MESSAGE
      std::cout<<"Choosing physical device...\n";
    #endif
    /*
	    * ResultValueType<std::vector<PhysicalDevice, PhysicalDeviceAllocator>>::type
		    Instance::enumeratePhysicalDevices( Dispatch const & d )
		    std::vector<vk::PhysicalDevice> instance.enumeratePhysicalDevices( Dispatch const & d = static/default )
	    */
	    std::vector<vk::PhysicalDevice> availableDevices = instance.enumeratePhysicalDevices();
    #ifdef DEBUG_MESSAGE
      std::cout << "There are " << availableDevices.size() << " physical devices available on this system\n";
    #endif

    /*
	    * check if a suitable device can be found
	    */
	    for (vk::PhysicalDevice device : availableDevices) {

		  #ifdef DEBUG_MESSAGE
			    vkLogging::log_device_properties(device);
		  #endif
		    if (isSuitable(device)) {
			    return device;
		    }
	    }

        return nullptr;

}

bool vkInit::isSuitable(const vk::PhysicalDevice& device){
  #ifdef DEBUG_MESSAGE
      std::cout << "Checking if device is suitable\n";
  #endif
      /*
		* A device is suitable if it can present to the screen, ie support
		* the swapchain extension
		*/
		const std::vector<const char*> requestedExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
    #ifdef DEBUG_MESSAGE
    std::cout << "We are requesting device extensions:\n";

		for (const char* extension : requestedExtensions) {
			std::cout << "\t\"" << extension << "\"\n";
		}
    #endif

    if (bool extensionsSupported = checkDeviceExtensionSupport(device, requestedExtensions)) {

			#ifdef DEBUG_MESSAGE
				std::cout << "Device can support the requested extensions!\n";
			#endif
		}
		else {

			#ifdef DEBUG_MESSAGE
				std::cout << "Device can't support the requested extensions!\n";
			#endif

			return false;
		}
		return true;
}

bool vkInit::checkDeviceExtensionSupport(
		const vk::PhysicalDevice& device,
		const std::vector<const char*>& requestedExtensions
	){
    /*
		* Check if a given physical device can satisfy a list of requested device
		* extensions.
		*/

    std::set<std::string> requiredExtensions(requestedExtensions.begin(), requestedExtensions.end());

		#ifdef DEBUG_MESSAGE
			std::cout << "Device can support extensions:\n";
		#endif
    for (vk::ExtensionProperties& extension : device.enumerateDeviceExtensionProperties()) {

			#ifdef DEBUG_MESSAGE
				std::cout << "\t\"" << extension.extensionName << "\"\n";
			#endif

			//remove this from the list of required extensions (set checks for equality automatically)
			requiredExtensions.erase(extension.extensionName);
		}

		//if the set is empty then all requirements have been satisfied
		return requiredExtensions.empty();
}

vk::Device vkInit::create_logical_device(vk::PhysicalDevice physicalDevice,vk::SurfaceKHR surface){
      /*
		* Create an abstraction around the GPU
		*/

		/*
		* At time of creation, any required queues will also be created,
		* so queue create info must be passed in.
		*/

		vkUtil::QueueFamilyIndices indices = vkUtil::findQueueFamilies(physicalDevice,surface);
		float queuePriority = 1.0f;
		/*
		* VULKAN_HPP_CONSTEXPR DeviceQueueCreateInfo( VULKAN_HPP_NAMESPACE::DeviceQueueCreateFlags flags_            = {},
                                                uint32_t                                     queueFamilyIndex_ = {},
                                                uint32_t                                     queueCount_       = {},
                                                const float * pQueuePriorities_ = {} ) VULKAN_HPP_NOEXCEPT
		*/
		vk::DeviceQueueCreateInfo queueCreateInfo = vk::DeviceQueueCreateInfo(
			vk::DeviceQueueCreateFlags(), indices.graphicsFamily.value(),
			1, &queuePriority
		);

		std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		/*
		* Device features must be requested before the device is abstracted,
		* therefore we only pay for what we need.
		*/

		vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures();

		/*
		* VULKAN_HPP_CONSTEXPR DeviceCreateInfo( VULKAN_HPP_NAMESPACE::DeviceCreateFlags flags_                         = {},
                                           uint32_t                                queueCreateInfoCount_          = {},
                                           const VULKAN_HPP_NAMESPACE::DeviceQueueCreateInfo * pQueueCreateInfos_ = {},
                                           uint32_t                                            enabledLayerCount_ = {},
                                           const char * const * ppEnabledLayerNames_                              = {},
                                           uint32_t             enabledExtensionCount_                            = {},
                                           const char * const * ppEnabledExtensionNames_                          = {},
                                           const VULKAN_HPP_NAMESPACE::PhysicalDeviceFeatures * pEnabledFeatures_ = {} )
		*/
		std::vector<const char*> enabledLayers;
		#ifdef DEBUG_MESSAGE
			enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
		#endif
		vk::DeviceCreateInfo deviceInfo = vk::DeviceCreateInfo(
			vk::DeviceCreateFlags(), 
			1, &queueCreateInfo,
			enabledLayers.size(), enabledLayers.data(),
			deviceExtensions.size(), deviceExtensions.data(),
			&deviceFeatures
		);

		try {
			vk::Device device = physicalDevice.createDevice(deviceInfo);
			#ifdef DEBUG_MESSAGE
				std::cout << "GPU has been successfully abstracted!\n";
			#endif
			return device;
		}
		catch (vk::SystemError err) {
			#ifdef DEBUG_MESSAGE
				std::cout << "Device creation failed!\n";
				return nullptr;
			#endif
		}
		return nullptr;
	}

	/**
		Get the graphics queue.
		\param physicalDevice the physical device
		\param device the logical device
		\param debug whether the system is running in debug mode
		\returns the physical device's graphics queue
	*/
	std::array<vk::Queue,2> get_queues(vk::PhysicalDevice physicalDevice, vk::Device device, vk::SurfaceKHR surface) {

		vkUtil::QueueFamilyIndices indices = vkUtil::findQueueFamilies(physicalDevice,surface);

		return { {
				device.getQueue(indices.graphicsFamily.value(), 0),
				device.getQueue(indices.presentFamily.value(), 0),
			} };
}

/**
		Get the graphics queue.
		\param physicalDevice the physical device
		\param device the logical device
		\param debug whether the system is running in debug mode
		\returns the physical device's graphics queue
	*/
	std::array<vk::Queue,2> vkInit::get_queues(vk::PhysicalDevice physicalDevice, vk::Device device, vk::SurfaceKHR surface){
      vkUtil::QueueFamilyIndices indices = vkUtil::findQueueFamilies(physicalDevice,surface);

		return { {
				device.getQueue(indices.graphicsFamily.value(), 0),
				device.getQueue(indices.presentFamily.value(), 0),
			} };
  }