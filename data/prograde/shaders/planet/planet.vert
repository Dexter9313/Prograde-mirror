#version 150 core

in vec3 position;

uniform mat4 camera;

out vec3 f_position;
out mat3 f_tantoworld;

void main()
{
	f_position = position;

	vec3 transNorm  = normalize(position);
	vec3 transTan   = cross(vec3(0.0, 0.0, 1.0), transNorm);
	vec3 transBiTan = cross(transNorm, transTan);

	f_tantoworld = mat3(transTan, transBiTan, transNorm);

	gl_Position = camera * vec4(position, 1.0);
}
