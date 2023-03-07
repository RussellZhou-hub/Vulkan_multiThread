#include "instance.h"


vk::Instance vkInit::create_instance(const char* applicationName){

    std::cout << "creating an instance...\n";

    uint32_t version{ 0 };
	vkEnumerateInstanceVersion(&version);

#ifdef DEBUG_MESSAGE
	std::cout << "System can support vulkan Variant: " << VK_API_VERSION_VARIANT(version)
				<< ", Major: " << VK_API_VERSION_MAJOR(version)
				<< ", Minor: " << VK_API_VERSION_MINOR(version)
				<< ", Patch: " << VK_API_VERSION_PATCH(version) << '\n';
#endif

	/*
	* We can then either use this version
	* (We shoud just be sure to set the patch to 0 for best compatibility/stability)
	*/
	version &= ~(0xFFFU);

	/*
	* Or drop down to an earlier version to ensure compatibility with more devices
	* VK_MAKE_API_VERSION(variant, major, minor, patch)
	*/
	version = VK_MAKE_API_VERSION(0, 1, 0, 0);

	/*
	* from vulkan_structs.hpp:
	*
	* VULKAN_HPP_CONSTEXPR ApplicationInfo( const char * pApplicationName_   = {},
									  uint32_t     applicationVersion_ = {},
									  const char * pEngineName_        = {},
									  uint32_t     engineVersion_      = {},
									  uint32_t     apiVersion_         = {} )
	*/

	vk::ApplicationInfo appInfo = vk::ApplicationInfo(
		applicationName,
		version,
		"Doing it the hard way",
		version,
		version
	);

	/*
	* Everything with Vulkan is "opt-in", so we need to query which extensions glfw needs
	* in order to interface with vulkan.
	*/
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	//In order to hook in a custom validation callback
#ifdef DEBUG_MESSAGE
		extensions.push_back("VK_EXT_debug_utils");
#endif

#ifdef DEBUG_MESSAGE
		std::cout << "extensions to be requested:\n";

		for (const char* extensionName : extensions) {
			std::cout << "\t\"" << extensionName << "\"\n";
		}
#endif

	std::vector<const char*> layers;

#ifdef DEBUG_MESSAGE
		layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

	if (!supported(extensions, layers)) {
		return nullptr;
	}

	/*
	* 
	* from vulkan_structs.hpp:
	* 
	* InstanceCreateInfo( VULKAN_HPP_NAMESPACE::InstanceCreateFlags     flags_                 = {},
                                         const VULKAN_HPP_NAMESPACE::ApplicationInfo * pApplicationInfo_      = {},
                                         uint32_t                                      enabledLayerCount_     = {},
                                         const char * const *                          ppEnabledLayerNames_   = {},
                                         uint32_t                                      enabledExtensionCount_ = {},
                                         const char * const * ppEnabledExtensionNames_ = {} )
	*/
	vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo(
		vk::InstanceCreateFlags(),
		&appInfo,
		static_cast<uint32_t>(layers.size()), layers.data(), // enabled layers
		static_cast<uint32_t>(extensions.size()), extensions.data() // enabled extensions
	);


	try {
		/*
		* from vulkan_funcs.h:
		* 
		* createInstance( const VULKAN_HPP_NAMESPACE::InstanceCreateInfo &          createInfo,
                Optional<const VULKAN_HPP_NAMESPACE::AllocationCallbacks> allocator = nullptr,
                Dispatch const &                                          d = ::vk::getDispatchLoaderStatic())
			
		*/
		return vk::createInstance(createInfo);
	}
	catch (vk::SystemError err) {
        #ifdef DEBUG_MESSAGE
			std::cout << "Failed to create Instance!\n"<<"error: "<<err.what()<<std::endl;
            LOG_ERROR()
        #endif
		return nullptr;
	}
}

bool vkInit::supported(std::vector<const char*>& extensions, std::vector<const char*>& layers){
    //check extension support
		std::vector<vk::ExtensionProperties> supportedExtensions = vk::enumerateInstanceExtensionProperties();

		#ifdef DEBUG_MESSAGE
			std::cout << "Device can support the following extensions:\n";
			for (vk::ExtensionProperties supportedExtension : supportedExtensions) {
				std::cout << '\t' << supportedExtension.extensionName << '\n';
			}
		#endif

		bool found;
		for (const char* extension : extensions) {
			found = false;
			for (vk::ExtensionProperties supportedExtension : supportedExtensions) {
				if (strcmp(extension, supportedExtension.extensionName) == 0) {
					found = true;
					#ifdef DEBUG_MESSAGE
						std::cout << "Extension \"" << extension << "\" is supported!\n";
					#endif
				}
			}
			if (!found) {
				#ifdef DEBUG_MESSAGE
					std::cout << "Extension \"" << extension << "\" is not supported!\n";
				#endif
				return false;
			}
		}

		//check layer support
		std::vector<vk::LayerProperties> supportedLayers = vk::enumerateInstanceLayerProperties();

		#ifdef DEBUG_MESSAGE
			std::cout << "Device can support the following layers:\n";
			for (vk::LayerProperties supportedLayer : supportedLayers) {
				std::cout << '\t' << supportedLayer.layerName << '\n';
			}
		#endif

		for (const char* layer : layers) {
			found = false;
			for (vk::LayerProperties supportedLayer : supportedLayers) {
				if (strcmp(layer, supportedLayer.layerName) == 0) {
					found = true;
					#ifdef DEBUG_MESSAGE
						std::cout << "Layer \"" << layer << "\" is supported!\n";
					#endif
				}
			}
			if (!found) {
				#ifdef DEBUG_MESSAGE
					std::cout << "Layer \"" << layer << "\" is not supported!\n";
				#endif
				return false;
			}
		}

		return true;
}