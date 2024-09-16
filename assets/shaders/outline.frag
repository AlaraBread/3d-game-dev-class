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

float cubicOut(float t) {
  float f = t - 1.0;
  return f * f * f + 1.0;
}

vec3 readNormal(vec2 UV) {
	vec3 v = (texture(normalTexture, UV).xyz*2)-vec3(1);
	if(dot(v, v) < 0.5*0.5) {
		return vec3(0);
	}
	return v;
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
	float edginess = 0;
	if(curDepth == 1.0) {
		return;
	}
	for(int i = 0; i < 20; i++) {
		vec2 offset = get_pixel_offset(i);
		vec2 uv = offset*pixel_size+fragTexCoord;
		vec3 n = readNormal(uv);
		float d = readDepth(uv);
		if(d == 1.0) {
			continue;
		}
		if(dot(n, curNormal) < 0.4 || abs(d-curDepth) > 0.1) {
			edginess += 1.0/20.0;
		}
	}
	outColor.w = cubicOut(edginess);
}
