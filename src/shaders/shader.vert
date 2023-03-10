#version 450

// vulkan NDC:	x: -1(left), 1(right)
//				y: -1(top), 1(bottom)

layout(set=0,binding=0) uniform UBO {
	mat4 model;
	mat4 view;
	mat4 projection;
} ubo;

layout(location = 0 ) in vec3 inPosition;
layout(location = 1 ) in vec3 inNormal;
//layout(location = 2 ) in vec2 vertexTexCoord;

/*
layout (push_constant) uniform constants{
	mat4 model;
} ObjectData;
*/


layout(location = 0) out vec3 fragPosition;
layout(location = 1 ) out vec3 fragNormal;

void main() {
	gl_Position = ubo.projection * ubo.view* ubo.model*vec4(inPosition,1.0);
	fragPosition=vec3(ubo.model*vec4(inPosition,1.0));
	fragNormal=mat3(transpose(inverse(ubo.model)))*inNormal;
}