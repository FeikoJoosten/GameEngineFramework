#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

layout(location = 3) in mat4 modelMatrix;

layout(set = 0, binding = 0) uniform UniformBufferObject{
	mat4 view;
	mat4 proj;
} ubo;

out gl_PerVertex{
	vec4 gl_Position;
};

layout(location = 0) out vec3 geomNormal;
layout(location = 1) out mat4 geomModelMatrix;

void main(){
	
	gl_Position = modelMatrix*vec4(position,1.0);
	
	geomNormal = mat3(inverse(transpose(modelMatrix))) * normalize(normal);
	
	geomModelMatrix = modelMatrix;
	
}