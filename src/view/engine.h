#pragma once
#include "../config.h"
#include "logging.h"
#include "vkInit/instance.h"

const uint32_t NUM_THREADS = 4;

class Engine{
public:
    Engine(int width, int height);

	~Engine();

    GLFWwindow* window;
    void build_glfw_window(int width, int height);
    void run();

    //instance-related variables
    vk::Instance instance{nullptr};
	vk::DebugUtilsMessengerEXT debugMessenger{ nullptr };
	vk::DispatchLoaderDynamic dldi;
	vk::SurfaceKHR surface;
    void create_instance();

    static std::mutex instanceMutex;

    static void threadFunc(vk::Instance instance,int index);
};