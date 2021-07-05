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
	vec4 lightingData;
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
	vec4 shaderColour = vec4(inObjData.shaderFloatArray1[0][0], inObjData.shaderFloatArray1[0][1], inObjData.shaderFloatArray1[0][2], inObjData.shaderFloatArray1[0][3]);
	outFragColor = vec4(inColor, 1.0f) * shaderColour;
}