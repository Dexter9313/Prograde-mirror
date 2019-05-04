#version 150 core

//	Simplex 4D Noise
//	by Ian McEwan, Ashima Arts
//
vec4 permute(vec4 x)
{
	return mod(((x * 34.0) + 1.0) * x, 289.0);
}
float permute(float x)
{
	return floor(mod(((x * 34.0) + 1.0) * x, 289.0));
}
vec4 taylorInvSqrt(vec4 r)
{
	return 1.79284291400159 - 0.85373472095314 * r;
}
float taylorInvSqrt(float r)
{
	return 1.79284291400159 - 0.85373472095314 * r;
}

vec4 grad4(float j, vec4 ip)
{
	const vec4 ones = vec4(1.0, 1.0, 1.0, -1.0);
	vec4 p, s;

	p.xyz = floor(fract(vec3(j) * ip.xyz) * 7.0) * ip.z - 1.0;
	p.w   = 1.5 - dot(abs(p.xyz), ones.xyz);
	s     = vec4(lessThan(p, vec4(0.0)));
	p.xyz = p.xyz + (s.xyz * 2.0 - 1.0) * s.www;

	return p;
}

float snoise(vec4 v)
{
	const vec2 C = vec2(0.138196601125010504,  // (5 - sqrt(5))/20  G4
	                    0.309016994374947451); // (sqrt(5) - 1)/4   F4
	// First corner
	vec4 i  = floor(v + dot(v, C.yyyy));
	vec4 x0 = v - i + dot(i, C.xxxx);

	// Other corners

	// Rank sorting originally contributed by Bill Licea-Kane, AMD (formerly
	// ATI)
	vec4 i0;

	vec3 isX  = step(x0.yzw, x0.xxx);
	vec3 isYZ = step(x0.zww, x0.yyz);
	//  i0.x = dot( isX, vec3( 1.0 ) );
	i0.x   = isX.x + isX.y + isX.z;
	i0.yzw = 1.0 - isX;

	//  i0.y += dot( isYZ.xy, vec2( 1.0 ) );
	i0.y += isYZ.x + isYZ.y;
	i0.zw += 1.0 - isYZ.xy;

	i0.z += isYZ.z;
	i0.w += 1.0 - isYZ.z;

	// i0 now contains the unique values 0,1,2,3 in each channel
	vec4 i3 = clamp(i0, 0.0, 1.0);
	vec4 i2 = clamp(i0 - 1.0, 0.0, 1.0);
	vec4 i1 = clamp(i0 - 2.0, 0.0, 1.0);

	//  x0 = x0 - 0.0 + 0.0 * C
	vec4 x1 = x0 - i1 + 1.0 * C.xxxx;
	vec4 x2 = x0 - i2 + 2.0 * C.xxxx;
	vec4 x3 = x0 - i3 + 3.0 * C.xxxx;
	vec4 x4 = x0 - 1.0 + 4.0 * C.xxxx;

	// Permutations
	i        = mod(i, 289.0);
	float j0 = permute(permute(permute(permute(i.w) + i.z) + i.y) + i.x);
	vec4 j1 = permute(permute(permute(permute(i.w + vec4(i1.w, i2.w, i3.w, 1.0))
	                                  + i.z + vec4(i1.z, i2.z, i3.z, 1.0))
	                          + i.y + vec4(i1.y, i2.y, i3.y, 1.0))
	                  + i.x + vec4(i1.x, i2.x, i3.x, 1.0));
	// Gradients
	// ( 7*7*6 points uniformly over a cube, mapped onto a 4-octahedron.)
	// 7*7*6 = 294, which is close to the ring size 17*17 = 289.

	vec4 ip = vec4(1.0 / 294.0, 1.0 / 49.0, 1.0 / 7.0, 0.0);

	vec4 p0 = grad4(j0, ip);
	vec4 p1 = grad4(j1.x, ip);
	vec4 p2 = grad4(j1.y, ip);
	vec4 p3 = grad4(j1.z, ip);
	vec4 p4 = grad4(j1.w, ip);

	// Normalise gradients
	vec4 norm = taylorInvSqrt(
	    vec4(dot(p0, p0), dot(p1, p1), dot(p2, p2), dot(p3, p3)));
	p0 *= norm.x;
	p1 *= norm.y;
	p2 *= norm.z;
	p3 *= norm.w;
	p4 *= taylorInvSqrt(dot(p4, p4));

	// Mix contributions from the five corners
	vec3 m0 = max(0.6 - vec3(dot(x0, x0), dot(x1, x1), dot(x2, x2)), 0.0);
	vec2 m1 = max(0.6 - vec2(dot(x3, x3), dot(x4, x4)), 0.0);
	m0      = m0 * m0;
	m1      = m1 * m1;
	return 49.0
	       * (dot(m0 * m0, vec3(dot(p0, x0), dot(p1, x1), dot(p2, x2)))
	          + dot(m1 * m1, vec2(dot(p3, x3), dot(p4, x4))));
}

