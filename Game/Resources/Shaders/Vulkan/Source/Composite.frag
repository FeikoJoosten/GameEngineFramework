#version 450

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(input_attachment_index=1, set=0, binding=0) uniform subpassInput render;
layout(input_attachment_index=2, set=0, binding=1) uniform subpassInput imgui;

void main(){
	outColor.rgb = (subpassLoad(render).a) * subpassLoad(render).rgb + (1-subpassLoad(imgui).a)*subpassLoad(imgui).rgb;
	//outColor.a = subpassLoad(imgui).a;
	//outColor = vec4(1.0,0.0,0.0,1.0);
	//outColor.rgb = subpassLoad(imgui).rgb;
	outColor.a=1.0;
	//outColor.rgb = subpassLoad(render).rgb;
}