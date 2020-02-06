#version 150 core

in vec3 position;
in float mag;
in vec3 color;

uniform mat4 camera;
uniform float pixelSolidAngle;

out vec4 f_finalcolor;

void main()
{
	gl_Position = camera * vec4(position, 1.0);
	// lm.m-2
	float irradiance = pow(10.0, 0.4 * (-mag - 14.0));

	float mul    = irradiance / pixelSolidAngle;
	f_finalcolor = vec4(mul * color, 1.0);
}
