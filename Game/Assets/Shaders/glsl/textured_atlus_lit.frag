//glsl version 4.5
#version 450

struct ObjectData{
	mat4 model;
	mat4 shaderFloatArray1;
	mat4 customFloatArray1;
}; 

//shader input
layout (location = 0) in vec3 inColor;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inFragPos;
layout (location = 3) in vec2 inTexCoord;

// index 0 to 3 = colour
// index 5 = cell coord x
// index 6 = cell coord y
// index 7 = total cells x
// index 8 = total cells y
// index 9 = tiling factor x
// index 10 = tiling factor y
layout (location = 4) in ObjectData inObjData; 

//output write
layout (location = 0) out vec4 outFragColor;

layout(set = 0, binding = 1) uniform  SceneData{   
    vec4 fogColor; // w is for exponent
	vec4 fogDistances; //x for min, y for max, zw unused.
	vec4 ambientColor;
	vec4 sunlightDirection; //w for sun power
	vec4 sunlightColor;
} sceneData;

layout(set = 2, binding = 0) uniform sampler2D tex1;

void main()
{	
	vec3 ambient = sceneData.ambientColor.xyz * sceneData.ambientColor.w;
	vec3 norm = normalize(inNormal);
	vec3 lightColour = sceneData.sunlightColor.xyz * sceneData.sunlightColor.w;
	vec3 lightDir = normalize(sceneData.sunlightDirection.xyz);
	float diffuse = max(dot(norm, lightDir), 0.0) * sceneData.sunlightDirection.w;
	vec3 diffColour = diffuse * lightColour;
	vec4 result = vec4((ambient + diffColour) * inColor, 1.0f);

	mat4 engineData = inObjData.shaderFloatArray1;

    vec2 atlusCell = vec2(engineData[1][0], engineData[1][1]);
    vec2 atlusDimensions = vec2(engineData[1][2], engineData[1][3]);
    vec2 atlusCellSize = vec2(1.0f / atlusDimensions.x, 1.0f / atlusDimensions.y); 
	vec2 tilingFactor = vec2(engineData[2][0], engineData[2][1]);

    vec2 texCoord;
    texCoord.x = (atlusCell.x * atlusCellSize.x + (fract(inTexCoord.x * tilingFactor.x) * atlusCellSize.x));
    texCoord.y = (atlusCell.y * atlusCellSize.y + (fract(inTexCoord.y * tilingFactor.y) * atlusCellSize.y));


    vec4 colour = texture(tex1, texCoord) * vec4(engineData[0][0], engineData[0][1], engineData[0][2], engineData[0][3]);

	if (colour.a <= 0.0f)
	{
		discard;
	}

	outFragColor = vec4(colour * result);
}