#include "engine.h"

Engine::Engine(int width, int height){
    this->width=width;
    this->height=height;

    frameNumber_atomic.store(0); // initialize frame index

    build_glfw_window();
    create_instance();
    create_device();
    create_descriptor_set_layouts();
	create_pipeline();
    create_framebuffers();
	create_commandbuffer();
	create_frame_resources();
	load_assets();
	create_vertexbuffer();
	create_indexbuffer();
}

Engine::~Engine(){

}

void Engine::build_glfw_window(){
    //initialize glfw
	glfwInit();

	//no default rendering client, we'll hook vulkan up
	//to the window later
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//resizing breaks the swapchain, we'll disable it for now
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	//GLFWwindow* glfwCreateWindow (int width, int height, const char *title, GLFWmonitor *monitor, GLFWwindow *share)
	if (window = glfwCreateWindow(width, height, "Vulkan Parallel rendering", nullptr, nullptr)) {
		std::cout << "Successfully made a glfw window called \"Vulkan Parallel rendering\", width: " << width << ", height: " << height << '\n';
	}
	else {
		std::cout << "GLFW window creation failed\n";
        LOG_ERROR()
	}
}

void Engine::run(){

        std::vector<std::thread> threads;
        for(auto i =0;i<NUM_THREADS;i++){
            threads.push_back(std::thread(threadFunc,instance,surface,i));
        }

        for(auto& thread : threads){
            //thread.join();
            thread.detach();
        }

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);
        
        
    }
}

void Engine::create_instance(){
    instance = vkInit::create_instance("Vulkan multi thread");
	dldi = vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);

	if (vkLogging::Logger::get_logger()->get_debug_mode()) {
		debugMessenger = vkLogging::make_debug_messenger(instance, dldi);
	}
	VkSurfaceKHR c_style_surface;
	if (glfwCreateWindowSurface(instance, window, nullptr, &c_style_surface) != VK_SUCCESS) {
		std::cout << "Failed to abstract glfw surface for Vulkan\n";
        LOG_ERROR()
	}
	std::cout << "Successfully abstracted glfw surface for Vulkan\n";
	//copy constructor converts to hpp convention
	surface = c_style_surface;
}

void Engine::create_device(){
    
    physicalDevice = vkInit::choose_physical_device(instance);
	device = vkInit::create_logical_device(physicalDevice, surface);
	std::array<vk::Queue,2> queues = vkInit::get_queues(physicalDevice, device, surface);
	graphicsQueue = queues[0];
	presentQueue = queues[1];
	create_swapchain();
	frameNumber=0;
	//frameNumberTotal.store(0);

}

void Engine::create_swapchain(){
    vkInit::SwapChainBundle bundle = vkInit::create_swapchain(device, physicalDevice, surface, width, height);
	swapchain = bundle.swapchain;
	swapchainFrames = bundle.frames;
	swapchainFormat = bundle.format;
	swapchainExtent = bundle.extent;
	maxFramesInFlight=static_cast<int>(swapchainFrames.size());

	std::cout<<"maxFramesInFlight: "<<maxFramesInFlight<<" \n";

    for (vkUtil::SwapChainFrame& frame : swapchainFrames) {
		frame.logicalDevice = device;
		frame.physicalDevice = physicalDevice;
		frame.width = swapchainExtent.width;
		frame.height = swapchainExtent.height;

		frame.make_depth_resources();
	}

    // copy to each thread to render different part of the same frame
    for(auto& res:renderThreadResources){
        res.swapchain = bundle.swapchain;
        res.swapchainFrames= swapchainFrames;
        res.swapchainFormat = bundle.format;
	    res.swapchainExtent = bundle.extent;
	    res.maxFramesInFlight=static_cast<int>(swapchainFrames.size());
    }
    for(auto& res:renderThreadResources){ // each thread needs its own image view
        for(auto& frame:res.swapchainFrames){
            frame.imageView=vkImage::make_image_view(device, frame.image, res.swapchainFormat,vk::ImageAspectFlagBits::eColor);
            frame.depthBufferView=vkImage::make_image_view(device, frame.depthBuffer, frame.depthFormat, vk::ImageAspectFlagBits::eDepth);
        }
    }
}

void Engine::create_descriptor_set_layouts(){

    for(auto i=0;i<NUM_THREADS;i++){
        vkInit::descriptorSetLayoutData bindings;
	    bindings.count=2;

	    bindings.indices.push_back(0);
	    bindings.types.push_back(vk::DescriptorType::eUniformBuffer);
	    bindings.counts.push_back(1);
	    bindings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

	    bindings.indices.push_back(1);
	    bindings.types.push_back(vk::DescriptorType::eStorageBuffer);
	    bindings.counts.push_back(1);
	    bindings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

        renderThreadResources[i].frameSetLayout=vkInit::make_descriptor_set_layout(device,bindings);

        /* 
	    bindings.count=1;

	    bindings.indices[0]=0;
	    bindings.types[0]=vk::DescriptorType::eCombinedImageSampler;
	    bindings.counts[0]=1;
	    bindings.stages[0]=vk::ShaderStageFlagBits::eFragment;

	    renderThreadResources[i].meshSetLayout = vkInit::make_descriptor_set_layout(device,bindings);
        */
    }
}

