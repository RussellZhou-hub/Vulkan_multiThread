#include "engine.h"

Engine::Engine(int width, int height){
    this->width=width;
    this->height=height;

	yaw=81.2;
	pitch=-1.2;
	glm::vec3 front;
  	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  	front.y = sin(glm::radians(pitch));
  	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  	cameraFront = glm::normalize(front);
	cameraPos={-0.828088,0.0471648,-6.07124};
	cameraUp = {0.0f,0.0f,-1.0f};
	  /*
	cameraPos={1.0f,0.0f,-1.0f};
    cameraFront={-1.0f,0.0f,1.0f};
    cameraUp = {0.0f,0.0f,-1.0f};
	*/
	firstMouse=true;


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

void Engine::mouse_callback(GLFWwindow* window,double xpos, double ypos){
	if (isMouseLeftBtnPressed) {
		if (firstMouse) {
    	lastX = xpos;
    	lastY = ypos;
    	firstMouse = false;
  		}

  		double xoffset = xpos - lastX;
  		double yoffset = lastY - ypos; // 翻转Y轴
  		lastX = xpos;
  		lastY = ypos;

  		float sensitivity = 0.1f; // 鼠标灵敏度
  		xoffset *= sensitivity;
  		yoffset *= sensitivity;

  		yaw += xoffset;
  		pitch += yoffset;

  		// 限制俯仰角的范围在[-89, 89]度之间
  		if (pitch > 89.0f) {
    		pitch = 89.0f;
  		}
  		if (pitch < -89.0f) {
    		pitch = -89.0f;
  		}

  		glm::vec3 front;
  		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  		front.y = sin(glm::radians(pitch));
  		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  		cameraFront = glm::normalize(front);
	}
	else{
		firstMouse = true;
	}
}

void Engine::scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
	scrollOffset = yoffset;
    isScrollChanged = true;
}
void Engine::mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        isMouseLeftBtnPressed=true;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        isMouseLeftBtnPressed = false;
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
        isMouseMiddleBtnPressed = true;
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
        isMouseMiddleBtnPressed = false;
}
void Engine::scroll_process(){
	if (isScrollChanged) {
        const float cameraSpeed = 100.0f * camera.getDeltaTime(glfwGetTime()); // adjust accordingly
        camera.fov -= (float)scrollOffset * cameraSpeed;
        if (camera.fov < 1.0f)
            camera.fov = 1.0f;
        if (camera.fov > 45.0f)
            camera.fov = 45.0f;
        isScrollChanged = false;
        //ubo.frameCount = 1; //camera moved
    }
}


void Engine::run(){

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

		{
			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);

			glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_CURSOR_DISABLED);
            //glfwSetCursorPosCallback(window, (GLFWcursorposfun)mouse_callback);
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            mouse_callback(window, xpos, ypos);
            glfwSetScrollCallback(window, scroll_callback);
            glfwSetMouseButtonCallback(window, mouse_button_callback);
            scroll_process();

            const float cameraSpeed = 1.0f * camera.getDeltaTime(glfwGetTime()); // adjust accordingly
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                //camera.pos += cameraSpeed * camera.front;
				cameraPos += cameraSpeed * cameraFront;
                //ubo.frameCount = 1; //camera moved
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                //camera.pos -= cameraSpeed * camera.front;
				cameraPos -= cameraSpeed * cameraFront;
                //ubo.frameCount = 1; //camera moved
            }

            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                //camera.pos -= glm::normalize(glm::cross(camera.front, camera.up)) * cameraSpeed;
				cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
                //ubo.frameCount = 1; //camera moved
            }

            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                //camera.pos += glm::normalize(glm::cross(camera.front, camera.up)) * cameraSpeed;
				cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
                //ubo.frameCount = 1; //camera moved
            }

			// for print debug info
			if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
				std::cout<<"cameraPos: "<<cameraPos.x<<" "<<cameraPos.y<<" "<<cameraPos.z<<" \n"
				 		 <<"cameraFront: "<<cameraFront.x<<" "<<cameraFront.y<<" "<<cameraFront.z<<" \n"
				 		 <<"cameraUp: "<<cameraUp.x<<" "<<cameraUp.y<<" "<<cameraUp.z<<" \n";

				std::cout<<"yaw: "<<yaw<<" \n"<<"pitch: "<<pitch<<" \n";
			}
		}

        render();
        calculateFrameRate();
        
    }
}

