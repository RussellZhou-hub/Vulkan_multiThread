#include "engine.h"

Engine::Engine(int width, int height){
    build_glfw_window(width, height);
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
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);
    }
}