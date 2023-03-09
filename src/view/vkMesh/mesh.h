#pragma once
#include "../../config.h"

namespace vkMesh{
    /**
		\returns the input binding description for a (vec2 pos, vec3 color) vertex format.
	*/
	vk::VertexInputBindingDescription getVertexBindingDescription();

	/**
		\returns the input attribute descriptions for a (vec2 pos, vec3 color) vertex format.
	*/
	std::vector<vk::VertexInputAttributeDescription> getVertexAttributeDescriptions();

    class  Vertex{
    public:
        glm::vec3 pos;
        glm::vec3 normal;
    };

}


/*
struct QuadArealignt {
    alignas(4) glm::vec4 A;  //    A* * *B
    alignas(4) glm::vec4 B;  //    *     *
    alignas(4) glm::vec4 C;  //    *     *
    alignas(4) glm::vec4 D;  //    C* * *D
};

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(16) glm::mat4 prev_Proj_View;
    alignas(16) QuadArealignt quadArealignt;
    alignas(4) glm::vec4 cameraPos;
    alignas(4) uint32_t frameCount;
    alignas(4) uint32_t mode;  //denoising algorithm
    alignas(4) uint32_t samples;  //sampling rate for GT
};

*/