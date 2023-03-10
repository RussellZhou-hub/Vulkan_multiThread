#pragma once

#include <vulkan/vulkan.h>
#include<string>
#include <functional>
#include <deque>

class DeletionQueue
{
public:
    std::deque<std::function<void()>> deletors;

    void push_function(std::function<void()>&& function);

    void flush();
};

/*
_mainDeletionQueue.push_function([=]() {
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroyBuffer(device, uniformBuffers[i], nullptr);
            vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
        }
    });
*/