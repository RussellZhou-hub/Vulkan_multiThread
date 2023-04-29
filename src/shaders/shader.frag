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
	Light light;
	//light.position = vec3(-0.4,-1.64,-5.8);  // for multiple geometry scene
	light.position = vec3(0.185334,1.80024,1.60735);  // for cornell scene
	light.color = vec3(1.0,0.955,0.825);
	
	vec3 lightDir = normalize(light.position - fragPosition);
    float diffuse = max(dot(fragNormal, lightDir), 0.0);
  	vec3 color = light.color * diffuse;
  	outColor = vec4(color, 1.0);
	
	//outColor = vec4(0.5,0.0,0.0, 1.0);
	//outColor = vec4(fragColor, 1.0)*texture(material,fragTexCoord);
}