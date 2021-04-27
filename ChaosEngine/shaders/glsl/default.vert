//glsl version 4.6
#version 460

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;
layout (location = 3) in vec2 vTexCoord;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec3 outFragPos;
layout (location = 3) out vec2 outTexCoord;

layout (set = 0, binding = 0) uniform CameraBuffer{
	mat4 view;
	mat4 proj;
	mat4 viewProj;
} cameraData;

struct ObjectData{
	mat4 model;
	mat4 optionalData;
}; 

layout(std140, set = 1, binding = 0) readonly buffer ObjectBuffer
{
	ObjectData objects[];
} objectBuffer;

//push constants block
layout( push_constant ) uniform constants
{
	vec4 data;
	mat4 render_matrix;
} PushConstants;

void main()
{	
	mat4 modelMatrix = objectBuffer.objects[gl_BaseInstance].model;
	mat4 transformMatrix = (cameraData.viewProj * modelMatrix);
	gl_Position = transformMatrix * vec4(vPosition, 1.0f);
	outFragPos = vec3(objectBuffer.objects[gl_BaseInstance].model * vec4(vPosition, 1.0f));
	outColor = vColor;
	outNormal = vNormal;
	outTexCoord = vTexCoord;
}