void Engine::create_pipeline(){
    vkInit::GraphicsPipelineInBundle specification = {};
	specification.device = device;
	specification.vertexFilepath = "shaders/vertex.spv";
	specification.fragmentFilepath = "shaders/fragment.spv";
	specification.swapchainExtent = swapchainExtent;
	specification.swapchainImageFormat = swapchainFormat;
	specification.depthFormat = swapchainFrames[0].depthFormat;
	//specification.descriptorSetLayouts = { frameSetLayout /* , meshSetLayout */ };

    for(auto& res:renderThreadResources){
        specification.descriptorSetLayouts = { res.frameSetLayout /* , meshSetLayout */ };
        vkInit::GraphicsPipelineOutBundle output = vkInit::create_graphics_pipeline(
		specification
	    );

	    res.pipelineLayout = output.layout;
	    res.renderpass = output.renderpass;
	    res.pipeline = output.pipeline;
    }

}

void Engine::create_framebuffers(){
    for(auto& res:renderThreadResources){
        vkInit::framebufferInput frameBufferInput;
	    frameBufferInput.device = device;
	    frameBufferInput.renderpass = res.renderpass;
	    frameBufferInput.swapchainExtent = res.swapchainExtent;
	    vkInit::make_framebuffers(frameBufferInput, res.swapchainFrames);
    }
}

void Engine::create_commandbuffer(){
	for(auto& res:renderThreadResources){
		res.commandPool = vkInit::make_command_pool(device, physicalDevice, surface);
		vkInit::commandBufferInputChunk commandBufferInput = { device, res.commandPool, res.swapchainFrames };
		res.mainCommandBuffer = vkInit::make_command_buffer(commandBufferInput);
		vkInit::make_frame_command_buffers(commandBufferInput);
	}
}

void Engine::create_frame_resources(){


	vkInit::descriptorSetLayoutData bindings;
	bindings.count=2;
	bindings.indices.push_back(0);
	bindings.indices.push_back(1);
	bindings.types.push_back(vk::DescriptorType::eUniformBuffer);
	bindings.types.push_back(vk::DescriptorType::eStorageBuffer);
	bindings.counts.push_back(1);
	bindings.counts.push_back(1);
	bindings.stages.push_back(vk::ShaderStageFlagBits::eVertex);
	bindings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

	for(auto& res:renderThreadResources){
		res.frameDescriptorPool = vkInit::make_descriptor_pool(device,static_cast<uint32_t>(swapchainFrames.size()),bindings);

		for(vkUtil::SwapChainFrame& frame: res.swapchainFrames){
			frame.inFlight = vkInit::make_fence(device);
			frame.imageAvailable = vkInit::make_semaphore(device);
			frame.renderFinished = vkInit::make_semaphore(device);

			frame.make_descriptor_resources();

			frame.descriptorSet = vkInit::allocate_descriptor_set(device,res.frameDescriptorPool,res.frameSetLayout);
		}
	}
}

void Engine::load_assets(){

	glm::vec3 frontNormal = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 backNormal = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 topNormal = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 bottomNormal = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::vec3 leftNormal = glm::vec3(-1.0f, 0.0f, 0.0f);
	glm::vec3 rightNormal = glm::vec3(1.0f, 0.0f, 0.0f);

	std::vector<vkMesh::Vertex> vertices = {
    	{glm::vec3(-1.0f, -1.0f, 1.0f), frontNormal},
    	{glm::vec3(1.0f, -1.0f, 1.0f), frontNormal},
    	{glm::vec3(1.0f, 1.0f, 1.0f), frontNormal},
    	{glm::vec3(-1.0f, 1.0f, 1.0f), frontNormal},
    	{glm::vec3(-1.0f, -1.0f, -1.0f), backNormal},
    	{glm::vec3(1.0f, -1.0f, -1.0f), backNormal},
    	{glm::vec3(1.0f, 1.0f, -1.0f), backNormal},
    	{glm::vec3(-1.0f, 1.0f, -1.0f), backNormal},
    	{glm::vec3(-1.0f, 1.0f, 1.0f), topNormal},
    	{glm::vec3(1.0f, 1.0f, 1.0f), topNormal},
    	{glm::vec3(1.0f, 1.0f, -1.0f), topNormal},
    	{glm::vec3(-1.0f, 1.0f, -1.0f), topNormal},
    	{glm::vec3(-1.0f, -1.0f, 1.0f), bottomNormal},
    	{glm::vec3(1.0f, -1.0f, 1.0f), bottomNormal},
    	{glm::vec3(1.0f, -1.0f, -1.0f), bottomNormal},
    	{glm::vec3(-1.0f, -1.0f, -1.0f), bottomNormal},
    	{glm::vec3(-1.0f, -1.0f, 1.0f), leftNormal},
    	{glm::vec3(-1.0f, 1.0f, 1.0f), leftNormal},
    	{glm::vec3(-1.0f, 1.0f, -1.0f), leftNormal},
    	{glm::vec3(-1.0f, -1.0f, -1.0f), leftNormal},
    	{glm::vec3(1.0f, -1.0f, 1.0f), rightNormal},
    	{glm::vec3(1.0f, 1.0f, 1.0f), rightNormal},
    	{glm::vec3(1.0f, 1.0f, -1.0f), rightNormal},
    	{glm::vec3(1.0f, -1.0f, -1.0f), rightNormal},
	};

	std::vector<uint32_t> indices = {
    	0, 1, 2, 2, 3, 0, // Front face
    	4, 5, 6, 6, 7, 4, //back face
    	8, 9, 10, 10, 11, 8, // Top face
    	12, 13, 14, 14, 15, 12, // Bottom face
    	16, 17, 18, 18, 19, 16, // Left face
    	20, 21, 22, 22, 23, 20, // Right face
	};

	for(auto& res:renderThreadResources){
		res.vertices=vertices;
		res.indices=indices;
	}
}

