#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform sampler2D depth;
layout(set = 0, binding = 1) uniform sampler2D normal;
layout(set = 0, binding = 2) uniform sampler2D noise;

layout(constant_id = 0) const int SSAO_KERNEL_SIZE = 32;
layout(constant_id = 1) const float SSAO_RADIUS = 0.5;

layout(set = 1, binding = 0) uniform UBOSSAOKernel
{
	vec4 samples[SSAO_KERNEL_SIZE];
}uboSsaoKernel;

layout(set = 2, binding = 0) uniform UBO
{
	mat4 projection;
}ubo;

layout(location = 0) in vec2 uv;
layout(location = 0) out float outColor;

void main(){

}