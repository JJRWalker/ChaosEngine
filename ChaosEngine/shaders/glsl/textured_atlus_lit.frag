//glsl version 4.5
#version 450

//shader input
layout (location = 0) in vec3 inColor;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inFragPos;
layout (location = 3) in vec2 inTexCoord;

// index 0 = cell coord x
// index 1 = cell coord y
// index 3 = total cells x
// index 4 = total cells y
layout (location = 4) in mat4 inOptionalData; 

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

    vec2 atlusCell = vec2(inOptionalData[0][0], inOptionalData[0][1]);
    vec2 atlusDimensions = vec2(inOptionalData[0][2], inOptionalData[0][3]);
    vec2 atlusCellSize = vec2(1.0f / atlusDimensions.x, 1.0f / atlusDimensions.y); 

    vec2 texCoord;
    texCoord.x = atlusCell.x * atlusCellSize.x + (inTexCoord.x * atlusCellSize.x);
    texCoord.y = atlusCell.y * atlusCellSize.y + (inTexCoord.y * atlusCellSize.y);

    vec4 colour = texture(tex1, texCoord);

	if (colour.a <= 0.0f)
	{
		discard;
	}

	outFragColor = vec4(colour * result);
}