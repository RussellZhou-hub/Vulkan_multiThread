#include "engine.h"

Engine::Engine(int width, int height){
    build_glfw_window(width, height);
    create_instance();
}

Engine::~Engine(){

}

void Engine::build_glfw_window(int width, int height){
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
            threads.push_back(std::thread(threadFunc,instance,i));
        }

        for(auto& thread : threads){
            thread.join();
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

std::mutex Engine::instanceMutex;

void Engine::threadFunc(vk::Instance instance,int index){
    std::unique_lock<std::mutex> lock(instanceMutex,std::defer_lock);
    while(!lock.try_lock()){
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    vk::Instance instance2=instance;
    std::cout<<"thread "<<index<<" is using instance\n";

    lock.unlock();
}