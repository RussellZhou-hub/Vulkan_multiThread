#include "mesh.h"

using namespace vkMesh;

/**
		\returns the input binding description for a (vec3 pos, vec3 normal) vertex format.
	*/
	vk::VertexInputBindingDescription vkMesh::getVertexBindingDescription() {

		/* Provided by VK_VERSION_1_0
		typedef struct VkVertexInputBindingDescription {
			uint32_t             binding;
			uint32_t             stride;
			VkVertexInputRate    inputRate;
		} VkVertexInputBindingDescription;
		*/

		vk::VertexInputBindingDescription bindingDescription;
		bindingDescription.binding = 0;
		// position and normal
		bindingDescription.stride = sizeof(vkMesh::Vertex);
		bindingDescription.inputRate = vk::VertexInputRate::eVertex;
		
		return bindingDescription;
	}

	/**
		\returns the input attribute descriptions for a (vec2 pos, vec3 color) vertex format.
	*/
	std::vector<vk::VertexInputAttributeDescription> vkMesh::getVertexAttributeDescriptions() {

		/* Provided by VK_VERSION_1_0
		typedef struct VkVertexInputAttributeDescription {
			uint32_t    location;
			uint32_t    binding;
			VkFormat    format;
			uint32_t    offset;
		} VkVertexInputAttributeDescription;
		*/

		std::vector<vk::VertexInputAttributeDescription> attributes;
		vk::VertexInputAttributeDescription dummy;
		attributes.push_back(dummy);
		attributes.push_back(dummy);
		//attributes.push_back(dummy);

		//Pos
		attributes[0].binding = 0;
		attributes[0].location = 0;
		attributes[0].format = vk::Format::eR32G32B32Sfloat;
		attributes[0].offset = offsetof(Vertex, pos);

		//Normal
		attributes[1].binding = 0;
		attributes[1].location = 1;
		attributes[1].format = vk::Format::eR32G32B32Sfloat;
		attributes[1].offset = offsetof(Vertex, normal);

		//TexCoord
		//attributeDesc[2].binding = 0;
		//attributeDesc[2].location = 2;
		//attributeDesc[2].format = vk::Format::eR32G32Sfloat;
		//attributeDesc[2].offset = offsetof(Vertex, texCoord);

		return attributes;
	}

	void vkMesh::Mesh::merge(Mesh& inputMesh){
		int startIndex= vertices.size();
		int num=inputMesh.vertices.size();
		for(auto i=0;i<num;++i){
			vertices.push_back(inputMesh.vertices[i]);
		}
		num=inputMesh.indices.size();
		for(auto i=0;i<num;++i){
			indices.push_back(inputMesh.indices[i]+static_cast<uint32_t>(startIndex));
		}
	}