void Engine::calculateFrameRate(){
	if(timing.lastTime==-1){
		timing.lastTime = glfwGetTime();
		return;
	}
	timing.currentTime = glfwGetTime();
	double delta = timing.currentTime - timing.lastTime;

	if (delta >= 1) {
		int framerate{ std::max(1, int(timing.numFrames / delta)) };
		std::stringstream title;
		title <<"Vulkan Parallel rendering: " << framerate << " fps.";
		
		glfwSetWindowTitle(window, title.str().c_str());
		timing.lastTime = timing.currentTime;
		timing.numFrames = -1;
		timing.frameTime = float(1000.0 / framerate);
	}

	++timing.numFrames;
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
	for(auto& res:renderThreadResources){
		res.physicalDevice = physicalDevice;
		res.device = device;
		res.graphicsQueue = queues[0];
		res.presentQueue = queues[1];
	}
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

void Engine::recreate_swapchain(){
	width=0;
	height=0;
	while(width==0 || height==0){
		glfwGetFramebufferSize(window,&width,&height);
		glfwWaitEvents();
	}
	
	device.waitIdle();

	cleanup_swapchain();
	create_swapchain();
	create_framebuffers();
	create_frame_resources();
	for(auto& res:renderThreadResources){
		vkInit::commandBufferInputChunk commandBufferInput = { device, res.commandPool, res.swapchainFrames };
		vkInit::make_frame_command_buffers(commandBufferInput);
	}
	
}

void Engine::cleanup_swapchain(){
	for (vkUtil::SwapChainFrame& frame : swapchainFrames) {
		frame.destroy();
	}

	device.destroySwapchainKHR(swapchain);

	for(auto& res:renderThreadResources){
		device.destroyDescriptorPool(res.frameDescriptorPool);
	}
}

void Engine::create_descriptor_set_layouts(){

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

	// for main thread
	frameSetLayout=vkInit::make_descriptor_set_layout(device,bindings);

	// for child theread
    for(auto i=0;i<NUM_THREADS;i++){
		
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

	// for main thread
	specification.descriptorSetLayouts = { frameSetLayout /* , meshSetLayout */ };
        vkInit::GraphicsPipelineOutBundle output = vkInit::create_graphics_pipeline(
		specification
	    );

	pipelineLayout = output.layout;
	renderpass = output.renderpass;
	pipeline = output.pipeline;

	// for child thread
	specification.isChildThread=true;
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
	// for main thread
	vkInit::framebufferInput frameBufferInput;
	frameBufferInput.device = device;
	frameBufferInput.renderpass = renderpass;
	frameBufferInput.swapchainExtent = swapchainExtent;
	vkInit::make_framebuffers(frameBufferInput, swapchainFrames);

	// for child thread
    for(auto& res:renderThreadResources){
        vkInit::framebufferInput frameBufferInput;
	    frameBufferInput.device = device;
	    frameBufferInput.renderpass = res.renderpass;
	    frameBufferInput.swapchainExtent = res.swapchainExtent;
	    vkInit::make_framebuffers(frameBufferInput, res.swapchainFrames);
    }
}

void Engine::create_commandbuffer(){
	//for main thread
	commandPool = vkInit::make_command_pool(device, physicalDevice, surface);
	vkInit::commandBufferInputChunk commandBufferInput = { device, commandPool, swapchainFrames };
	mainCommandBuffer = vkInit::make_command_buffer(commandBufferInput);
	vkInit::make_frame_command_buffers(commandBufferInput);

	// for child thread
	for(auto& res:renderThreadResources){
		res.commandPool = vkInit::make_command_pool(device, physicalDevice, surface);
		vkInit::commandBufferInputChunk commandBufferInput = { device, res.commandPool, res.swapchainFrames };
		res.threadCommandBuffer = vkInit::make_command_buffer(commandBufferInput);
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


	// for main thread
	frameDescriptorPool = vkInit::make_descriptor_pool(device,static_cast<uint32_t>(swapchainFrames.size()),bindings);
	// for main thread swapchainFrames
	for(vkUtil::SwapChainFrame& frame: swapchainFrames){
		frame.inFlight = vkInit::make_fence(device);
		for(auto i = 0; i<NUM_THREADS;++i){
			frame.inFlights.push_back(vkInit::make_fence(device));
		}
		frame.imageAvailable = vkInit::make_semaphore(device);
		frame.renderFinished = vkInit::make_semaphore(device);
		for(auto i =0;i<NUM_THREADS;i++){
			frame.renderFinisheds.push_back(vkInit::make_semaphore(device));
		}

		frame.make_descriptor_resources();

		frame.descriptorSet = vkInit::allocate_descriptor_set(device,frameDescriptorPool,frameSetLayout);
	}

	// for child thread
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

	for(auto& vertex:vertices){  // differt location of the mesh for different threads
			vertex = {vertex.pos-glm::vec3(2.5f, 1.0f, 0.0f),vertex.normal};
		}
	for(auto i=0;i<4;++i){
		for(auto& vertex:vertices){  // differt location of the mesh for different threads
			vertex = {vertex.pos+glm::vec3(2.5f, 1.0f, 0.0f),vertex.normal};
		}
		

		vkMesh::Mesh mesh;
		mesh.vertices=vertices;
		mesh.indices=indices;

		meshs.push_back(mesh);
	}

	
	int index=0;
	for(auto& res:renderThreadResources){
		
		res.vertices=meshs[index].vertices;
		res.indices=meshs[index].indices;
		index++;
	}
	

	if(NUM_THREADS==1){
		vkMesh::Mesh mesh;
		mesh.vertices=meshs[0].vertices;
		mesh.indices=meshs[0].indices;
		for(auto i =1;i<meshs.size();++i){
			mesh.merge(meshs[i]);
		}
		renderThreadResources[0].vertices=mesh.vertices;
		renderThreadResources[0].indices=mesh.indices;
	}
	else if(NUM_THREADS==2){
		vkMesh::Mesh mesh;
		mesh.vertices=meshs[0].vertices;
		mesh.indices=meshs[0].indices;
		int num = meshs.size()/2;
		for(auto i =1;i<num;++i){
			mesh.merge(meshs[i]);
		}
		renderThreadResources[0].vertices=mesh.vertices;
		renderThreadResources[0].indices=mesh.indices;

		vkMesh::Mesh mesh2;
		mesh2.vertices=meshs[num].vertices;
		mesh2.indices=meshs[num].indices;
		for(auto i =1;i<meshs.size();++i){
			mesh2.merge(meshs[i]);
		}
		renderThreadResources[1].vertices=mesh2.vertices;
		renderThreadResources[1].indices=mesh2.indices;
	}
	else if(NUM_THREADS==3){
		vkMesh::Mesh mesh;
		mesh.vertices=meshs[0].vertices;
		mesh.indices=meshs[0].indices;
		int num = meshs.size()/3;
		for(auto i =1;i<num;++i){
			mesh.merge(meshs[i]);
		}
		renderThreadResources[0].vertices=mesh.vertices;
		renderThreadResources[0].indices=mesh.indices;

		vkMesh::Mesh mesh2;
		mesh2.vertices=meshs[num].vertices;
		mesh2.indices=meshs[num].indices;
		int num2=2*meshs.size()/3;
		for(auto i =1;i<num2;++i){
			mesh2.merge(meshs[i]);
		}
		renderThreadResources[1].vertices=mesh2.vertices;
		renderThreadResources[1].indices=mesh2.indices;

		vkMesh::Mesh mesh3;
		mesh3.vertices=meshs[num2].vertices;
		mesh3.indices=meshs[num2].indices;
		for(auto i =1;i<meshs.size();++i){
			mesh3.merge(meshs[i]);
		}
		renderThreadResources[2].vertices=mesh3.vertices;
		renderThreadResources[2].indices=mesh3.indices;
	}
	else{

	}

	
}

void Engine::create_vertexbuffer(){
	for(auto& res:renderThreadResources){
		FinalizationChunk finalizationChunk;
		finalizationChunk.logicalDevice = device;
		finalizationChunk.physicalDevice=physicalDevice;
		finalizationChunk.queue=graphicsQueue;
		finalizationChunk.commandBuffer=res.threadCommandBuffer;

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
		finalizationChunk.commandBuffer=res.threadCommandBuffer;

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

void Engine::update_frame(int imageIndex){
	vkUtil::SwapChainFrame _frame = swapchainFrames[imageIndex];

	vkUtil::UBO ubo = {};
  	ubo.model = glm::mat4(1.0f);
  	ubo.view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
  	ubo.projection = glm::perspective(glm::radians(45.0f),
      swapchainExtent.width / (float) swapchainExtent.height, 0.1f, 10.0f);
  	ubo.projection[1][1] *= -1; // 翻转Y轴坐标系以匹配Vulkan

  	_frame.cameraData.model = ubo.model;
	_frame.cameraData.view = ubo.view;
	_frame.cameraData.projection = ubo.projection;
	memcpy(_frame.cameraDataWriteLocation, &(_frame.cameraData), sizeof(vkUtil::UBO));

	swapchainFrames[imageIndex].write_descriptor_set();
	
	for(auto& res:renderThreadResources){
		memcpy(res.swapchainFrames[imageIndex].cameraDataWriteLocation, &(_frame.cameraData), sizeof(vkUtil::UBO));

		res.swapchainFrames[imageIndex].write_descriptor_set();
	}
	
}

void Engine::render(){
	int frameIndex=frameNumber_atomic.load();

	//device.waitForFences(1, &swapchainFrames[frameIndex].inFlight, VK_TRUE, UINT64_MAX);  // for single thread rendering
	device.waitForFences(swapchainFrames[frameIndex].inFlights.size(), swapchainFrames[frameIndex].inFlights.data(), VK_TRUE, UINT64_MAX);    // for multi thread rendering
	uint32_t imageIndex;
	//vk::ResultValue<uint32_t> acquire;
	try{
		vk::ResultValue acquire = device.acquireNextImageKHR(swapchain, UINT64_MAX, swapchainFrames[frameIndex].imageAvailable, nullptr);
		imageIndex=acquire.value;
	} catch(vk::OutOfDateKHRError){
		recreate_swapchain();
		return;
	}
	

	vk::CommandBuffer commandBuffer = swapchainFrames[imageIndex].commandBuffer;

	
#ifdef VK_MAKE_VERSION
	VULKAN_HPP_NAMESPACE::CommandBufferResetFlags flags={};
	VULKAN_HPP_NAMESPACE::DispatchLoaderStatic d VULKAN_HPP_DEFAULT_DISPATCHER_ASSIGNMENT ;
	commandBuffer.reset(flags,d);
#else 
	commandBuffer.reset();
#endif

	update_frame(imageIndex);

	vk::CommandBufferBeginInfo beginInfo = {};
	try {
		commandBuffer.begin(beginInfo);
	}
	catch (vk::SystemError err) {
		#ifdef DEBUG_MESSAGE
			std::cout << "Failed to begin recording command buffer!" << std::endl;
		#endif
	}
	
	vk::RenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.renderPass = renderpass;
	renderPassInfo.framebuffer = swapchainFrames[imageIndex].framebuffer;
	renderPassInfo.renderArea.offset.x = 0;
	renderPassInfo.renderArea.offset.y = 0;
	renderPassInfo.renderArea.extent = swapchainExtent;

	vk::ClearValue colorClear;
	std::array<float, 4> colors = { 1.0f, 0.5f, 0.25f, 1.0f };
	colorClear.color = vk::ClearColorValue(colors);
	vk::ClearValue depthClear;

	#ifdef VK_MAKE_VERSION
	depthClear.depthStencil = vk::ClearDepthStencilValue({ 1.0f },{0});
#else
	depthClear.depthStencil = vk::ClearDepthStencilValue({ 1.0f,0 });
#endif
	std::vector<vk::ClearValue> clearValues = { {colorClear,depthClear} };

	renderPassInfo.clearValueCount = clearValues.size();
	renderPassInfo.pClearValues = clearValues.data();

	commandBuffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);
	commandBuffer.endRenderPass();

	try {
		commandBuffer.end();
	}
	catch (vk::SystemError err) {
		
		#ifdef DEBUG_MESSAGE
			std::cout << "failed to record command buffer!" << std::endl;
		#endif
	}

	vk::SubmitInfo submitInfo = {};

	vk::Semaphore waitSemaphores[] = { swapchainFrames[frameIndex].imageAvailable };
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vk::Semaphore signalSemaphores[] = { swapchainFrames[frameIndex].renderFinished };
	//submitInfo.signalSemaphoreCount = 1;
	//submitInfo.pSignalSemaphores = signalSemaphores;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;

	//device.resetFences(1, &swapchainFrames[frameIndex].inFlight);

	try {
		graphicsQueue.submit(submitInfo, nullptr); 
	}
	catch (vk::SystemError err) {
		
		#ifdef DEBUG_MESSAGE
			std::cout << "failed to submit draw command buffer!" << std::endl;
		#endif
	}

	std::vector<std::thread> threads;
    for(auto i =0;i<NUM_THREADS;i++){
	//for(auto i =0;i<1;i++){
            threads.push_back(std::thread(
				thread_record_draw_commands,
				window,
				instance,surface,renderThreadResources[i],i,imageIndex,
				swapchainFrames[imageIndex].inFlight,
				swapchainFrames[frameIndex].imageAvailable,
				swapchainFrames[frameIndex].renderFinished,
				swapchainFrames[frameIndex].renderFinisheds,
				swapchainFrames[frameIndex].inFlights
				));
    }

    for(auto& thread : threads){
        thread.join();
        //thread.detach();
    }

	

	//device.resetFences(1, &swapchainFrames[frameIndex].inFlight);
	/*
	try {
		graphicsQueue.submit(submitInfo, swapchainFrames[frameIndex].inFlight); 
	}
	catch (vk::SystemError err) {
		
		#ifdef DEBUG_MESSAGE
			std::cout << "failed to submit draw command buffer!" << std::endl;
		#endif
	}
	*/
	

	vk::PresentInfoKHR presentInfo = {};
	//presentInfo.waitSemaphoreCount = 1;
	//presentInfo.pWaitSemaphores = signalSemaphores;
	presentInfo.waitSemaphoreCount = swapchainFrames[frameIndex].renderFinisheds.size();
	presentInfo.pWaitSemaphores = swapchainFrames[frameIndex].renderFinisheds.data();

	vk::SwapchainKHR swapChains[] = { swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	vk::Result present;
	
	try{
		present=presentQueue.presentKHR(presentInfo);
	}
	catch(vk::OutOfDateKHRError error){
		present = vk::Result::eErrorOutOfDateKHR;
	}
	

	if(present==vk::Result::eErrorOutOfDateKHR || present==vk::Result::eSuboptimalKHR){
		std::cout<<"Recreate" << std::endl;
		recreate_swapchain();
		return;
	}

	frameNumber=(frameNumber+1) % maxFramesInFlight;
	frameNumber_atomic.store((frameIndex+1)% maxFramesInFlight);

	//device.waitIdle();
}

std::mutex Engine::graphicQueueMutex;

void Engine::thread_record_draw_commands(
		GLFWwindow* window,vk::Instance instance,vk::SurfaceKHR surface,
		RenderThreadResource res,int index,int imageIndex,vk::Fence inFlight,
		vk::Semaphore imageAvailable,vk::Semaphore renderFinished,
		std::vector<vk::Semaphore> renderFinisheds,
		std::vector<vk::Fence> inFlights
		){
    //std::unique_lock<std::mutex> lock(instanceMutex,std::defer_lock);
    //while(!lock.try_lock()){
    //    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    //}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		std::cout<<"thread "<<index<<" is running\n";
	}
    //vk::Device pDevice=vkInit::create_logical_device(physicalDevice,surface);
    //std::cout<<"thread "<<index<<" is using physicalDevice\n";

    //lock.unlock();

	vk::CommandBuffer commandBuffer = res.swapchainFrames[imageIndex].commandBuffer;

	
#ifdef VK_MAKE_VERSION
	VULKAN_HPP_NAMESPACE::CommandBufferResetFlags flags={};
	VULKAN_HPP_NAMESPACE::DispatchLoaderStatic d VULKAN_HPP_DEFAULT_DISPATCHER_ASSIGNMENT ;
	commandBuffer.reset(flags,d);
#else 
	commandBuffer.reset();
#endif

	vk::CommandBufferBeginInfo beginInfo = {};
	try {
		commandBuffer.begin(beginInfo);
	}
	catch (vk::SystemError err) {
		#ifdef DEBUG_MESSAGE
			std::cout << "Failed to begin recording command buffer!" << std::endl;
		#endif
	}

	vk::RenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.renderPass = res.renderpass;
	renderPassInfo.framebuffer = res.swapchainFrames[imageIndex].framebuffer;
	renderPassInfo.renderArea.offset.x = 0;
	renderPassInfo.renderArea.offset.y = 0;
	renderPassInfo.renderArea.extent = res.swapchainExtent;

	vk::ClearValue colorClear;
	std::array<float, 4> colors = { 1.0f, 0.5f, 0.25f, 1.0f };
	colorClear.color = vk::ClearColorValue(colors);
	vk::ClearValue depthClear;

	#ifdef VK_MAKE_VERSION
	depthClear.depthStencil = vk::ClearDepthStencilValue({ 1.0f },{0});
#else
	depthClear.depthStencil = vk::ClearDepthStencilValue({ 1.0f,0 });
#endif
	std::vector<vk::ClearValue> clearValues = { {colorClear, depthClear} };

	renderPassInfo.clearValueCount = 0;
	renderPassInfo.pClearValues = nullptr;
	//renderPassInfo.pClearValues = clearValues.data();

	commandBuffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);

	commandBuffer.bindDescriptorSets(
		vk::PipelineBindPoint::eGraphics,
		res.pipelineLayout,0,res.swapchainFrames[imageIndex].descriptorSet,nullptr);


	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, res.pipeline);

	vk::Buffer vertexBuffers[] = { res.vertexBuffer.buffer };
	vk::DeviceSize offsets[] = {0};
	commandBuffer.bindVertexBuffers(0,1,vertexBuffers,offsets);
	commandBuffer.bindIndexBuffer(res.indexBuffer.buffer, 0, vk::IndexType::eUint32);

	//todo: descriptor set for textures
	//commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, res.pipelineLayout, 1, res.descriptorSet, nullptr);

	int firstIndex=0;
	int startInstance=0;
	commandBuffer.drawIndexed(res.indices.size(), 1, firstIndex, 0, startInstance);
	
	commandBuffer.endRenderPass();

	try {
		commandBuffer.end();
	}
	catch (vk::SystemError err) {
		
		#ifdef DEBUG_MESSAGE
			std::cout << "failed to record command buffer!" << std::endl;
		#endif
	}

	vk::SubmitInfo submitInfo = {};

	vk::Semaphore waitSemaphores[] = { imageAvailable };
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	//submitInfo.waitSemaphoreCount = 1;
	//submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vk::Semaphore signalSemaphores[] = { renderFinished };
	//submitInfo.signalSemaphoreCount = 1;
	//submitInfo.pSignalSemaphores = signalSemaphores;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &renderFinisheds[index];

	res.device.resetFences(1, &inFlights[index]);
	
	//***********syn on threads************************
	std::unique_lock<std::mutex> lock(graphicQueueMutex,std::defer_lock);
    while(!lock.try_lock()){
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
	
	try {
		res.graphicsQueue.submit(submitInfo, inFlights[index]); 
	}
	catch (vk::SystemError err) {
		
		#ifdef DEBUG_MESSAGE
			std::cout << "failed to submit draw command buffer!" << std::endl;
		#endif
	}

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
    	std::cout<<"thread "<<index<<" is using graphicQueue\n";
	}
	

    lock.unlock();
	//***********end************************
}

vk::PhysicalDevice Engine::physicalDevice=nullptr;