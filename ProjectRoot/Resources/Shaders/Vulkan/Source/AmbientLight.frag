#version 450

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

layout(input_attachment_index=0, set=0, binding=0) uniform subpassInput albedo;
layout(input_attachment_index=1, set=0, binding=1) uniform subpassInput position;
layout(input_attachment_index=2, set=0, binding=2) uniform subpassInput normal;

layout(set = 1, binding = 0) uniform AmbientLight{
	vec3 color;
}AmbientLightInfo;

void main(){
	vec4 fragColor = subpassLoad(albedo);
	
	outColor = fragColor * vec4(AmbientLightInfo.color, 1.0);
	
}