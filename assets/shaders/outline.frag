#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
	vec3 color;
	vec2 extents;
	float zNear;
	float zFar;
} ubo;
layout(binding = 1) uniform sampler2D normalTexture;
layout(binding = 2) uniform sampler2D depthTexture;

layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

float cubicIn(float t) {
  return t * t * t;
}

vec3 readNormal(vec2 UV) {
	return normalize((texture(normalTexture, UV).xyz*2)-vec3(1));
}

float linearizeDepth(float d, float zNear, float zFar) {
	return zNear * zFar / (zFar + d * (zNear - zFar));
}

float readDepth(vec2 UV) {
	return linearizeDepth(texture(depthTexture, UV).x, 0.1, 100000);
}

vec2 get_pixel_offset(int i) {
	switch(i) {
		case 0: return vec2(1, 0);
		case 1: return vec2(0, 1);
		case 2: return vec2(0, -1);
		case 3: return vec2(-1, 0);

		case 4: return vec2(1, 1);
		case 5: return vec2(-1, -1);
		case 6: return vec2(-1, 1);
		case 7: return vec2(1, -1);

		case 8: return vec2(-2, -1);
		case 9: return vec2(-2, 0);
		case 10: return vec2(-2, 1);

		case 11: return vec2(2, -1);
		case 12: return vec2(2, 0);
		case 13: return vec2(2, 1);

		case 14: return vec2(-1, -2);
		case 15: return vec2(0, -2);
		case 16: return vec2(1, -2);

		case 17: return vec2(-1, 2);
		case 18: return vec2(0, 2);
		case 19: return vec2(1, 2);
	}
}

void main() {
	vec3 curNormal = readNormal(fragTexCoord);
	float curDepth = readDepth(fragTexCoord);
	vec2 pixel_size = 1.0/vec2(1280, 720);
	outColor.xyz = ubo.color;
	outColor.w = 0;
	for(int i = 0; i < 16; i++) {
		vec2 offset = get_pixel_offset(i);
		vec2 uv = offset*pixel_size+fragTexCoord;
		vec3 n = readNormal(uv);
		float d = readDepth(uv);
		if(dot(n, curNormal) < 0.9 || abs(d-curDepth) > 0.1) {
			outColor.w = 1;
			break;
		}
	}
	outColor.w = 1;
	outColor.xyz = curNormal;
}