void Engine::create_vertexbuffer(){
	for(auto& res:renderThreadResources){
		FinalizationChunk finalizationChunk;
		finalizationChunk.logicalDevice = device;
		finalizationChunk.physicalDevice=physicalDevice;
		finalizationChunk.queue=graphicsQueue;
		finalizationChunk.commandBuffer=res.mainCommandBuffer;

		//make a staging buffer for vertices
		BufferInputChunk inputChunk;
		inputChunk.logicalDevice = finalizationChunk.logicalDevice;
		inputChunk.physicalDevice = finalizationChunk.physicalDevice;
		inputChunk.size = sizeof(vkMesh::Vertex) * res.vertices.size();
		inputChunk.usage = vk::BufferUsageFlagBits::eTransferSrc;
		inputChunk.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
		Buffer stagingBuffer = vkUtil::createBuffer(inputChunk);

		//fill it with vertex data
		void* memoryLocation = device.mapMemory(stagingBuffer.bufferMemory, 0, inputChunk.size);
		memcpy(memoryLocation, res.vertices.data(), inputChunk.size);
		device.unmapMemory(stagingBuffer.bufferMemory);

		//make the vertex buffer
		inputChunk.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
		inputChunk.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
		res.vertexBuffer = vkUtil::createBuffer(inputChunk);

		//copy to it
		vkUtil::copyBuffer(stagingBuffer, res.vertexBuffer, inputChunk.size, finalizationChunk.queue, finalizationChunk.commandBuffer);

		//destroy staging buffer
		device.destroyBuffer(stagingBuffer.buffer);
		device.freeMemory(stagingBuffer.bufferMemory);
	}

}
	
void Engine::create_indexbuffer(){
	for(auto& res:renderThreadResources){
		FinalizationChunk finalizationChunk;
		finalizationChunk.logicalDevice = device;
		finalizationChunk.physicalDevice=physicalDevice;
		finalizationChunk.queue=graphicsQueue;
		finalizationChunk.commandBuffer=res.mainCommandBuffer;

		//make a staging buffer for indices
		BufferInputChunk inputChunk;
		inputChunk.logicalDevice = finalizationChunk.logicalDevice;
		inputChunk.physicalDevice = finalizationChunk.physicalDevice;
		inputChunk.size = sizeof(uint32_t) * res.indices.size();
		inputChunk.usage = vk::BufferUsageFlagBits::eTransferSrc;
		inputChunk.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
		Buffer stagingBuffer = vkUtil::createBuffer(inputChunk);

		//fill it with index data
		void* memoryLocation = device.mapMemory(stagingBuffer.bufferMemory, 0, inputChunk.size);
		memcpy(memoryLocation, res.indices.data(), inputChunk.size);
		device.unmapMemory(stagingBuffer.bufferMemory);

		//make the index buffer
		inputChunk.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
		inputChunk.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
		res.indexBuffer = vkUtil::createBuffer(inputChunk);

		//copy to it
		vkUtil::copyBuffer(stagingBuffer, res.indexBuffer, inputChunk.size, finalizationChunk.queue, finalizationChunk.commandBuffer);

		//destroy staging buffer
		device.destroyBuffer(stagingBuffer.buffer);
		device.freeMemory(stagingBuffer.bufferMemory);
	}
}

void Engine::render(){
	
}

std::mutex Engine::instanceMutex;

void Engine::threadFunc(vk::Instance instance,vk::SurfaceKHR surface,int index){
    //std::unique_lock<std::mutex> lock(instanceMutex,std::defer_lock);
    //while(!lock.try_lock()){
    //    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    //}
    std::cout<<"thread "<<index<<" is running\n";
    //vk::Device pDevice=vkInit::create_logical_device(physicalDevice,surface);
    //std::cout<<"thread "<<index<<" is using physicalDevice\n";

    //lock.unlock();
}

vk::PhysicalDevice Engine::physicalDevice=nullptr;