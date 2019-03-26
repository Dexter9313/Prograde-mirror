#version 150 core

in vec3 position;

uniform mat4 camera;
uniform vec3 bodyCenter;
uniform vec3 centralBodyCenter;

out vec3 centralBodyLightDir;
out vec3 vNormal;

void main()
{
	vNormal             = position;
	centralBodyLightDir = normalize(centralBodyCenter - bodyCenter);
	gl_Position         = camera * vec4(position, 1.0);
}
