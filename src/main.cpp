#include"config.h"
#include"view/engine.h"

/*
struct RenderingResourcesVulkan
{
    RenderingResourcesVulkan()
        : mainCommandBuffer(VK_NULL_HANDLE)
        , imageAvailableSemaphore(VK_NULL_HANDLE)
        , finishedRenderingSemaphore(VK_NULL_HANDLE)
        , fence(VK_NULL_HANDLE)
    {}
    VkCommandBuffer mainCommandBuffer;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore finishedRenderingSemaphore;
    VkFence fence;
    std::vector<VkCommandPool> drawingCommandPools;
    std::vector<VkCommandBuffer> commandBuffersForDrawing;
};

*/

int main(){

    std::cout<<"hello, vulkan\n";

    Engine* engine = new Engine(640,480);

    engine->run();

    delete engine;

    return 0;
}