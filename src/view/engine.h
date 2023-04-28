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
#include"vkImage/image.h"
#include"vkMesh/mesh.h"
#include"camera.h"
#include"timing.h"
#include"vkUtil/thread_pool.h"
#include"../model/tiny_obj_loader.h"
#include"vkUtil/filePath.h"

const uint32_t NUM_THREADS = 1;


#define RENDER_OBJ_MODEL
#ifdef RENDER_OBJ_MODEL
const uint32_t NUM_MESH = 393;
const int render_width = 640;
const int render_height = 480;
#else
const uint32_t NUM_MESH = 32;
const int render_width = 640;
const int render_height = 480;
#endif

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
    void loadModel();
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

    // obj model related
    std::string exePath;
    std::string model_Name;
    std::vector<vkMesh::Mesh> model_meshes;

    static std::mutex graphicQueueMutex;

    Timing timing;
    void calculateFrameRate();

    std::vector<DeletionQueue> _frame_resouce_DeletionQueues;
    void clean_frame_resources(int index);

    ThreadPool pool;

    static void thread_record_draw_commands(
        vkMesh::Mesh* mesh,
        GLFWwindow* window,vk::Instance instance,vk::SurfaceKHR surface,
        RenderThreadResource res,int index,int imageIndex,vk::Fence inFlight,
        vk::CommandBuffer* main_commandBuffer,vk::RenderPass renderPass,
        vk::Semaphore imageAvailable,vk::Semaphore renderFinished,
        std::vector<vk::Semaphore> renderFinisheds,
        std::vector<vk::Fence> inFlights,
        DeletionQueue* _thread_resources_DeletionQueue
    );
    static void create_vertex_index_buffer(
        vkMesh::Mesh& mesh,
        RenderThreadResource res
    );
};