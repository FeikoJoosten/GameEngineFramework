#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;

layout(set = 0, binding = 0) uniform UniformBufferObject{
	mat4 view;
	mat4 proj;
} ubo;

layout(push_constant) uniform pushConstants
{
	mat4 modelMatrix;
}push;

out gl_PerVertex {
	vec4 gl_Position;
};

void main(){
gl_Position = ubo.proj * ubo.view * push.modelMatrix * vec4(position, 1.0);
}