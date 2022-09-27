#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;

layout(set = 0, binding = 0) uniform UniformBufferObject{
	mat4 view;
	mat4 proj;
} ubo;

layout(push_constant) uniform pushConstants
{
	mat4 modelMatrix;
}constants;

out gl_PerVertex {
	vec4 gl_Position;
};

layout(location = 0) out vec2 fragTexCoords;
layout(location = 1) out vec4 fragColor;

void main()
{
	gl_Position = ubo.proj*ubo.view*constants.modelMatrix*vec4(position,1.0);
	gl_Position.y = -gl_Position.y;
	fragTexCoords=texCoords;
	fragColor = vec4(1.0,1.0,1.0,1.0);
}