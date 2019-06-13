#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec4 color;

layout(push_constant) uniform uPushConstant{
	mat4 projection;
} pc;

out gl_PerVertex{
    vec4 gl_Position;
};

layout(location = 0) out vec4 oColor;
layout(location = 1) out vec2 oUV;

void main(){
	gl_Position = pc.projection * vec4(position,0.0,1.0);
	oColor = color;
	oUV = uv;
}