#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
	vec4 gl_Position;
};

layout(location = 0) in vec2 inPosition;

layout(location = 0)out vec2 fragTexCoord;

void main() {
	gl_Position = vec4(inPosition,0.0,1.0);
}