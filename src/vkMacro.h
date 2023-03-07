#pragma once
//statically load vulkan library
#include <vulkan/vulkan.hpp>

#ifndef VULKAN_HPP_DEFAULT_ARGUMENT
#define VULKAN_HPP_DEFAULT_ARGUMENT = {}
#define VULKAN_HPP_DEFAULT_ARGUMENT_NULLPTR_ASSIGNMENT =nullptr
#define VULKAN_HPP_DEFAULT_DISPATCHER_ASSIGNMENT   = VULKAN_HPP_DEFAULT_DISPATCHER
#define VULKAN_HPP_DEFAULT_DISPATCH_TYPE
#endif

#ifndef VK_API_VERSION_MAJOR
#define VK_API_VERSION_MAJOR VK_VERSION_MAJOR
#endif

#ifndef VK_API_VERSION_MINOR
#define VK_API_VERSION_MINOR VK_VERSION_MINOR
#endif

#ifndef VK_API_VERSION_PATCH
#define VK_API_VERSION_PATCH VK_VERSION_PATCH
#endif

#ifndef VK_API_VERSION_VARIANT
#define VK_API_VERSION_VARIANT(version) ((uint32_t)(version) >> 29)
#endif

#ifndef VK_MAKE_API_VERSION
#define VK_MAKE_API_VERSION(variant, major, minor, patch) \
    ((((uint32_t)(variant)) << 29) | (((uint32_t)(major)) << 22) | (((uint32_t)(minor)) << 12) | ((uint32_t)(patch)))
#endif

#ifdef VK_MAKE_VERSION

//VULKAN_HPP_NAMESPACE{
//
//}

#endif

