#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec4 boneWeights;
layout(location = 4) in ivec4 boneIds;

layout(set = 0, binding = 0) uniform UniformBufferObject{
	mat4 view;
	mat4 proj;
} ubo;


layout(push_constant) uniform pushConstants
{
	mat4 modelMatrix;
	vec4 color;
	float time;
	float ticksPerSecond;
	float duration;
	bool looping;
}constants;

out gl_PerVertex{
	vec4 gl_Position;
};

layout(rgba32f, set=3, binding=0) readonly uniform image2D boneData;

layout(set=4, binding=0) uniform OffsetArray{
mat4 Offsets[256];
}OffsetData;

vec4 q_slerp(vec4 a, vec4 b, float t){
	if(length(a) == 0.0){
		if(length(b) == 0.0) {
			return vec4(0.0,0.0,0.0,1.0);
		}
		return b;
	}else if(length(b)==0.0){
		return a;
	}
	
	float cosHalfAngle = a.w*b.w + dot(a.xyz, b.xyz);
	
	if(cosHalfAngle >= 1.0 || cosHalfAngle <= -1.0){
		return a;
	} else if (cosHalfAngle < 0.0){
		b.xyz = -b.xyz;
		b.w = -b.w;
		cosHalfAngle = -cosHalfAngle;
	}
	
	float blendA;
	float blendB;
	if(cosHalfAngle < 0.99){
		float halfAngle = acos(cosHalfAngle);
		float sinHalfAngle = sin(halfAngle);
		float oneOverSinHalfAngle = 1.0 / sinHalfAngle;
		blendA = sin(halfAngle * (1.0 - t)) * oneOverSinHalfAngle;
		blendB = sin(halfAngle * t) * oneOverSinHalfAngle;
	}else{
		blendA = 1.0 - t;
		blendB = t;
	}
	
	vec4 result = vec4(blendA * a.xyz + blendB * b.xyz, blendA * a.w + blendB * b.w);
	if(length(result) > 0.0){
		return normalize(result);
	}
	return vec4(0.0, 0.0, 0.0, 1.0);
}

mat4 q_ToMat4(vec4 q){
	float qxx = (q.x * q.x);
	float qyy = (q.y * q.y);
	float qzz = (q.z * q.z);
	float qxz = (q.x * q.z);
	float qxy = (q.x * q.y);
	float qyz = (q.y * q.z);
	float qwx = (q.w * q.x);
	float qwy = (q.w * q.y);
	float qwz = (q.w * q.z);
	
	mat4 Result = mat4(1.0);
	
	Result[0][0] = float(1) - float(2) * (qyy +  qzz);
	Result[0][1] = float(2) * (qxy + qwz);
	Result[0][2] = float(2) * (qxz - qwy);

	Result[1][0] = float(2) * (qxy - qwz);
	Result[1][1] = float(1) - float(2) * (qxx +  qzz);
	Result[1][2] = float(2) * (qyz + qwx);

	Result[2][0] = float(2) * (qxz + qwy);
	Result[2][1] = float(2) * (qyz - qwx);
	Result[2][2] = float(1) - float(2) * (qxx +  qyy);
	
	return Result;
}

mat4 GetBone(int id){
	int location = id;
	int tick1 = int(floor(constants.time*constants.ticksPerSecond));
	int tick2 = tick1+1;
	
	vec4 translation1 = imageLoad(boneData, ivec2(location, tick1*3));
	vec4 translation2 = imageLoad(boneData, ivec2(location, tick2*3));
	vec3 scale1 = imageLoad(boneData, ivec2(location, tick1*3+1)).xyz;
	vec3 scale2 = imageLoad(boneData, ivec2(location, tick2*3+1)).xyz;
	vec4 rotation1 = imageLoad(boneData, ivec2(location, tick1*3+2));
	vec4 rotation2 = imageLoad(boneData, ivec2(location, tick2*3+2));
	
	float progress;
	if(tick2>constants.duration*constants.ticksPerSecond && constants.looping == false){
		progress = 0.0;
	}else{
		progress = (constants.time*constants.ticksPerSecond-tick1)/(tick2-tick1);
	}
	
	progress = clamp(progress, 0.0, 1.0);
	
	vec4 translation = mix(translation1, translation2, progress);
	vec3 scale = mix(scale1, scale2, progress);
	vec4 rotation = q_slerp(rotation1, rotation2, progress);
	
	mat4 translationM = mat4(	1.0, 0.0, 0.0, 0.0,
								0.0, 1.0, 0.0, 0.0,
								0.0, 0.0, 1.0, 0.0,
								translation);
								
	mat4 scaleM = mat4( scale.x,0.0,0.0,0.0,
						0.0,scale.y,0.0,0.0,
						0.0,0.0,scale.z,0.0,
						0.0,0.0,0.0,1.0);
						
	mat4 rotationM = q_ToMat4(rotation);
	
	return translationM*rotationM*scaleM;
}

layout(location = 0) out vec3 geomNormal;
layout(location = 1) out mat4 modelMatrix;

void main(){
	mat4 boneTransform = mat4(1.0);
	
	if(boneWeights != vec4(0.0)){	
		boneTransform = (GetBone(boneIds.x)*OffsetData.Offsets[boneIds.x])*boneWeights[0];
		boneTransform += (GetBone(boneIds.y)*OffsetData.Offsets[boneIds.y])*boneWeights[1];
		boneTransform += (GetBone(boneIds.z)*OffsetData.Offsets[boneIds.z])*boneWeights[2];
		boneTransform += (GetBone(boneIds.w)*OffsetData.Offsets[boneIds.w])*boneWeights[3];
	}
	
	//boneTransform = mat4(1.0);
	
	gl_Position = constants.modelMatrix*boneTransform*vec4(position,1.0);
	
	geomNormal = mat3(inverse(transpose(constants.modelMatrix * boneTransform))) * normalize(normal);
	
	modelMatrix = constants.modelMatrix;
}