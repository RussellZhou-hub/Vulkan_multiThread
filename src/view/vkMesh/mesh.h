#pragma once
#include "../../config.h"
#include <gtx/hash.hpp>

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

        bool operator==(const Vertex& other) const {
            return pos == other.pos && normal==other.normal;
            //return pos == other.pos && color == other.color && normal==other.normal && texCoord == other.texCoord;
        }
    };

    class Mesh{
    public:
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        Buffer vertexBuffer;
	    Buffer indexBuffer;
        void merge(Mesh& inputMesh);
    };

}

namespace std {
    template<> struct hash<vkMesh::Vertex> {
        size_t operator()(vkMesh::Vertex const& vertex) const {
            return (hash<glm::vec3>()(vertex.pos) ^ hash<glm::vec3>()(vertex.normal) );
            //return ((hash<glm::vec3>()(vertex.pos) ^ hash<glm::vec3>()(vertex.normal) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
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