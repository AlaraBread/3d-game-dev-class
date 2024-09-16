#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
	vec4 gl_Position;
};

layout(location = 1) out vec2 fragTexCoord;

void main() {
	switch (gl_VertexIndex) {
		case 0:
			gl_Position = vec4(1.0, 1.0, 0.0, 1.0);
			fragTexCoord = vec2(1.0, 1.0);
		break;
		case 1:
			gl_Position = vec4(-1.0, 1.0, 0.0, 1.0);
			fragTexCoord = vec2(0.0, 1.0);
		break;
		case 2:
			gl_Position = vec4(1.0, -1.0, 0.0, 1.0);
			fragTexCoord = vec2(1.0, 0.0);
		break;
		case 3:
			gl_Position = vec4(-1.0, -1.0, 0.0, 1.0);
			fragTexCoord = vec2(0.0, 0.0);
		break;
	}
}
