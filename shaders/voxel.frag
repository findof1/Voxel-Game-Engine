#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in flat uint fragTexIndex;

layout(binding = 1) uniform sampler2DArray texArray;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(texArray, vec3(fragTexCoord, fragTexIndex));
}