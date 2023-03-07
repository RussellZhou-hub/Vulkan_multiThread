#pragma once
#include "../../config.h"
#include "../logging.h"

//namespace for creation functions/definitions etc.
namespace vkInit {
    vk::Instance create_instance(const char* applicationName);
    bool supported(std::vector<const char*>& extensions, std::vector<const char*>& layers);
}