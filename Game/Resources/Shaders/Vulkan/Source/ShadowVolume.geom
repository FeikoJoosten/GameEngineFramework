#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(triangles_adjacency) in;
layout(triangle_strip) out;
layout(max_vertices = 18) out;

struct Light{
	vec4 position;
	vec4 direction;
	vec4 color;
	float radius;
	float attunuation;
	float coneInnerAngle;
	float coneOuterAngle;
};

layout(set = 1, binding = 0) uniform Lights{
	Light light;
}LightInfo;

layout(set = 1, binding = 1) uniform SceneData{
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

layout(location = 0) in vec3 geomNormal[];
layout(location = 1) in mat4 geomModelMatrix[];

float EPSILON = -0.5;

void main(){	
	
	vec3 ns[3];
	vec3 d[3];
	vec4 v[4];
	
	vec4 or_pos[3] = {
		gl_in[0].gl_Position,
		gl_in[2].gl_Position,
		gl_in[4].gl_Position,
	};
	
	Light light = LightInfo.light;
	
	if(light.position.w==0.0)
		light.position.xyz = light.direction.xyz;
	
	ns[0] = cross(
	gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz,
	gl_in[4].gl_Position.xyz - gl_in[0].gl_Position.xyz);
	ns[1] = cross(
	gl_in[4].gl_Position.xyz - gl_in[2].gl_Position.xyz,
	gl_in[0].gl_Position.xyz - gl_in[2].gl_Position.xyz);
	ns[2] = cross(
	gl_in[0].gl_Position.xyz - gl_in[4].gl_Position.xyz,
	gl_in[2].gl_Position.xyz - gl_in[4].gl_Position.xyz);
	
	d[0] = light.position.xyz - light.position.w*gl_in[0].gl_Position.xyz;
	d[1] = light.position.xyz - light.position.w*gl_in[2].gl_Position.xyz;
	d[2] = light.position.xyz - light.position.w*gl_in[4].gl_Position.xyz;
	
	bool faces_light = true;
	if(!(dot(ns[0],d[0])>0 || dot(ns[1],d[1])>0 ||
		dot(ns[2],d[2])>0) /*|| 
		(length(d[0])>light.radius ||
		 length(d[1])>light.radius ||
		 length(d[2])>light.radius)*/){
		return;
		or_pos[1] = gl_in[4].gl_Position;
		or_pos[2] = gl_in[2].gl_Position;
		faces_light = false;
	}
	
	mat4 pv = ubo.proj*ubo.view;
	
	gl_Position = pv*(or_pos[0]+vec4(d[0]*EPSILON,0.0));
	gl_Position.y = -gl_Position.y;
	EmitVertex();
	
	gl_Position = pv*(or_pos[1]+vec4(d[1]*EPSILON,0.0));
	gl_Position.y = -gl_Position.y;
	EmitVertex();
	
	gl_Position = pv*(or_pos[2]+vec4(d[2]*EPSILON,0.0));
	gl_Position.y = -gl_Position.y;
	EmitVertex();
	EndPrimitive();
	
	v[0] = vec4(light.position.w*or_pos[0].xyz - light.position.xyz,0.0);
	v[1] = vec4(light.position.w*or_pos[2].xyz - light.position.xyz,0.0);
	v[2] = vec4(light.position.w*or_pos[1].xyz - light.position.xyz,0.0);
	
	gl_Position = pv*v[0];
	gl_Position.y = -gl_Position.y;
	EmitVertex();
	
	gl_Position = pv*v[1];
	gl_Position.y = -gl_Position.y;
	EmitVertex();
	
	gl_Position = pv*v[2];
	gl_Position.y = -gl_Position.y;
	EmitVertex();
	EndPrimitive();
	
	for(int i = 0; i < 3; i++){
		int v0 = i*2;
		int nb = (i*2+1);
		int v1 = (i*2+2)%6;
		
		ns[0] = cross(
		gl_in[nb].gl_Position.xyz-gl_in[v0].gl_Position.xyz,
		gl_in[v1].gl_Position.xyz-gl_in[v0].gl_Position.xyz);
		ns[1] = cross(
		gl_in[v1].gl_Position.xyz-gl_in[nb].gl_Position.xyz,
		gl_in[v0].gl_Position.xyz-gl_in[nb].gl_Position.xyz);
		ns[2] = cross(
		gl_in[v0].gl_Position.xyz-gl_in[v1].gl_Position.xyz,
		gl_in[nb].gl_Position.xyz-gl_in[v1].gl_Position.xyz);
		
		d[0] = light.position.xyz - light.position.w*gl_in[v0].gl_Position.xyz;
		d[1] = light.position.xyz - light.position.w*gl_in[nb].gl_Position.xyz;
		d[2] = light.position.xyz - light.position.w*gl_in[v1].gl_Position.xyz;
		
		if(gl_in[nb].gl_Position.w<1e-3 ||
			(faces_light!=	(dot(ns[0],d[0])>0 ||
							 dot(ns[1],d[1])>0 ||
							 dot(ns[2],d[2])>0)));
		{
			int i0 = faces_light ? v0 : v1;
			int i1 = faces_light ? v1 : v0;
			
			v[0] = (gl_in[i0].gl_Position+vec4(d[0]*EPSILON,0.0));
			v[1] = vec4(light.position.w*gl_in[i0].gl_Position.xyz - light.position.xyz,0.0);
			v[2] = (gl_in[i1].gl_Position+vec4(d[2]*EPSILON,0.0));
			v[3] = vec4(light.position.w*gl_in[i1].gl_Position.xyz - light.position.xyz,0.0);
			
			gl_Position = pv*v[0];
			gl_Position.y = -gl_Position.y;
			EmitVertex();
			
			gl_Position = pv*v[1];
			gl_Position.y = -gl_Position.y;
			EmitVertex();
			
			gl_Position = pv*v[2];
			gl_Position.y = -gl_Position.y;
			EmitVertex();
			
			gl_Position = pv*v[3];
			gl_Position.y = -gl_Position.y;
			EmitVertex();
			EndPrimitive();
		}
	}
}