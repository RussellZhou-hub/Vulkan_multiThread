#pragma once
#include "../config.h"
#include "logging.h"
#include "vkInit/instance.h"
#include "vkInit/device.h"
#include "RenderThreadResource.h"
#include "vkInit/swapchain.h"
#include "vkUtil/frame.h"
#include "vkInit/descriptors.h"
#include "vkInit/pipeline.h"
#include"vkInit/framebuffer.h"

const uint32_t NUM_THREADS = 4;

class Engine{
public:
    Engine(int width, int height);

	~Engine();

    int width,height;
    GLFWwindow* window;
    void build_glfw_window();
    void run();

    //Synchronization objects
	int maxFramesInFlight,frameNumber;
	std::atomic<int> frameNumber_atomic; //for multiThread rendering

    //instance-related variables
    vk::Instance instance{ nullptr };
	vk::DebugUtilsMessengerEXT debugMessenger{ nullptr };
	vk::DispatchLoaderDynamic dldi;
	vk::SurfaceKHR surface;
    void create_instance();
    void create_device();

    //device-related variable
	static vk::PhysicalDevice physicalDevice;
	vk::Device device{ nullptr };

	vk::Queue graphicsQueue{ nullptr };
	vk::Queue presentQueue{ nullptr };
	vk::SwapchainKHR swapchain{ nullptr };
	std::vector<vkUtil::SwapChainFrame> swapchainFrames;
	vk::Format swapchainFormat;
	vk::Extent2D swapchainExtent;
    void create_swapchain();

    RenderThreadResource renderThreadResources[NUM_THREADS];

    void create_descriptor_set_layouts();
    void create_pipeline();
    void create_framebuffers();

    static std::mutex instanceMutex;

    static void threadFunc(vk::Instance instance,vk::SurfaceKHR surface,int index);
};