#version 450
#extension GL_ARB_separate_shader_objects : enable

const uint MAX_SHADOWS = 64;

struct Shadow {
	vec3 top;
	float size;
	vec3 bottom;
	float padding;
};

struct MeshUBO {
	mat4 model;
	mat4 view;
	mat4 proj;
	vec4 color;
	vec4 camera; // needed for many light calculations
	Shadow shadows[MAX_SHADOWS];
};

struct MaterialUBO {
	vec4 ambient;		// how much ambient light affects this material
	vec4 diffuse;		// how much diffuse light affects this material - primary influcen for material color
	vec4 specular;		// color of the shine on the materials
	vec4 emission;		// color that shows regardless of light
	float transparency; // how translucent the material should be overall
	float shininess;	// how shiny the materials is.  // how pronounced the specular is
	vec2 padding;		// for alignment
};

layout(binding = 0) uniform UniformBufferObject {
	MeshUBO mesh;
	MaterialUBO material; // this may become an array
}
ubo;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 position;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 normalColor;

// https://stackoverflow.com/a/32246825
vec3 getWorldPos(vec2 texCoord, float depth) {
	float z = depth * 2.0 - 1.0;

	vec4 clipSpacePosition = vec4(texCoord * 2.0 - 1.0, z, 1.0);
	vec4 viewSpacePosition = inverse(ubo.mesh.proj) * clipSpacePosition;

	// Perspective division
	viewSpacePosition /= viewSpacePosition.w;

	vec4 worldSpacePosition = inverse(ubo.mesh.view) * viewSpacePosition;

	return worldSpacePosition.xyz;
}

float linearizeDepth(float d) { return 0.1 * 100000. / (100000. + d * (0.1 - 100000.)); }

void main() {
	int i;
	vec4 surfaceColor = texture(texSampler, fragTexCoord);
	vec3 normal = fragNormal;
	vec3 surfaceToCamera = normalize(ubo.mesh.camera.xyz - position);
	vec3 worldPos = position;
	bool isShadow = false;
	for(int i = 0; i < MAX_SHADOWS; i++) {
		float size = ubo.mesh.shadows[i].size;
		if(size < 0) { break; }
		float horizontalDist = length(worldPos.xy - ubo.mesh.shadows[i].bottom.xy);
		float dist = length(worldPos - ubo.mesh.shadows[i].bottom);
		if(worldPos.z < ubo.mesh.shadows[i].top.z && worldPos.z > ubo.mesh.shadows[i].bottom.z &&
			   horizontalDist < size ||
		   dist < size) {
			isShadow = true;
			break;
		}
	}

	surfaceColor.xyz *= ubo.material.diffuse.xyz;
	surfaceColor.w *= ubo.material.diffuse.w * ubo.material.transparency;

	vec3 sunDir = vec3(0, 0, 1);
	float d = dot(normal, normalize(sunDir));
	if(d >= 0.97) {
		// specular
		outColor = mix(vec4(1.0, 1.0, 1.0, 1.0), surfaceColor, 0.3);
		normal = -normal; // draw outline around specular
	} else if(d >= 0.3) {
		// midtone
		outColor = mix(vec4(1.0, 1.0, 1.0, 1.0), surfaceColor, 0.8);
	} else {
		// shadow
		float bands = 4.0;
		outColor = (round(clamp((d + 2.0) * 0.5, 0.0, 1.0) * bands) / bands) * surfaceColor;
	}
	if(isShadow) {
		outColor = mix(vec4(0.0, 0.0, 0.0, 1.0), surfaceColor, 0.7);
		normal = -normal; // draw outline around shadow
	}
	outColor.w = 1.0;
	normalColor.xyz = (normal + vec3(1)) / 2;
	normalColor.w = 1.0;
}
