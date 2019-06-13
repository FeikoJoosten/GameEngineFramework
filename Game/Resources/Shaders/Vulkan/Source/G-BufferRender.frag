#version 450

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(input_attachment_index=0, set=0, binding=0) uniform subpassInput albedo;
layout(input_attachment_index=1, set=0, binding=1) uniform subpassInput position;
layout(input_attachment_index=2, set=0, binding=2) uniform subpassInput normal;

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

void main(){
	vec3 fragPos = subpassLoad(position).rgb;
	vec4 fragColor = subpassLoad(albedo);
	vec3 fragNormal = subpassLoad(normal).rgb;
	
	float specExponent = subpassLoad(normal).a;
	
	outColor.rgb = vec3(0.0);
	
	outColor.a = 1.0;
	
		Light light = LightInfo.light;
		
		vec3 dir = light.direction.xyz;
		//dir.y = -dir.y;
		
		if(light.position.w==0.0){	// Directional light
		if(light.direction.xyz == vec3(0.0)){
			outColor.xyz+= light.color.xyz * fragColor.xyz;
		}else{
			
				vec3 V = SceneInfo.viewPos.xyz - fragPos;
				V = normalize(V);
				
				vec3 N = normalize(fragNormal);
				float NdotL = max(0.0, dot(N, normalize(dir)));
				vec3 diff = light.color.xyz * fragColor.xyz * NdotL;
				
				vec3 R = reflect( - normalize(dir), N);
				float NdotR = max(0.0, dot(R,V));
				vec3 spec = light.color.xyz * fragColor.a * pow(NdotR, specExponent);
				
				if(specExponent==0.0){
					spec = vec3(0.0);
				}
			
				outColor.xyz += diff+spec;
			}
		}
		else if(light.coneInnerAngle>=(2.0*3.141592)||light.coneOuterAngle>=(2.0*3.141592)){ // Point light
			vec3 L = light.position.xyz - fragPos;
			float dist = length(L);
			
			if(dist<light.radius){
				vec3 V = SceneInfo.viewPos.xyz - fragPos;
				V = normalize(V);
				
				L = normalize(L);
				
				float intensity = (1.0 / 255.0) * (4.0 * 3.141592 * pow(light.radius, 2.0));
				
				float atten = intensity / (4.0 * 3.141592 * pow(dist, 2.0));
				
				atten = light.radius / (pow(dist, 2.0) + 1.0);
								
				vec3 N = normalize(fragNormal);
				float NdotL = max(0.0, dot(N, L));
				vec3 diff = light.color.xyz * fragColor.xyz * NdotL * atten;
				
				vec3 R = reflect(-L, N);
				float NdotR = max(0.0, dot(R,V));
				vec3 spec = light.color.xyz * fragColor.a * pow(NdotR, specExponent) * atten;
					
				if(specExponent==0.0){
					spec = vec3(0.0);
				}
				
				outColor.xyz += diff+spec;
			}
		}
		else { // Spot light
			vec3 L = light.position.xyz - fragPos;
			float dist = length(L);
			
			L = normalize(L);
			
			float angle = acos(dot(normalize(dir), L));
			
			if(dist<light.radius && angle < light.coneOuterAngle){
				vec3 V = SceneInfo.viewPos.xyz - fragPos;
				V = normalize(V);
				
				L = normalize(L);
				
				float intensity = (1.0 / 255.0) * (4.0 * 3.141592 * pow(light.radius, 2.0));
				
				float atten = intensity / (4.0 * 3.141592 * pow(dist, 2.0));
								
				atten = light.radius / (pow(dist, 2.0) + 1.0);
				
				vec3 N = normalize(fragNormal);
				float NdotL = max(0.0, dot(N, L));
				vec3 diff = light.color.xyz * fragColor.xyz * NdotL * atten;
				
				vec3 R = reflect(-L, N);
				float NdotR = max(0.0, dot(R,V));
				vec3 spec = light.color.xyz * fragColor.a * pow(NdotR, specExponent) * atten;
					
				if(specExponent==0.0){
					spec = vec3(0.0);
				}
				
				float factor = (angle - light.coneInnerAngle) / (light.coneOuterAngle - light.coneInnerAngle);
				
				diff = mix(diff, vec3(0.0), factor);
				spec = mix(spec, vec3(0.0), factor);
				
				outColor.xyz += diff + spec;
			}
		}
	//outColor = vec4(fragNormal,1.0);
}