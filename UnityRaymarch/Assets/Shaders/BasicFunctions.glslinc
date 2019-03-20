
// ------------------ 
// Rotations 
// ------------------
mat2 rot2(float th) { vec2 a = sin(vec2(1.5707963, 0) + th); return mat2(a, -a.y, a.x); }
mat3 rotationMatrix(vec3 axis, float angle) {
	axis = normalize(axis);
	float s = sin(angle);
	float c = cos(angle);
	float oc = 1.0 - c;

	return mat3(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s,
		oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s,
		oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c);
}

// ------------------ 
// Noises 
// ------------------
float Noise(vec2 p)
{
	vec2 s = sin(p * 0.6345) + sin(p * 1.62423);
	return dot(s, vec2(0.125)) + 0.5;
}
vec2 hash(vec2 p)
{
	p = vec2(dot(p, vec2(127.1, 311.7)),
		dot(p, vec2(269.5, 183.3)));

	return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}
float rand(vec2 co) {
	return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}
float rand(float co) {
	return rand(vec2(co));
}
float perlinnoise(in vec2 p)
{
	float K1 = 0.366025404;
	float K2 = 0.211324865;
	vec2 i = floor(p + (p.x + p.y)*K1);
	vec2 a = p - i + (i.x + i.y)*K2;
	vec2 o = (a.x > a.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
	vec2 b = a - o + K2;
	vec2 c = a - 1.0 + 2.0*K2;
	vec3 h = max(0.5 - vec3(dot(a, a), dot(b, b), dot(c, c)), 0.0);
	vec3 n = h * h*h*h*vec3(dot(a, hash(i + 0.0)), dot(b, hash(i + o)), dot(c, hash(i + 1.0)));
	return dot(n, vec3(70.0));
}

// ------------------ 
// Smoothing 
// ------------------
float smin(float a, float b, float k) {
	float res = exp(-k * a) + exp(-k * b);
	return -log(res) / k;
}
float length2(vec2 p) {
	return sqrt(p.x*p.x + p.y*p.y);
}
float length6(vec2 p) {
	p = p * p*p; p = p * p;
	return pow(p.x + p.y, 1.0 / 6.0);
}
float length8(vec2 p) {
	p = p * p; p = p * p; p = p * p;
	return pow(p.x + p.y, 1.0 / 8.0);
}

// ------------------ 
// Basic Math for Vectors
// ------------------ 
//Sign function that doesn't return 0
float sgn(float x) {
	return (x < 0) ? -1 : 1;
}

vec2 sgn(vec2 v) {
	return vec2((v.x < 0) ? -1 : 1, (v.y < 0) ? -1 : 1);
}

float square(float x) {
	return x * x;
}

vec2 square(vec2 x) {
	return x * x;
}

vec3 square(vec3 x) {
	return x * x;
}

float lengthSqr(vec3 x) {
	return dot(x, x);
}


// ------------------ 
// Maximum/minumum elements of a vector
// ------------------ 

float vmax(vec2 v) {
	return max(v.x, v.y);
}

float vmax(vec3 v) {
	return max(max(v.x, v.y), v.z);
}

float vmax(vec4 v) {
	return max(max(v.x, v.y), max(v.z, v.w));
}

float vmin(vec2 v) {
	return min(v.x, v.y);
}

float vmin(vec3 v) {
	return min(min(v.x, v.y), v.z);
}

float vmin(vec4 v) {
	return min(min(v.x, v.y), min(v.z, v.w));
}

// ------------------ 
// special variable exponents
// ------------------ 

const vec3 GDFVectors[19] = vec3[](
	normalize(vec3(1, 0, 0)),
	normalize(vec3(0, 1, 0)),
	normalize(vec3(0, 0, 1)),

	normalize(vec3(1, 1, 1)),
	normalize(vec3(-1, 1, 1)),
	normalize(vec3(1, -1, 1)),
	normalize(vec3(1, 1, -1)),

	normalize(vec3(0, 1, PHI + 1)),
	normalize(vec3(0, -1, PHI + 1)),
	normalize(vec3(PHI + 1, 0, 1)),
	normalize(vec3(-PHI - 1, 0, 1)),
	normalize(vec3(1, PHI + 1, 0)),
	normalize(vec3(-1, PHI + 1, 0)),

	normalize(vec3(0, PHI, 1)),
	normalize(vec3(0, -PHI, 1)),
	normalize(vec3(1, 0, PHI)),
	normalize(vec3(-1, 0, PHI)),
	normalize(vec3(PHI, 1, 0)),
	normalize(vec3(-PHI, 1, 0))
	);

// Version with variable exponent.
// This is slow and does not produce correct distances, but allows for bulging of objects.
float fGDF(vec3 p, float r, float e, int begin, int end) {
	float d = 0;
	for (int i = begin; i <= end; ++i)
		d += pow(abs(dot(p, GDFVectors[i])), e);
	return pow(d, 1 / e) - r;
}

// Version with without exponent, creates objects with sharp edges and flat faces
float fGDF(vec3 p, float r, int begin, int end) {
	float d = 0;
	for (int i = begin; i <= end; ++i)
		d = max(d, abs(dot(p, GDFVectors[i])));
	return d - r;
}