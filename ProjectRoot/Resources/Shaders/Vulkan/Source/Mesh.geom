#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(triangles_adjacency, invocations = 32) in;
layout(triangle_strip) out;
layout(max_vertices = 6) out;

struct Light{
	vec4 position;
	vec4 direction;
	vec4 color;
	float radius;
	float attunuation;
	float coneInnerAngle;
	float coneOuterAngle;
};

layout(set = 2, binding = 0) uniform Lights{
	Light lights[1024];
}LightInfo;

layout(set = 2, binding = 1) uniform SceneData{
	int lightCount;
	vec4 viewPos;
}SceneInfo;

layout(set = 0, binding = 0) uniform UBO
{
	mat4 view;
	mat4 proj;
} ubo;

in gl_PerVertex
{
	vec4 gl_Position;
}gl_in[];

layout(location = 0) in vec3 normal[];
layout(location = 1) in vec2 texCoord[];
layout(location = 2) in vec4 color[];
layout(location = 3) in vec3 worldPos[];

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec4 fragColor;
layout(location = 3) out vec3 fragWorldPos;

void main(){	
	
	gl_Position = gl_in[0].gl_Position;
	fragNormal = normal[0];
	fragTexCoord = texCoord[0];
	fragColor = color[0];
	fragWorldPos = worldPos[0];
	EmitVertex();
			
	gl_Position = gl_in[2].gl_Position;
	fragNormal = normal[2];
	fragTexCoord = texCoord[2];
	fragColor = color[2];
	fragWorldPos = worldPos[2];
	EmitVertex();
	
	gl_Position = gl_in[4].gl_Position;
	fragNormal = normal[4];
	fragTexCoord = texCoord[4];
	fragColor = color[4];
	fragWorldPos = worldPos[4];
	EmitVertex();
	
	EndPrimitive();
	
	/*		
		vec3 e1 = worldPos[2] - worldPos[0];
		vec3 e2 = worldPos[4] - worldPos[0];
		vec3 e3 = worldPos[1] - worldPos[0];
		vec3 e4 = worldPos[3] - worldPos[2];
		vec3 e5 = worldPos[4] - worldPos[2];
		vec3 e6 = worldPos[5] - worldPos[0];
		
		vec3 Normal = normal[0];
		
		Light light = LightInfo.lights[gl_InvocationID];
		
		vec3 LightDir;
		
		if(light.position.w==0.0){
			LightDir = light.direction.xyz;
		}
		
		else {
			LightDir = light.position.xyz - worldPos[0];
		}
		
		if(light.position.w != 0.0 && length(LightDir)>light.radius)
		return;
		
		LightDir = normalize(LightDir);		
		
		if(dot(Normal, LightDir)>0.00001) {
			Normal = cross(e3,e1);
			
			if(dot(Normal, LightDir) <= 0){
				gl_Position = gl_in[0].gl_Position;
				fragNormal = normal[0];
				fragTexCoord = texCoord[0];
				fragColor = color[0];
				fragWorldPos = worldPos[0];
				EmitVertex();
				
				gl_Position = gl_in[0].gl_Position+vec4(LightDir*90.0,0.0);
				fragNormal = normal[0];
				fragTexCoord = texCoord[0];
				fragColor = color[0];
				fragWorldPos = worldPos[0];
				EmitVertex();
				
				EndPrimitive();
			}
			
			Normal = cross(e4,e5);
			LightDir = LightInfo.lights[1].direction.xyz;
			
			if(dot(Normal, LightDir) <= 0){
				gl_Position = gl_in[2].gl_Position;
				fragNormal = normal[2];
				fragTexCoord = texCoord[2];
				fragColor = color[2];
				fragWorldPos = worldPos[2];
				EmitVertex();
				
				gl_Position = gl_in[2].gl_Position+vec4(LightDir*90.0,0.0);
				fragNormal = normal[2];
				fragTexCoord = texCoord[2];
				fragColor = color[2];
				fragWorldPos = worldPos[2];
				EmitVertex();
				
				EndPrimitive();
			}
			
			Normal = cross(e2, e6);
			LightDir = LightInfo.lights[1].direction.xyz;
			
			if(dot(Normal, LightDir) <= 0){
				gl_Position = gl_in[4].gl_Position;
				fragNormal = normal[4];
				fragTexCoord = texCoord[4];
				fragColor = color[4];
				fragWorldPos = worldPos[4];
				EmitVertex();
				
				gl_Position = gl_in[4].gl_Position+vec4(LightDir*90.0,0.0);
				fragNormal = normal[4];
				fragTexCoord = texCoord[4];
				fragColor = color[4];
				fragWorldPos = worldPos[4];
				EmitVertex();
				
				EndPrimitive();
			}
			
		}
	*/
}