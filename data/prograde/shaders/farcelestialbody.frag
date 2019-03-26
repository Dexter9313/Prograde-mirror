#version 150 core

in vec3 centralBodyLightDir;
in vec3 vNormal;

uniform vec3 color;

out vec4 outColor;

void main()
{
	//-1 is related to the temporary fix
	// float multiplier = max(0, dot(-1 * vNormal, centralBodyLightDir));
	float multiplier = max(0, dot(vNormal, centralBodyLightDir));
	vec3 trueColor   = multiplier * color;
	outColor     = vec4(trueColor, 1.0);
}
