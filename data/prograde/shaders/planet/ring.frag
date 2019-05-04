#version 150 core

in vec2 f_position;

uniform sampler2D tex;

uniform float inner;
uniform float outer;
uniform float planetradius;
uniform vec3 lightpos;

out vec4 outColor;

void main()
{
	// make perfect circles
	if(length(f_position) < inner || length(f_position) > outer)
		discard;

	float alt      = length(f_position);
	float texCoord = (alt - inner) / (outer - inner);

	vec3 lightdir = normalize(lightpos - vec3(f_position, 0.0));

	vec3 closestPoint = dot(lightdir, -1 * vec3(f_position, 0.0)) * lightdir
	                    + vec3(f_position, 0.0);

	float coeff = 1.0;
	if(length(closestPoint) < planetradius
	   && dot(lightdir, vec3(f_position, 0.0)) < 0.0)
		coeff = pow(length(closestPoint) / planetradius, 500);

	outColor = texture(tex, vec2(texCoord, 0.5));
	outColor.rgb *= coeff;
}
