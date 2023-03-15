#pragma once
#include "../config.h"
#include "logging.h"
#include "garbage_collection.h"
#include "vkInit/instance.h"
#include "vkInit/device.h"
#include "RenderThreadResource.h"
#include "vkInit/swapchain.h"
#include "vkUtil/frame.h"
#include "vkInit/descriptors.h"
#include "vkInit/pipeline.h"
#include"vkInit/framebuffer.h"
#include"vkInit/commands.h"
#include"vkInit/sync.h"
#include"vkUtil/memory.h"
#include"vkMesh/mesh.h"
#include"camera.h"
#include"timing.h"
#include"vkUtil/thread_pool.h"

const uint32_t NUM_THREADS = 3;
const uint32_t NUM_MESH = 32;

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

    //pipeline-related variables
	vk::PipelineLayout pipelineLayout;
	vk::RenderPass renderpass;
	vk::Pipeline pipeline;
    
    //descriptor-related variables
	vk::DescriptorSetLayout frameSetLayout;
	vk::DescriptorPool frameDescriptorPool; //Descriptors bound on a "per frame" basis

	vk::SwapchainKHR swapchain{ nullptr };
	std::vector<vkUtil::SwapChainFrame> swapchainFrames;
	vk::Format swapchainFormat;
	vk::Extent2D swapchainExtent;
    void create_swapchain();
    void recreate_swapchain();
    void cleanup_swapchain();

    //Command-related variables
	vk::CommandPool commandPool;
	vk::CommandBuffer mainCommandBuffer;

    RenderThreadResource renderThreadResources[NUM_MESH];

    void create_descriptor_set_layouts();
    void create_pipeline();
    void create_framebuffers();
    void create_commandbuffer();
    void create_frame_resources();
    void load_assets();
    void create_vertexbuffer();
	void create_indexbuffer();
    void render();

    std::vector<vkMesh::Mesh> meshes;

    void update_frame(int imageIndex);

    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    Camera camera;
    void mouse_callback(GLFWwindow* window,double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    void scroll_process();
    bool firstMouse;
    double lastX,lastY,yaw,pitch;

    static std::mutex graphicQueueMutex;

    Timing timing;
    void calculateFrameRate();

    DeletionQueue _mainDeletionQueue;

    ThreadPool pool;

    static void thread_record_draw_commands(
        vkMesh::Mesh mesh,
        GLFWwindow* window,vk::Instance instance,vk::SurfaceKHR surface,
        RenderThreadResource res,int index,int imageIndex,vk::Fence inFlight,
        vk::Semaphore imageAvailable,vk::Semaphore renderFinished,
        std::vector<vk::Semaphore> renderFinisheds,
        std::vector<vk::Fence> inFlights
    );
    static void render_meshs(
        std::vector<vkMesh::Mesh> meshs,
        RenderThreadResource res,
        int index,
        int imageIndex
    );
    static void create_vertex_index_buffer(
        vkMesh::Mesh& mesh,
        RenderThreadResource res
    );
};