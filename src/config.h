#pragma once
//statically load vulkan library
#include <vulkan/vulkan.hpp>
/*
* Statically linking the prebuilt header from the lunarg sdk will load
* most functions, but not all.
*
* Functions can also be dynamically loaded, using the call
*
* PFN_vkVoidFunction vkGetInstanceProcAddr(
	VkInstance                                  instance,
	const char*                                 pName);
 or
 PFN_vkVoidFunction vkGetDeviceProcAddr(
	VkDevice                                    device,
	const char*                                 pName);
	We will look at this later, once we've created an instance and device.
*/
#include <GLFW/glfw3.h>

#include <iostream>
#include<vector>
#include <set>
#include <string>
#include<optional>
#include<fstream>
#include<sstream>
#include<unordered_map>
#include<thread>
#include<atomic>
#include "vkMacro.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

/**
	Data structures used for creating buffers
	and allocating memory
*/
struct BufferInputChunk {
	size_t size;
	vk::BufferUsageFlags usage;
	vk::Device logicalDevice;
	vk::PhysicalDevice physicalDevice;
	vk::MemoryPropertyFlags memoryProperties;
};

/**
	holds a vulkan buffer and memory allocation
*/
struct Buffer {
	vk::Buffer buffer;
	vk::DeviceMemory bufferMemory;
};

//--------- Assets -------------//
enum class meshTypes {
	TRIANGLE,
	SQUARE,
	STAR
};