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

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec4 fragColor;
layout(location = 3) out vec3 fragWorldPos;
layout(location = 4) flat out int fragMaterialIndex;

void main(){
	
	gl_Position = ubo.proj*ubo.view*modelMatrix*vec4(position,1.0);
	
	gl_Position.y = -gl_Position.y;
	
	fragWorldPos = vec3(modelMatrix*vec4(position,1.0));
		
	fragNormal = mat3(inverse(transpose(modelMatrix))) * normalize(normal);
	
	fragTexCoord = texCoord;
	
	fragColor = vec4(1.0, 1.0, 1.0, 1.0);
	
	fragMaterialIndex = gl_InstanceIndex;
}