#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
	vec3 color;
	uvec2 extents;
} ubo;
layout(binding = 1) uniform sampler2D normalTexture;

layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

vec3 readNormal(vec2 UV) {
	return (texture(normalTexture, UV).xyz*2.0)-vec3(1.0);
}

void main() {
	vec3 curNormal = readNormal(fragTexCoord);
	float edginess = 0;
	for(int i = 0; i < 8; i++) {
		vec2 offset = vec2(0);
		switch(i) {
			case 0:
				offset = vec2(1, 0);
			break;
			case 1:
				offset = vec2(-1, 0);
			break;
			case 2:
				offset = vec2(0, 1);
			break;
			case 3:
				offset = vec2(0, -1);
			break;
			case 4:
				offset = vec2(1, 1);
			break;
			case 5:
				offset = vec2(-1, -1);
			break;
			case 6:
				offset = vec2(-1, 1);
			break;
			case 7:
				offset = vec2(1, -1);
			break;
		}
		vec2 UV = offset*0.0025+fragTexCoord;// / ubo.extents;
		vec3 n = readNormal(UV);
		if(dot(n, curNormal) < 0.6 && ((n != vec3(0)) || (curNormal != vec3(0)))) {
			edginess = 1;
			break;
		}
	}
	outColor.xyz = ubo.color;
	outColor.w = edginess;
}
