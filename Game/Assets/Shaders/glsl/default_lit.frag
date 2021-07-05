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
	vec3 ambient = sceneData.ambientColor.xyz * sceneData.ambientColor.w;
	vec3 norm = normalize(inNormal);
	vec3 lightColour = sceneData.sunlightColor.xyz * sceneData.sunlightColor.w;
	vec3 lightDir = normalize(sceneData.sunlightDirection.xyz);
	float diffuse = max(dot(norm, lightDir), 0.0f) * sceneData.sunlightDirection.w;
	vec3 diffColour = diffuse * lightColour;
	vec4 result = vec4((ambient + diffColour) * inColor, 1.0f);

	for (int i = 0; i < sceneData.lightingData.x; i++)
	{
		if (lightingBuffer.lights[i].shaderFloatArray1[3][3] < 1.0f) continue;

		LightingData light = lightingBuffer.lights[i];
		vec3 lightPos = vec3(light.transform[3][0], light.transform[3][1], light.transform[3][2]);

		if (inFragPos.z > lightPos.z) continue;

		float dist = distance(inFragPos, lightPos);
		float radius = light.shaderFloatArray1[1][0];

		if (dist > radius) continue;

		float attenuation = clamp(1.0 - dist * dist / (radius * radius), 0.0f, 1.0f);

		attenuation *= attenuation;

		vec3 lightColour = vec3(light.shaderFloatArray1[0][0], light.shaderFloatArray1[0][1], light.shaderFloatArray1[0][2]) * light.shaderFloatArray1[0][3];

		result += vec4(lightColour.xyz * attenuation, 1.0f);
	}

	outFragColor = vec4(result);
}