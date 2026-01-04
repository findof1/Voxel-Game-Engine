#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject {
  mat4 view;
  mat4 proj;
} ubo;

layout(set = 1, binding = 0) readonly buffer ChunkBuffer {
mat4 models[];
}
chunks;

layout(location = 0) in ivec3 inPosition;
layout(location = 1) in uint inTexCoord;
layout(location = 2) in uint inTexIndex;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) flat out uint fragTexIndex;

vec2 unpackUV(uint packed) {
float u = float(packed & 0xFF) / 16.0;
float v = float((packed >> 8) & 0xFF) / 16.0;
return vec2(u, v);
}

void main() {
vec3 pos = vec3(inPosition) / 16.0; //since inPosition is a fixed point 12.4, convert to float

gl_Position = ubo.proj * ubo.view * chunks.models[gl_InstanceIndex] * vec4(pos, 1.0);
fragTexCoord = unpackUV(inTexCoord);
fragTexIndex = inTexIndex;
}