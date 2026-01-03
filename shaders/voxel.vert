#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject {
  mat4 view;
  mat4 proj;
} ubo;

layout(set = 1, binding = 0) readonly buffer ChunkBuffer {
mat4 models[];
}
chunks;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in uint inTexIndex;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) flat out uint fragTexIndex;

void main() {
gl_Position = ubo.proj * ubo.view * chunks.models[gl_InstanceIndex] * vec4(inPosition, 1.0);
fragTexCoord = inTexCoord;
fragTexIndex = inTexIndex;
}