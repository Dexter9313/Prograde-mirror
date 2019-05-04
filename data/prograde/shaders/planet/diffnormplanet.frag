#version 150 core

in vec3 f_position;

uniform samplerCube diff;
uniform samplerCube norm;

uniform float atmosphere;

uniform vec3 lightpos;

out vec4 outColor;

const float minR = 0.45;
const float maxR = 1.0;

const float minG = 0.45;
const float maxG = 1.0;

const float minB = 0.45;
const float maxB = 1.0;

void main()
{
	vec4 diffuse = texture(diff, f_position);
	vec3 normal  = texture(norm, f_position).xyz;
	normal       = normalize(normal * 2.0 - 1.0); // from [0;1] to [-1;1]

	// 0 or 1
	// avoids lighting stuff behind the planet
	// float coeff_pos = normalize(ceil(dot(normalize(lightpos),
	// normalize(f_position)) + 0.3));
	float coeff_pos      = dot(normalize(lightpos), normalize(f_position));
	float atmosColorGrad = coeff_pos * 0.5 + 0.5;

	// Fully empirical eye-balled algorithm, FAR from accurate, no actual
	// scattering computed at all...
	// Fast as heck though
	float atmoscoeff = clamp(atmosphere * (1.0 - coeff_pos), 0.0, 1.0);
	vec3 atmosCol = vec3(0.0);
	if(atmosColorGrad > minR && atmosColorGrad < maxR)
	{
		atmosCol.r
		    = 0.5
		          * pow(-atmosColorGrad / (maxR - minR) + maxR / (maxR - minR),
		                15.0)
		          * 0.7
		      + 0.3;
	}
	if(atmosColorGrad > minG && atmosColorGrad < maxG)
	{
		atmosCol.g
		    = 0.3
		          * pow(-atmosColorGrad / (maxG - minG) + maxG / (maxG - minG),
		                1.0)
		          * 0.53
		      + 0.47;
	}
	if(atmosColorGrad > minB && atmosColorGrad < maxB)
	{
		atmosCol.b = 1.0 + atmosColorGrad / (maxB - minB)
		             - maxB / (maxB - minB) * 0.8 + 0.2;
	}

	coeff_pos = min(1.0, max(0.0, coeff_pos + 0.1) * 10.0);

	float coeff = max(0.0, dot(normalize(lightpos), normal));

	outColor = diffuse;
	outColor.rgb *= coeff * coeff_pos;

	outColor.rgb = mix(outColor.rgb, atmosCol, atmoscoeff);
}
