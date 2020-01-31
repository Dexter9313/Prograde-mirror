#version 150 core

in vec3 position;
in float mag;
in vec3 color;

uniform mat4 camera;
uniform float exposure;
uniform float pixelSolidAngle;

out vec4 f_finalcolor;
out float f_pointsize;

float max3(vec3 v)
{
	return max(max(v.x, v.y), v.z);
}

float changeRange(float x, float a, float b, float c, float d)
{
	return c + (d-c)/(b-a) * (x-a);
}
/*
vec3 RGBtoxyY(vec3 rgb)
{
	// Convert from RGB to XYZ
	float X = rgb.r * 0.4124 + rgb.g * 0.3576 + rgb.b * 0.1805;
	float Y = rgb.r * 0.2126 + rgb.g * 0.7152 + rgb.b * 0.0722;
	float Z = rgb.r * 0.0193 + rgb.g * 0.1192 + rgb.b * 0.9505;

	// Convert from XYZ to xyY
	float L = (X + Y + Z);
	float x = X / L;
	float y = Y / L;

	return vec3(x, y, Y);
}

vec3 xyYtoRGB(vec3 xyY)
{
	// Convert from xyY to XYZ
	float X = xyY.r * (xyY.b / xyY.g);
	float Y = xyY.b;
	float Z = (1.0 - xyY.r - xyY.g) * (xyY.b / xyY.g);

	// Convert from XYZ to RGB
	float R = X * 3.2406 + Y * -1.5372 + Z * -0.4986;
	float G = X * -0.9689 + Y * 1.8758 + Z * 0.0415;
	float B = X * 0.0557 + Y * -0.2040 + Z * 1.0570;

	return vec3(R, G, B);
}
*/

void main()
{
	gl_Position = camera * vec4(position, 1.0);
	// lm.m-2
	float irradiance = pow(10.0, 0.4 * (-mag - 14.0));

	float e = exposure;
	//e = 0.00392156862 / pow(10.0, 0.4 * (-6.5 - 19.0));

	//e = 1.0 / pow(10.0, 0.4 * (-6.5 - 19.0));

	float mul = e * irradiance / pixelSolidAngle;

	//mul = changeRange(mul, 1.0/255.0, 10000.0/255.0, 2.0/255.0, 1.0);
	//if(mul < 2.0 / 255.0) mul = 0.0;
	//mul = max(mul, 0.0);

	if(mul < 10000.0)
	{
		float b = 0.7;
		mul=pow(mul,b)*pow(1.0/10000.0,b);
	}
	else
	{
		mul /= 10000.0;
	}

	vec3 col = mul * color;
	/* TONE MAPPING */
	// col = col / (col + vec3(1.0));
	// col = log(col+vec3(1.0))/log(col+vec3(2.0));

	//col = RGBtoxyY(col);
	//col.b = log(col.b + 0.00392927) / log(10000.00392927);
	//col.b = col.b / (col.b + 1.0);
	//col = xyYtoRGB(col);

	//col = xyYtoRGB(RGBtoxyY(col));

	float m = max3(col);
	f_pointsize = sqrt(m);
	if(m > 1.0)
	{
		col /= m;
		gl_PointSize = min(256.0, sqrt(m) * 33.55263157894737 / 2.0);
		f_finalcolor = vec4(col, 1.0);
	}
	else
	{
		gl_PointSize = 1.0;
		f_finalcolor = vec4(col / mul, mul);
	}
}
