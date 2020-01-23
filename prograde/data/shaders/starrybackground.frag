#version 150 core

in vec4 f_finalcolor;
in float f_pointsize;

uniform sampler2D tex;

out vec4 outColor;

void main()
{
	vec4 texcol = vec4(1.0);
	if(f_pointsize > 1.0)
	{
		texcol = texture(tex, gl_PointCoord);
		texcol.a *= texcol.a;
	}

	outColor = texcol * f_finalcolor;
}
