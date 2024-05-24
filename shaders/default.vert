#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
    vec4 color;
    vec4 flags;
} ubo;

layout(binding = 1) uniform UniformBufferObject
{
    mat4 bones[100];
} armature;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in uvec4 boneIndices;
layout(location = 4) in vec4  boneWeights;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec4 colorMod;
layout(location = 3) out vec4 fragAmbient;
layout(location = 4) out vec3 fragAmbientDir;
layout(location = 5) out vec3 position;
layout(location = 6) out vec3 camPosition;
layout(location = 7) out vec4 detailColor;

void main()
{
    vec4 tempNormal;
    mat4 bone;
    mat4 model = ubo.model;
    mat4 mvp = ubo.proj * ubo.view * ubo.model;
    model[3][0] = 0;
    model[3][1] = 0;
    model[3][2] = 0;
    fragTexCoord = inTexCoord;
    colorMod = ubo.color;
    fragAmbient = ubo.ambientColor;
    fragAmbientDir = ubo.ambientDir.xyz;
    detailColor = ubo.detailColor;
    if (ubo.flags.x != 0.0)
    {
        bone = (boneWeights.x * ubo.bones[int(boneIndices.x)])
             + (boneWeights.y * ubo.bones[int(boneIndices.y)])
             + (boneWeights.z * ubo.bones[int(boneIndices.z)])
             + (boneWeights.w * ubo.bones[int(boneIndices.w)]);
        gl_Position =  mvp * bone * vec4(inPosition, 1.0);
        //colorMod = boneWeights;
    }
    else
    {
        gl_Position = mvp * vec4(inPosition, 1.0);
    }
    tempNormal = model * vec4(inNormal,1.0);
    fragNormal = normalize(tempNormal.xyz);
    camPosition = ubo.cameraPostion.xyz;
}
