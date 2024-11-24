#version 450
#extension GL_ARB_separate_shader_objects : enable

// from https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83

float rand(vec2 c) { return fract(sin(dot(c.xy, vec2(12.9898, 78.233))) * 43758.5453); }

#define M_PI 3.14159265358979323846

float noise(vec2 p, float freq) {
	float unit = 1.0 / freq;
	vec2 ij = floor(p / unit);
	vec2 xy = mod(p, unit) / unit;
	// xy = 3.*xy*xy-2.*xy*xy*xy;
	xy = .5 * (1. - cos(M_PI * xy));
	float a = rand((ij + vec2(0., 0.)));
	float b = rand((ij + vec2(1., 0.)));
	float c = rand((ij + vec2(0., 1.)));
	float d = rand((ij + vec2(1., 1.)));
	float x1 = mix(a, b, xy.x);
	float x2 = mix(c, d, xy.x);
	return mix(x1, x2, xy.y);
}

float pNoise(vec2 p, int res) {
	float persistance = .5;
	float n = 0.;
	float normK = 0.;
	float f = 4.;
	float amp = 1.;
	int iCount = 0;
	for(int i = 0; i < 50; i++) {
		n += amp * noise(p, f);
		f *= 2.;
		normK += amp;
		amp *= persistance;
		if(iCount == res) break;
		iCount++;
	}
	float nf = n / normK;
	return nf * nf * nf * nf;
}

float rand(vec2 co, float l) { return rand(vec2(rand(co), l)); }
float rand(vec2 co, float l, float t) { return rand(vec2(rand(co, l), t)); }

float perlin(vec2 p, float dim, float time) {
	vec2 pos = floor(p * dim);
	vec2 posx = pos + vec2(1.0, 0.0);
	vec2 posy = pos + vec2(0.0, 1.0);
	vec2 posxy = pos + vec2(1.0);

	float c = rand(pos, dim, time);
	float cx = rand(posx, dim, time);
	float cy = rand(posy, dim, time);
	float cxy = rand(posxy, dim, time);

	vec2 d = fract(p * dim);
	d = -0.5 * cos(d * M_PI) + 0.5;

	float ccx = mix(c, cx, d.x);
	float cycxy = mix(cy, cxy, d.x);
	float center = mix(ccx, cycxy, d.y);

	return center * 2.0 - 1.0;
}

// p must be normalized!
float perlin(vec2 p, float dim) {

	/*vec2 pos = floor(p * dim);
	vec2 posx = pos + vec2(1.0, 0.0);
	vec2 posy = pos + vec2(0.0, 1.0);
	vec2 posxy = pos + vec2(1.0);

	// For exclusively black/white noise
	/*float c = step(rand(pos, dim), 0.5);
	float cx = step(rand(posx, dim), 0.5);
	float cy = step(rand(posy, dim), 0.5);
	float cxy = step(rand(posxy, dim), 0.5);*/

	/*float c = rand(pos, dim);
	float cx = rand(posx, dim);
	float cy = rand(posy, dim);
	float cxy = rand(posxy, dim);

	vec2 d = fract(p * dim);
	d = -0.5 * cos(d * M_PI) + 0.5;

	float ccx = mix(c, cx, d.x);
	float cycxy = mix(cy, cxy, d.x);
	float center = mix(ccx, cycxy, d.y);

	return center * 2.0 - 1.0;*/
	return perlin(p, dim, 0.0);
}

const uint MAX_LIGHTS = 16;

struct MeshUBO {
	mat4 model;
	mat4 view;
	mat4 proj;
	vec4 color;
	vec4 camera; // needed for many light calculations
};

struct MaterialUBO {
	vec4 diffuse;		// how much diffuse light affects this material - primary influcen for material color
	float transparency; // how translucent the material should be overall
	vec3 padding1;
	float flag;
	vec3 padding2;
	float time;
	vec3 padding3;
};

layout(binding = 0) uniform UniformBufferObject {
	MaterialUBO material; // this may become an array
	MeshUBO mesh;
}
ubo;

out gl_PerVertex { vec4 gl_Position; };

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 position;

void main() {
	fragTexCoord = inTexCoord;

	vec3 worldPos = (ubo.mesh.model * vec4(inPosition, 1.0)).xyz;
	// worldPos.z += perlin(vec2(sin(ubo.material.time) * 0.0, worldPos.y), 1.0, 0.0);
	gl_Position = ubo.mesh.proj * ubo.mesh.view * vec4(worldPos, 1.0);

	mat3 normalMatrix = inverse(mat3(ubo.mesh.model));
	fragNormal = normalize(inNormal * normalMatrix);
	position = worldPos;
}