float smoothNoise(vec4 v, int octaves, float frequency, float persistence)
{
	float total        = 0.0; // Total value so far
	float maxAmplitude = 0.0; // Accumulates highest theoretical amplitude
	float amplitude    = 1.0;
	for(int i = 0; i < octaves; i++)
	{
		// Get the noise sample
		total += snoise(v * frequency) * amplitude;

		// Make the wavelength twice as small
		frequency *= 2.0;

		// Add to our maximum possible amplitude
		maxAmplitude += amplitude;

		// Reduce amplitude according to persistence for the next octave
		amplitude *= persistence;
	}

	// Scale the result by the maximum amplitude
	return total / maxAmplitude;
}

// CRATERS

float noise1(vec3 p)
{
	p = fract(p * vec3(233.42, 865.32, 1234.0));
	p += dot(p, p + 32.33);
	return fract(p.x * p.y * p.z);
}

vec2 noise2(vec3 p)
{
	float n = noise1(p);
	return vec2(n, noise1(p + n));
}

vec3 noise3(vec3 p)
{
	float n = noise1(p);
	return vec3(n, noise2(p + n));
}

// get a random position for cellId id
// position is within the cube [-0.4;0.4]^3
vec3 getPos(vec3 id)
{
	float seed = 3.0;

	vec3 noise = noise3(id / 1000.0);

	float x = sin(noise.x * 100000.0 * seed + 2348.0);
	float y = cos(noise.y * 100000.0 * seed + 2424.0);
	float z = cos(noise.z * 100000.0 * seed + 1243.0);

	return vec3(x, y, z) * min(0.4, noise1(id));
}

float clampDerivative(in float x, in float a, in float b)
{
	if(x <= a || x >= b)
	{
		return 0.0;
	}
	return 1.0;
}

// Derivative of the crater height function (see terrestrialdiff)
float craterDerivative(in float len)
{
	float pi = 3.1415926535;
	float x  = clamp(pow(len, 4.0) * 8.0, pi * 0.5, pi * 3.5 / 2.0);
	float dxdl
	    = 32.0 * pow(len, 3.0)
	      * clampDerivative(pow(len, 4.0) * 8.0, pi * 0.5, pi * 3.5 / 2.0);
	float t    = clamp(len, 0.0, 1.0);
	float dtdl = clampDerivative(len, 0.0, 1.0);

	// derivative of radius over t
	// return (-32.0*pow(t,3.0)*cos(x) + 1.5708 * sin(pi*t));
	return -0.666666666 * dxdl * cos(x) + pi * dtdl * sin(pi * t) / 3.0;
}

vec3 craterNormal(in vec3 pos, in float size)
{
	float pi  = 3.1415926535;
	float foo = 540.31;

	vec3 mpos = pos * size + foo;

	vec3 gridUv = fract(mpos) - .5;
	vec3 cellId = floor(mpos);

	vec3 p        = getPos(cellId);
	vec3 pworld   = normalize(p + 0.5 + floor(mpos) - foo);
	vec3 posworld = normalize(pos);

	float len = length(gridUv - p) * 10.0;

	// https://en.wikipedia.org/wiki/Azimuth#Calculating_azimuth
	float phi1 = sin(pworld.z);
	float phi2 = sin(posworld.z);
	float L    = atan(pworld.y, pworld.x) - atan(posworld.y, posworld.x);

	float angle = atan(sin(L), (cos(phi1) * tan(phi2) - sin(phi1) * cos(L)));
	angle       = pi / 2.0 - angle;

	// derivative of radius over t
	float drdt = craterDerivative(len) * noise1(cellId);

	vec3 tangent = normalize(vec3(1.0, 0.0, drdt));

	vec3 normal = cross(tangent, vec3(0.0, 1.0, 0.0));

	normal.xy
	    = mat2(vec2(cos(angle), sin(angle)), vec2(-sin(angle), cos(angle)))
	      * normal.xy;

	return normal;
}

vec3 normalmap(in vec3 pos)
{
	vec3 normal = vec3(0.0, 0.0, 1.0);

	int iter = 100;
	for(int i = 0; i < iter; ++i)
	{
		vec3 curNorm = craterNormal(
		    pos, 4.0 * sqrt(float(i + 1))); // * pow(1.0, -1.0*float(i));
		normal = mix(curNorm, normal, curNorm.z * 0.5 + 0.5);
	}
	// normal /= float(iter);

	return normalize(normal);
}

in vec3 f_position;
in mat3 f_tantoworld;

uniform vec3 color;
uniform float seed;
uniform float polarLatitude;

out vec4 outColor;

void main()
{
	float nx = smoothNoise(vec4(f_position, 2000.0 + seed), 24, 10.0, 0.9);
	float ny = smoothNoise(vec4(f_position, 3000.0 + seed), 24, 10.0, 0.9);
	float nz = smoothNoise(vec4(f_position, 4000.0 + seed), 24, 10.0, 0.9);

	vec3 mountainishNormal
	    = normalize(vec3(nx * 0.3, ny * 0.3, nz * 0.5 + 0.5));
	vec3 normal = normalmap(f_position);
	normal.x *= -1.0;

	normal = f_tantoworld * normal;

	outColor.xyz = (mountainishNormal * 1.1 + normal * 0.9) * 0.25 + 0.5;
	// outColor.xyz = f_tantoworld * vec3(0.5, 0.5, 1.0);
	outColor.a = 1.0;
}
