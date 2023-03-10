#version 450

struct Light{
	vec3 position;
	vec3 color;
};

//layout(set=0,binding=1) uniform Lights{
//	Light light;
//} lights;

layout(location = 0) in vec3 fragPosition;
layout(location = 1 ) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

//layout(set=1,binding=0) uniform sampler2D material;

void main() {
	/*
	vec3 lightDir = normalize(lights.position - fragPosition);
    float diffuse = max(dot(fragNormal, lightDir), 0.0);
  	vec3 color = lights.color * diffuse;
  	outColor = vec4(color, 1.0);
	*/
	outColor = vec4(0.5,0.0,0.0, 1.0);
	//outColor = vec4(fragColor, 1.0)*texture(material,fragTexCoord);
}