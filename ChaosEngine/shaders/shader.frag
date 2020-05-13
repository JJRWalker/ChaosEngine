#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler[32];

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in float fragImgIndex;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 textureColor = texture(texSampler[int(fragImgIndex)], fragTexCoord);
    outColor = vec4(fragColor.xyz * textureColor.xyz, textureColor.w);
    outColor =  (fragColor.w) * outColor;
}