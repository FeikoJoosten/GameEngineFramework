#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 fragColor;
layout(location = 3) in vec3 worldPos;
layout(location = 4) flat in int materialId;

layout(set = 1, binding = 0) uniform sampler2D diffuse;
layout(set = 1, binding = 1) uniform sampler2D specular;
layout(set = 1, binding = 2) uniform sampler2D bump;

layout(set = 1, binding = 3) uniform Material {
	int diffuseFound;
	vec4 diffuseColor;
	int specularFound;
	vec4 specularColor;
	int bumpMapFound;
	float specularScale;
	float specularExponent;
}MaterialData;

layout(location = 0) out vec4 outAlbedo;
layout(location = 1) out vec4 outPosition;
layout(location = 2) out vec4 outNormal;

const float NEAR_PLANE = 0.1;
const float FAR_PLANE = 1000.0;

float linearDepth(float depth)
{
	float z = depth * 2.0f - 1.0f; 
	return (2.0f * NEAR_PLANE * FAR_PLANE) / (FAR_PLANE + NEAR_PLANE - z * (FAR_PLANE - NEAR_PLANE));	
}

void main(){
	vec4 outColor;
	if(MaterialData.diffuseFound>0){
		outColor = texture(diffuse,fragTexCoord) /** MaterialData.diffuseColor*/;
	}
	else{
		outColor = MaterialData.diffuseColor;
	}
	
	float spec = 0.0;
	
	if(MaterialData.specularFound>0){
		spec = texture(specular, fragTexCoord).x*MaterialData.specularScale;
	}else{
		spec = MaterialData.specularColor.x * MaterialData.specularScale;
	}
	
	outPosition = vec4(worldPos,1.0);
	vec3 N = normalize(fragNormal);
	
	if(MaterialData.bumpMapFound>0){
		N = normalize(2.0 * texture(bump, fragTexCoord).rgb - 1.0);
	}
	
	//N.x = -N.x;
	outNormal = vec4(N,MaterialData.specularExponent);
	
	outAlbedo = outColor;
	
	outAlbedo.a = spec;
	
	outPosition.a = gl_FragCoord.z;
	
}