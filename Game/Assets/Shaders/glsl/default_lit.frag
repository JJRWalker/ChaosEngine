//glsl version 4.5
#version 450

//shader input
layout (location = 0) in vec3 inColor;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inFragPos;
layout (location = 3) in vec2 inTexCoord;
layout (location = 4) in ObjectData{
	mat4 model;
	mat4 shaderFloatArray1;
	mat4 customFloatArray1;
} inObjData; 

//output write
layout (location = 0) out vec4 outFragColor;

layout(set = 0, binding = 1) uniform  SceneData{   
    vec4 fogColor; // w is for exponent
	vec4 fogDistances; //x for min, y for max, zw unused.
	vec4 ambientColor;
	vec4 sunlightDirection; //w for sun power
	vec4 sunlightColor;
} sceneData;

struct LightingData
{
	mat4 transform;
	mat4 shaderFloatArray1;
};

layout(std140, set = 3, binding = 0) readonly buffer LightingBuffer
{
	LightingData lights[];
} lightingBuffer;

void main()
{	
	vec3 ambient = sceneData.ambientColor.xyz * sceneData.ambientColor.w;
	vec3 norm = normalize(inNormal);
	vec3 lightColour = sceneData.sunlightColor.xyz * sceneData.sunlightColor.w;
	vec3 lightDir = normalize(sceneData.sunlightDirection.xyz);
	float diffuse = max(dot(norm, lightDir), 0.0f) * sceneData.sunlightDirection.w;
	vec3 diffColour = diffuse * lightColour;
	vec3 result = vec3((ambient + diffColour) * inColor);
	outFragColor = vec4(result, 1.0f);
}