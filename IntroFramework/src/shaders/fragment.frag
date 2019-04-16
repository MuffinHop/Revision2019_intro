#version 130


uniform sampler2D _TextTex;
uniform float _iTime;
uniform vec4 _iMouse;
uniform vec4 _iResolution;
uniform vec4 _DirectionalLight;
uniform vec4 _DirectionalLightColor;
uniform vec4 _PointLightPosition;
uniform vec4 _PointLightColor;
uniform vec4 _CameraPosition;
uniform vec4 _CameraLookAt;
uniform vec4 _CameraUp;
uniform float _FOV;
uniform float _Distance;
uniform float _LensCoeff;
uniform float _MaxCoC;
uniform float _RcpMaxCoC;
uniform float _MarchMinimum;
uniform float _FarPlane;
uniform float _Step;
uniform float fogDensity;
uniform float _TextId;
uniform float _Environment;
uniform float _StepIncreaseByDistance;
uniform float _StepIncreaseMax;
#define maxItersGlobal 48
#define ENABLE_FOG
#define ENABLE_REFLECTIONS
#define ENABLE_SHADOWS
#define ENABLE_TRANSPARENCY
#define ENABLE_AO
#define DOUBLE_SIDED_TRANSPARENCY
#define saturate(x) clamp(x, 0, 1)

#define FRAGMENT_P lowp
//#define DEBUG_STEPS 1
const float PI = 3.14159265359;
const float DEG_TO_RAD = PI / 180.0;
const float TWOPI = PI * 2.0;
const float TAU = PI * 2.0;
const float PHI = (sqrt(5)*0.5 + 0.5);
const float farClip = 32.0;

const float noTransparency = -1.0;
const float transparencyInformation = 1.0;
const float emptyInformation = 0.0;

struct Trace
{
	vec3 origin;
	vec3 direction;
	float startdistanc;
	float length;
};

struct Surface
{
	vec3 normal;
	vec3 reflection;
	vec3 subsurface;
};

struct Material
{
	float reflectionCoefficient;
	float smoothness;
	float transparency;
	float reflectindx;
	vec3 albedo;
};

struct Shading
{
	vec3 diffuse;
	vec3 specular;
};

struct ContactInfo
{
	vec3 position;
	float distanc;
	vec3 id;
};

struct PointLight
{
	vec3 position;
	vec3 color;
};

struct DirectionLight
{
	vec3 direction;
	vec3 color;
};

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

mat3 RotateQuaternion (vec4 q) 
{
  mat3 m;
  float a1, a2, s;
  s = q.w * q.w - 0.5;

  return mat3(q.x * q.x + s, q.x * q.y - q.z * q.w, q.x * q.z - q.y * q.w,
	  q.x * q.y + q.z * q.w, q.y * q.y + s, q.y * q.z + q.x * q.w,
	  q.x * q.z + q.y * q.w, q.y * q.z - q.x * q.w, q.z * q.z + s);
  return 2. * m;
}

// ------------------ 
// Noises 
// ------------------
float Noise(vec2 p)
{
	vec2 s = sin(p * 0.6345) + sin(p * 1.62423);
	return dot(s, vec2(0.125)) + 0.5;
}

const float kHashScale1 = 443.8975;


float hash11(float p) {
  vec3 p3 = fract(vec3(p) * kHashScale1);
  p3 += dot(p3, p3.yyx + 0.5);
  return fract(p3.y*p3.z*p3.z*0.2)-fract(p3.y*p3.z*1.);
}

vec2 hash(vec2 p)
{
	p = vec2(dot(p, vec2(127.1, 311.7)),
		dot(p, vec2(269.5, 183.3)));

	return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}
float hash1(float h)
{
  return fract(sin(h) * 43758.5453123);
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
// A hash function for some noise
float hash12(vec2 p)
{
  vec3 p3  = fract(vec3(p.xyx) * .1031);
  p3 += dot(p3, p3.yzx + 19.19);
  return fract((p3.x + p3.y) * p3.z);
}

// Some magic values for a pretty filmic grain
vec3 filmgrain(vec3 color) {
    float noiseR = ((hash12(gl_FragCoord.xy + 0.00111*_iTime)-0.5) / 4.2 * (0.125+color.r*0.25));
    float noiseG = ((hash12(gl_FragCoord.xy + 0.00123*_iTime)-0.5) / 4.2 * (0.125+color.g*0.25));
    float noiseB = ((hash12(gl_FragCoord.xy + 0.00132*_iTime)-0.5) / 4.2 * (0.125+color.b*0.25));
    return color + vec3(noiseR,noiseG,noiseB);
}

// ------------------ 
// Others 
// ------------------

float GetHeightmap(vec3 p)
{
    float hills;

    // Combine from components
    float hill_1 = 8.0 * perlinnoise(0.1 * p.xz); // High amplitude, very low frequency
    float hill_2 = 4.0 * perlinnoise(0.5 * p.xz); // Medium amplitude, low frequency
    float hill_3 = 2.0 * perlinnoise(1.0 * p.xz); // Small amplitude, high frequency
    float hill_4 = 1.0 * perlinnoise(2.0 * p.xz); // Tiny amplitude, very high frequency
    
    hills += hill_1;
    hills += hill_2;
    hills += hill_3;
    hills += hill_4; // Optional, if performance hit is too large

	return  hills;
}

float GetHeightmapLowPrecision(vec3 p)
{
    float hills;

    // Combine from components
    float hill_1 = 8.0 * perlinnoise(0.1 * p.xz); // High amplitude, very low frequency 

    hills += hill_1;

	return  hills;
}


void pR(inout vec2 p, float a) {
	p = cos(a)*p + sin(a)*vec2(p.y, -p.x);
}

float celli(in vec3 p) { p = fract(p) - .5; return dot(p, p); }

float cellTile(in vec3 p) {
	vec4 d;
	d.x = celli(p - vec3(.81, .62, .53));
	p.xy = vec2(p.y - p.x, p.y + p.x)*.7071;
	d.y = celli(p - vec3(.39, .2, .11));
	p.yz = vec2(p.z - p.y, p.z + p.y)*.7071;
	d.z = celli(p - vec3(.62, .24, .06));
	p.xz = vec2(p.z - p.x, p.z + p.x)*.7071;
	d.w = celli(p - vec3(.2, .82, .64));
	d.xy = min(d.xz, d.yw);
	return min(d.x, d.y)*2.66;
}

float hex(vec2 p) {
    p.x *= 0.57735*2.0;
	p.y += mod(floor(p.x), 2.0)*0.5;
	p = abs((mod(p, 1.0) - 0.5));
	return abs(max(p.x*1.5 + p.y, p.y*2.0) - 1.0);
}

float cellTile2(in vec3 p){
    vec4 d; 
    d.x = celli(p - vec3(.81, .62, .53));
    p.xy = vec2(p.y-p.x, p.y + p.x)+hex(p.xy*0.2);
    d.y = celli(p - vec3(.39, .2, .11));
    p.yz = vec2(p.z-p.y, p.z + p.y)+hex(p.yz*0.2);
    d.z = celli(p - vec3(.62, .24, .06));
    p.xz = vec2(p.z-p.x, p.z + p.x)+hex(p.xz*0.2);
    d.w = celli(p - vec3(.2, .82, .64));
    d.xy = min(d.xz, d.yw);
    return min(d.x, d.y)*0.5; 
}
vec4 skybox(vec3 coord, float stretch) {
    coord.y+=0.2;
    float tsky = _iTime*(100./stretch);
    vec4 color = vec4(0.);
	vec3 coord2;
    coord2 = coord;
	vec3 coord3;
    coord3 = coord;
	vec4 color2 = vec4(0.);
    for(float i=0.; i<600.; i+=8.0) {
        coord2 = coord;
        float tiam = i/stretch+tsky;
    	coord2 *= rotationMatrix(vec3(0.0,0.6,0.1), tiam);
        float v = perlinnoise(coord2.zx*44.);
        color2 += min(max(coord.y*12.-5.,0.)*0.2,1.) * 
        (vec4(1.,0.95,0.9,1.)*(1.-v)+vec4(0.9,0.95,1.0,1.)*v)*max(perlinnoise( coord2.zx*233. )-0.67,0.0)*32.*i;
        
        /*coord2 += vec3(cellTile2(coord2+tiam*1.3),cellTile2(coord2+tiam*2.2+.1),cellTile2(coord2+tiam*1.3+.3));
    	color2 += min(max(coord.y*12.-4.,0.),1.) * min(max(coord2.y*12.-4.,0.),1.) * vec4(0.,1.,0.,1.) * pow(distance(cellTile2((coord2+vec3(.0,.13,.0))),cellTile2(coord2)),4.)*stretch*stretch * 0.01 * i;
    	color2 += min(max(coord.y*12.-4.,0.),1.) * min(max(coord2.y*12.-4.,0.),1.) * vec4(.8,0.4,0.,1.) * pow(distance(cellTile2((coord2+vec3(.0,.1,.0))),cellTile2(coord2)),4.)*stretch*stretch * 0.01 * i;
    */
    }
    color2 = max(color2,vec4(0.));
    color2 /= stretch;
    color += color2;
    float sd = dot(normalize(vec3(-0.5, -0.6, 0.9)), coord)*0.5+0.5;
    sd = pow(sd, 5.);
    vec3 col = mix(vec3(0.05,0.1,0.2), vec3(0.1,0.05,0.2), coord);
    color.rgb += col/64.;
    return color;
}
vec3 skyboxSample() {
     vec2 uv = 2.0 * gl_FragCoord.xy / _iResolution.xy - 1.0;
     float aspect = _iResolution.x / _iResolution.y;
     vec3 direction = normalize( vec3(.5 * uv * vec2( aspect, 1.0), 1.0 ) );
    direction *= rotationMatrix(vec3(0.0,1.0,0.0), _iMouse.x);
    direction *= rotationMatrix(vec3(1.0,0.0,0.0), _iMouse.y);
     vec4 fragColor = pow(skybox(direction*3.141592*2.*0.1, 8220.-_iTime*145.),vec4(1./2.3));
    return fragColor.rgb;
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

float maxMinus(float a, float b) {
 return max(-a,b);
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
vec4 DistUnionCombine(in vec4 v1, in vec4 v2)	{
 return mix(v1, v2, step(v2.x, v1.x));
}
vec4 DistUnionCombineTransparent(in vec4 v1, in vec4 v2, in float transparencyPointer)
{
	vec4 vScaled = vec4(v2.x * (transparencyPointer * 2.0 - 1.0), v2.yzw);
	return mix(v1, vScaled, step(vScaled.x, v1.x) * step(0.0, transparencyPointer));
}


PointLight GetPointLight() {
	PointLight result;
	result.position = _PointLightPosition.xyz;
	result.color = _PointLightColor.xyz;

	result.position = -result.position;

	return result;
}

DirectionLight GetDirectionLight() {
	DirectionLight result;
	result.direction = _DirectionalLight.xyz;
	result.color = _DirectionalLightColor.xyz;
	result.direction = -result.direction;
	result.color *= 0.123;
	return result;
}

// Repeat space along one axis. Use like this to repeat along the x axis:
// <float cell = pMod1(p.x,5);> - using the return value is optional.
float pMod1(inout float p, float size) {
	float halfsize = size * 0.5;
	float c = floor((p + halfsize) / size);
	p = mod(p + halfsize, size) - halfsize;
	return c;
}
// Shortcut for 45-degrees rotation
void pR45(inout vec2 p) {
	p = (p + vec2(p.y, -p.x))*sqrt(0.5);
}
float getTilt(vec3 p) { return dot(p, vec3(0.299, 0.587, 0.114)); }

vec3 tex3D(sampler2D tex, in vec3 p, in vec3 n) {

	n = max(n*n, 0.001);
	n /= (n.x + n.y + n.z);

	return (texture(tex, p.yz)*n.x + texture(tex, p.zx)*n.y + texture(tex, p.xy)*n.z).xyz;
}

vec3 doDisplacement(sampler2D tex, in vec3 p, in vec3 normal, float bumpPower) {

	float epsilon = 0.001;
	vec3 grad = vec3(getTilt(tex3D(tex, vec3(p.x - epsilon, p.y, p.z), normal)),
		getTilt(tex3D(tex, vec3(p.x, p.y - epsilon, p.z), normal)),
		getTilt(tex3D(tex, vec3(p.x, p.y, p.z - epsilon), normal)));

	grad = (grad - getTilt(tex3D(tex, p, normal))) / epsilon;
	grad -= normal * dot(normal, grad);
	return normalize(normal + grad * bumpPower);
}

const float material_ID0 = 1;

const float material_ID1 = 2;

const float material_ID2 = 3;

const float material_ID3 = 4;

const float material_ID4 = 5;

const float material_ID5 = 6;

const float material_ID6 = 7;

 uniform float _Objects[250];
float fSphere(vec3 p, float r) {
	return length(p) - r;
}
float sdCapsule(vec3 p, vec3 a, vec3 b, float r)
{
	vec3 pa = p - a, ba = b - a;
	float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
	return length(pa - ba * h) - r;
}
// Box: correct distance to corners
float fBox(vec3 p, vec3 b) {
	vec3 d = abs(p) - b;
	return length(max(d, vec3(0))) + vmax(min(d, vec3(0)));
}
float Wheat(vec3 pos, vec3 algorithm) {
    float vali = 5.;
    float ps = min(perlinnoise(pos.xz*0.1 / vali) - 0.2,0.0);
	pos.y-= (1.0+ps) * _Environment * 1000.0;
	pos.x+=cos(algorithm.z+pos.y*7.+pos.x*5.)*0.03;
	pos.z-=0.5;
	vec3 opos = pos;
	opos.x = mod(opos.x, algorithm.z) - algorithm.z / 2.0;
	opos.x+= mod(opos.z*1.5, algorithm.y)*algorithm.x;
	opos.z = mod(opos.z, algorithm.z) - algorithm.z / 2.0;
	opos.z-= mod(opos.z, algorithm.z/2.)*0.1;
	float dist = fBox(opos + vec3(0.2,0.0,0.2), vec3(0.002, 0.2, 0.003));
	return dist;
}

float sdBox(vec3 p, vec3 b)
{
	vec3 d = abs(p) - b;
	return length(max(d, 0.0))
		+ min(max(d.x, max(d.y, d.z)), 0.0); // remove this line for an only partially signed sdf 
}
float Smoke(vec3 pos, vec3 algorithm) {
	vec3 opos = pos;
	vec3 opos_ = opos;
	
	float i = 0.0;

	float distance5 = 1.0;

	return min(distance5,sdBox(opos+vec3(fract(-opos.z*0.5+(0.2+0.5*cos(fract(opos.z*0.2)*algorithm.x*0.1)*0.2)*opos.y)*sin(opos.y+i*1.+opos.z*4.+algorithm.x)*1.+1.0,0.,i*0.01),vec3((2.0-opos.y)*0.03*abs(cos(opos.y*2.+algorithm.x)),3.0+fract(i)*1.,0.9)));

    return distance5;
}
float Water(vec3 pos, vec3 algorithm) {
	pos.y += perlinnoise(pos.xz*3.)*0.025;
	float dist = fBox(pos, algorithm);
	return dist;
}


float fOpUnionRund(float a, float b, float r) {
	vec2 u = max(vec2(r - a,r - b), vec2(0));
	return max(r, min (a, b)) - length(u);
}
float TreeTrunk(vec3 pos, vec3 algorithm) {
    float vali = 5.;
    float ps = perlinnoise(pos.xz*0.1 / vali) - 0.2;
	pos.y-= ps * _Environment * 1000.0;
	vec3 opos = pos;
	vec3 opos_ = opos;
	
    opos.x += hash1(floor(pos.z / vali) + vali) * vali / 2.0;
    opos.x = mod(opos.x, vali) - vali / 2.0;
    opos.z += hash1(floor(pos.x / vali) + vali) * vali / 2.0;
    opos.z = mod(opos.z, vali) - vali / 2.0;
    pR(opos.xz, pos.z*1.);
    pR(opos.zy, 3.14);
	vec3 rpos = pos;
	rpos.x = floor(pos.x / vali);
	rpos.z = floor(pos.z / vali);
    float ss = min(perlinnoise(rpos.xz*0.1) * 12.0,2.0);

	float height = 0.0;
	opos.y += 0.75;
	float dist = fBox(opos + vec3(0.0,height,0.0), ss*vec3(0.08 + atan(opos.y*2.)*0.02, 1.5, 0.05));
	opos.y -= 0.75;
	vec3 o = opos + vec3(0.0, height, 0.0);
	pR(opos.yx, 5.0);
	opos *= rotationMatrix(vec3(1.0,0.2,0.0), rpos.x + rpos.z) ;
	dist = fOpUnionRund(dist, fBox(opos + vec3(-0.1 + 0.1, 0.0, 0.0), ss*vec3(0.03, 0.5, 0.02)), 0.2);
	opos = o;
	return dist;
}
float sdSphere(vec3 p, float s)
{
	return length(p) - s;
}


float TreeBush(vec3 pos, vec3 algorithm) {
    float vali = 5.;
    float ps = perlinnoise(pos.xz*0.1 / vali) - 0.2;
	pos.y += GetHeightmapLowPrecision(pos*algorithm) * algorithm.y - ps * _Environment;
    vec3 opos = pos;
    
    opos.x += hash1(floor(pos.z / vali) + vali) * vali / 2.0;
    opos.x = mod(opos.x, vali) - vali / 2.0;
    opos.z += hash1(floor(pos.x / vali) + vali) * vali / 2.0;
    opos.z = mod(opos.z, vali) - vali / 2.0;
    pR(opos.xz, pos.z*1.);
    pR(opos.zy, 3.14);
	vec3 rpos = pos;
	rpos.x = floor(pos.x / vali);
	rpos.z = floor(pos.z / vali);
    float ss = min(perlinnoise(rpos.xz*0.1) * 12.0,1.5);
    vec3 o = opos;
    opos = o;
	opos.y += GetHeightmapLowPrecision(pos*algorithm) *  algorithm.y;
	opos.y += 3.0;
    float distance2 = sdSphere(opos,ss);
    
	pos.y += GetHeightmapLowPrecision(pos*algorithm) * algorithm.y;
    distance2 = min(distance2,fBox(pos+vec3(0.0,2.5,0.0), vec3(1111.,2.,1111.)));
    distance2*=0.2;
    distance2+= cellTile(pos)*0.1;
    distance2+= cellTile(pos*9.0)*0.025;
    return distance2;
}

// The "Round" variant uses a quarter-circle to join the two objects smoothly:
float fOpUnionRound(float a, float b, float r) {
	vec2 u = max(vec2(r - a, r - b), vec2(0));
	return max(r, min(a, b)) - length(u);
}
vec4 GetDistanceScene(vec3 position, in float transparencyPointer)
        {
            vec4 result = vec4(10000.0, -1.0, 0.0, 0.0);
        
         float id0_distance = 1e9;

//Sphere
               vec3 posID0 = position - vec3(_Objects[0], _Objects[1], _Objects[2]);
               id0_distance  = min(fSphere(posID0,_Objects[3]), id0_distance);

//Cube (1)
               vec3 posID9 = position - vec3(_Objects[90], _Objects[91], _Objects[92]);
               id0_distance  = min(Water(posID9, vec3(_Objects[93], _Objects[94],_Objects[95])), id0_distance);

//Sphere (1)
               vec3 posID13 = position - vec3(_Objects[130], _Objects[131], _Objects[132]);
               id0_distance  = min(fSphere(posID13,_Objects[133]), id0_distance);

//Sphere (2)
               vec3 posID17 = position - vec3(_Objects[170], _Objects[171], _Objects[172]);
               id0_distance  = min(fSphere(posID17,_Objects[173]), id0_distance);

//Sphere
               vec3 posID18 = position - vec3(_Objects[180], _Objects[181], _Objects[182]);
               id0_distance  = min(fSphere(posID18,_Objects[183]), id0_distance);
               vec4 distID0 = vec4(id0_distance, material_ID0, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombineTransparent(result, distID0, transparencyPointer);

         float id1_distance = 1e9;

//LeftShoulder
               id1_distance  = min(sdCapsule(position, vec3(_Objects[10], _Objects[11],_Objects[12]), vec3(_Objects[13], _Objects[14],_Objects[15]), 0.1), id1_distance);

//RightForeArm
               id1_distance  = min(sdCapsule(position, vec3(_Objects[20], _Objects[21],_Objects[22]), vec3(_Objects[23], _Objects[24],_Objects[25]), 0.06), id1_distance);

//LeftArm
               id1_distance  = min(sdCapsule(position, vec3(_Objects[30], _Objects[31],_Objects[32]), vec3(_Objects[33], _Objects[34],_Objects[35]), 0.08), id1_distance);

//RightUpLeg
               id1_distance  = min(sdCapsule(position, vec3(_Objects[40], _Objects[41],_Objects[42]), vec3(_Objects[43], _Objects[44],_Objects[45]), 0.1), id1_distance);

//LeftForeArm
               id1_distance  = min(sdCapsule(position, vec3(_Objects[50], _Objects[51],_Objects[52]), vec3(_Objects[53], _Objects[54],_Objects[55]), 0.06), id1_distance);

//Cube Ray Marched (1)
               vec3 posID7 = position - vec3(_Objects[70], _Objects[71], _Objects[72]);
               posID7= RotateQuaternion(vec4(_Objects[76], _Objects[77], _Objects[78], - _Objects[79]))*posID7;
               id1_distance  = min(fBox(posID7, vec3(_Objects[73], _Objects[74],_Objects[75])), id1_distance);

//RightArm
               id1_distance  = min(sdCapsule(position, vec3(_Objects[100], _Objects[101],_Objects[102]), vec3(_Objects[103], _Objects[104],_Objects[105]), 0.08), id1_distance);

//GameObject
               vec3 posID12 = position - vec3(_Objects[120], _Objects[121], _Objects[122]);
               id1_distance  = fOpUnionRound(fSphere(posID12,_Objects[123]), id1_distance,0.12);

//LeftUpLeg
               id1_distance  = min(sdCapsule(position, vec3(_Objects[140], _Objects[141],_Objects[142]), vec3(_Objects[143], _Objects[144],_Objects[145]), 0.1), id1_distance);

//Spine
               id1_distance  = min(sdCapsule(position, vec3(_Objects[160], _Objects[161],_Objects[162]), vec3(_Objects[163], _Objects[164],_Objects[165]), 0.15), id1_distance);

//LeftHand
               id1_distance  = min(sdCapsule(position, vec3(_Objects[190], _Objects[191],_Objects[192]), vec3(_Objects[193], _Objects[194],_Objects[195]), 0.04), id1_distance);

//GameObject
               vec3 posID20 = position - vec3(_Objects[200], _Objects[201], _Objects[202]);
               id1_distance  = fOpUnionRound(fSphere(posID20,_Objects[203]), id1_distance,0.12);

//RightHand
               id1_distance  = min(sdCapsule(position, vec3(_Objects[230], _Objects[231],_Objects[232]), vec3(_Objects[233], _Objects[234],_Objects[235]), 0.04), id1_distance);

//RightShoulder
               id1_distance  = min(sdCapsule(position, vec3(_Objects[240], _Objects[241],_Objects[242]), vec3(_Objects[243], _Objects[244],_Objects[245]), 0.1), id1_distance);
               vec4 distID1 = vec4(id1_distance, material_ID1, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombine(result, distID1);

         float id2_distance = 1e9;

//Wheat
               vec3 posID6 = position - vec3(_Objects[60], _Objects[61], _Objects[62]);
               id2_distance  = min(Wheat(posID6, vec3(_Objects[63], _Objects[64],_Objects[65])), id2_distance);

//Wheat (2)
               vec3 posID11 = position - vec3(_Objects[110], _Objects[111], _Objects[112]);
               id2_distance  = min(Wheat(posID11, vec3(_Objects[113], _Objects[114],_Objects[115])), id2_distance);
               vec4 distID2 = vec4(id2_distance, material_ID2, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombine(result, distID2);

         float id3_distance = 1e9;

//Smoke
               vec3 posID8 = position - vec3(_Objects[80], _Objects[81], _Objects[82]);
               id3_distance  = min(Smoke(posID8, vec3(_Objects[83], _Objects[84],_Objects[85])), id3_distance);
               vec4 distID3 = vec4(id3_distance, material_ID3, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombine(result, distID3);

         float id4_distance = 1e9;

//Tree Trunk Ray Marched
               #define posID15 position
               id4_distance  = min(TreeTrunk(posID15, vec3(_Objects[153], _Objects[154],_Objects[155])), id4_distance);
               vec4 distID4 = vec4(id4_distance, material_ID4, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombine(result, distID4);

         float id5_distance = 1e9;

//Cube Ray Marched
               vec3 posID21 = position - vec3(_Objects[210], _Objects[211], _Objects[212]);
               id5_distance  = min(fBox(posID21, vec3(_Objects[213], _Objects[214],_Objects[215])), id5_distance);
               vec4 distID5 = vec4(id5_distance, material_ID5, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombine(result, distID5);

         float id6_distance = 1e9;

//Tree Bush Ray Marched
               #define posID22 position
               id6_distance  = min(TreeBush(posID22, vec3(_Objects[223], _Objects[224],_Objects[225])), id6_distance);
               vec4 distID6 = vec4(id6_distance, material_ID6, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombine(result, distID6);


            return result;
        }
Material RockPattern(vec3 position) {
	vec3 _position = position;
	_position.y += abs(_position.z) / 12.;
	vec3 _position2 = _position;
	vec3 _position3 = position;
	vec3 _position4 = position;
	float dist = 1e8;
	float dist2 = 1e8;

	float heightmap = perlinnoise(_position.xz / 6.);
	heightmap += perlinnoise(_position.xz*12.) / 64.;
	heightmap += perlinnoise(_position.xz*40.) / 128.;

	dist += (perlinnoise(_position.xy / 3.)*sin(_position.z) + perlinnoise(_position.xy)*cos(_position.z)) / 3.;


	float heightmap2 = perlinnoise(_position.xz*2.) / 6.;

	float a = heightmap;
	vec3 color = vec3(1.) - 6.*vec3(.4, .6, .8)*perlinnoise(_position.xz / 6.)*perlinnoise(_position.xz*12.);


	dist -= (perlinnoise(_position.xz*120.) + perlinnoise(_position.zy*120.)) / 880.;


	vec3 kummaj = vec3(0.8, 1.0, 0.4);
	color += kummaj / (1. + 0.6*pow(length(_position - vec3(1., 0., 4.)), 2.));

	Material mat;
	mat.albedo = color;
	mat.reflectionCoefficient = 0.1;
	mat.smoothness = 0.1;
	mat.transparency = 0.0;
	mat.reflectindx = 0.1;
	return mat;
}
 Material GetObjectMaterial(in ContactInfo hitNfo)
        {
            Material mat;
            
       if (hitNfo.id.x == material_ID0){
              mat.reflectionCoefficient = 0.27;
              mat.albedo = vec3(1,1,1);;
              mat.transparency =0.5019608;
              mat.smoothness = 0.8;
              mat.reflectindx = 0.69;
       }
       if (hitNfo.id.x == material_ID1){
              mat.reflectionCoefficient = 0.178;
              mat.albedo = vec3(1,1,1);;
              mat.transparency =0;
              mat.smoothness = 0.37;
              mat.reflectindx = 0.54;
       }
       if (hitNfo.id.x == material_ID2){
              mat.reflectionCoefficient = 0.1;
              mat.albedo = vec3(0.6517754,0.6698113,0.2243236);;
              mat.transparency =0;
              mat.smoothness = 0.1;
              mat.reflectindx = 0.2;
       }
       if (hitNfo.id.x == material_ID3){
              mat.reflectionCoefficient = 0.71;
              mat.albedo = vec3(1,0,0);;
              mat.transparency =0;
              mat.smoothness = 0.84;
              mat.reflectindx = 0.8;
       }
       if (hitNfo.id.x == material_ID4){
              mat.reflectionCoefficient = 0.04;
              mat.albedo = vec3(0.6509434,0.3772959,0.2671325);;
              mat.transparency =0;
              mat.smoothness = 0.51;
              mat.reflectindx = 0.03;
       }
       if (hitNfo.id.x == material_ID5){
              mat = RockPattern(hitNfo.position);
;
       }
       if (hitNfo.id.x == material_ID6){
              mat.reflectionCoefficient = 0.11;
              mat.albedo = vec3(0.3978099,0.6603774,0.2896938);;
              mat.transparency =0;
              mat.smoothness = 0.17;
              mat.reflectindx = 0.05;
       }

            return mat;
        }

vec3 GetSkyGradient(vec3 rayDirection)
{
	vec3 lightDir = GetDirectionLight().direction;
	vec3 color = GetDirectionLight().color;
	lightDir = normalize(lightDir);
	float directLight = max(dot(rayDirection, lightDir), 0.0);
	vec3 backdrop = min(max(pow(directLight, 41.0) * vec3(1.8, 1.1, .9) * 0.6, 0.01), 1.);
	backdrop += min(max(pow(directLight, 2.5) * vec3(0.8, 0.9, 1.0) * 1.6, 0.01), 1.);
	backdrop += (min(max(pow(directLight, 12.0) * 1.6, 0.01), 1.)*1.1);
	backdrop *= normalize(color);
	if (_iMouse.x > 1.0) {
		backdrop = skyboxSample().rgb;
	}
	return max(backdrop, vec3(0.8, 0.9, 1.0)*0.125);
}

void ApplyAtmosphere(inout vec3 col, const in  Trace ray, const in  ContactInfo hitInfo)
{

	float fogAmount = exp(hitInfo.distanc * -fogDensity * 0.1 /* * (1.0 + wasInWater) */);
	vec3 fog = GetSkyGradient(ray.direction);

	DirectionLight directionalLight = GetDirectionLight();
	float dirDot = clamp(dot(-directionalLight.direction, ray.direction), 0.0, 1.0);
	fog += directionalLight.color * pow(dirDot, 10.0);

	col = mix(fog, col, fogAmount);

	PointLight pointLight = GetPointLight();

	vec3 vToLight = pointLight.position - ray.origin;
	float pointDot = dot(vToLight, ray.direction);
	pointDot = clamp(pointDot, 0.0, hitInfo.distanc);

	vec3 closestPoint = ray.origin + ray.direction * pointDot;
	float fDist = length(closestPoint - pointLight.position);
	col += pointLight.color * 0.01 / (fDist * fDist);
}


vec3 GetNormal(in vec3 position, in float transparencyPointer)
{
	float delta = 0.025;
	vec3 offset[4];

	offset[0] = vec3(delta, -delta, -delta);
	offset[1] = vec3(-delta, -delta, delta);
	offset[2] = vec3(-delta, delta, -delta);
	offset[3] = vec3(delta, delta, delta);

	float f1 = GetDistanceScene(position + offset[0], transparencyPointer).x;
	float f2 = GetDistanceScene(position + offset[1], transparencyPointer).x;
	float f3 = GetDistanceScene(position + offset[2], transparencyPointer).x;
	float f4 = GetDistanceScene(position + offset[3], transparencyPointer).x;
	vec3 normal = normalize(offset[0] * f1 + offset[1] * f2 + offset[2] * f3 + offset[3] * f4);
	return normal;
}
#ifdef DEBUG_STEPS
float focus;
#endif
void RayMarch(in Trace ray, out ContactInfo result, int maxIter, float transparencyPointer)
{
	ContactInfo originalResult = result;
	result.distanc = ray.startdistanc;
	result.id.x = 0.0;
	for (int i = 0;i <= maxIter;i++)
	{
		result.position = ray.origin + ray.direction * result.distanc;
		vec4 sceneDistance = GetDistanceScene(result.position, transparencyPointer);
		/*
		if (inWater == 0. && (i < 1) && (sceneDistance.y == material_ID2) && (sceneDistance.x < 0.001)) {
			inWater = 1.;
			wasInWater = inWater;
		}
		else {*/

			float cocs = max(result.distanc - _Distance,0.0) * _LensCoeff * 0.1;
			cocs = min(cocs, _MaxCoC * 0.1);



			result.id = sceneDistance.yzw;
			result.distanc = result.distanc + sceneDistance.x * _Step * (1.0 + min(_StepIncreaseByDistance, _StepIncreaseMax) * result.distanc);
		//}
		
		if (sceneDistance.x < max(cocs, _MarchMinimum * 0.1) || result.distanc > _FarPlane) {
			sceneDistance = GetDistanceScene(result.position, transparencyPointer);
#ifdef DEBUG_STEPS
			focus = cocs;
#endif
			break;
		}

	}
	if (result.distanc >= _FarPlane)
	{
		result.distanc = _FarPlane;
		result.position = ray.origin + ray.direction * result.distanc;
		result.id.x = 0.0;
	}
}

void insideMarch(in Trace ray, out ContactInfo result, int maxIter, float transparencyPointer)
{
	result.distanc = ray.startdistanc;
	result.id.x = 0.0;
	for (int i = 0;i <= maxIter / 3;i++)
	{
		result.position = ray.origin + ray.direction * result.distanc;
		vec4 sceneDistance = GetDistanceScene(result.position, transparencyPointer);
		result.id = sceneDistance.yzw;

		result.distanc = result.distanc + sceneDistance.x;
		if (sceneDistance.y != material_ID2)
			return;
	}


	if (result.distanc >= ray.length)
	{
		result.distanc = 1000.0;
		result.position = ray.origin + ray.direction * result.distanc;
		result.id.x = 0.0;
	}
}

float traceToLight(vec3 rayPosition, vec3 normalTrace, vec3 lightDir, float rayLightDistance) {

	vec3 ro = rayPosition;
	vec3 rd = lightDir;
	float t = 0.1;
	float k = rayLightDistance;
	float res = 1.0;
	for (int i = 0; i < 20; i++)
	{
		float h = GetDistanceScene(ro + rd * t, transparencyInformation).x;
		h = max(h, 0.0);
		res = min(res, k*h / t);
		t += clamp(h, 0.001, 0.9);
	}
	return clamp(res, 0.1, 9.0);
}
float GetShadow(in vec3 position, in vec3 normal, in vec3 lightDirection, in float lightDistance)
{
	return traceToLight(position, normal, lightDirection, lightDistance);
}

float GetAmbientOcclusion(in ContactInfo intersection, in Surface surface)
{
	vec3 position = intersection.position;
	vec3 normal = surface.normal;

	float AO = 1.0;

	float sdfDistance = 0.0;
	for (int i = 0; i <= 4; i++)
	{
		sdfDistance += 0.1;
		vec4 sceneDistance = GetDistanceScene(position + normal * sdfDistance, transparencyInformation);
		AO *= 1.0 - max(0.0, (sdfDistance - sceneDistance.x) * 0.4 / sdfDistance);
	}

	return AO;
}

void AddAtmosphere(inout vec3 col, in Trace ray, in ContactInfo hitNfo)
{
	float fFogAmount = exp(hitNfo.distanc * -fogDensity);
	vec3 fogColor = GetSkyGradient(ray.direction);

	DirectionLight directionalLight = GetDirectionLight();
	float fDirDot = clamp(dot(directionalLight.direction, ray.direction), 0.0, 1.0);
	fogColor += directionalLight.color * pow(fDirDot, 2.0);

	PointLight pointLight = GetPointLight();

	vec3 lightPointer = pointLight.position - ray.origin;
	float PNT_dot = dot(lightPointer, ray.direction);
	PNT_dot = clamp(PNT_dot, 0.0, hitNfo.distanc);

	vec3 closestPoint = ray.origin + ray.direction * PNT_dot;
	float sdfDistance = length(closestPoint - pointLight.position);
	col += pointLight.color * 0.01 / (sdfDistance * sdfDistance);
}

vec3 AddFresnel(in vec3 diffuse, in vec3 specular, in vec3 normal, in vec3 viewDirection, in Material material)
{
	vec3 reflection = reflect(viewDirection, normal);
	vec3 reflectionToView = normalize(reflection + -viewDirection);
	float reflectionCoefficient = material.reflectionCoefficient;
	float smoothFactor = material.smoothness * 0.9 + 0.1;
	// 
	float r1 = dot(reflectionToView, -viewDirection);
	r1 = clamp((1.0 - r1), 0.0, 1.0);
	float r1Pow = pow(r1, 5.0);
	float fresnelApprox = reflectionCoefficient + (1.0 - reflectionCoefficient) * r1Pow * smoothFactor;

	return mix(diffuse, specular, fresnelApprox);
}

float BlinnPhong(in vec3 collisionDirection, in vec3 lightDirection, in vec3 normal, in float smoothness)
{
	vec3 reflectionToView = normalize(lightDirection - collisionDirection);
	float n_dot_h = max(0.0, dot(reflectionToView, normal));

	float specularPower = exp2(4.0 + 6.0 * smoothness);
	float specularIntensity = (specularPower + 2.0) * 0.125;

	return pow(n_dot_h, specularPower) * specularIntensity;
}

Shading AddPointLight(in PointLight light, in vec3 surfacePosition, in vec3 collisionDirection, in vec3 normal, in Material material)
{
	Shading shading;

	vec3 lightPointer = light.position - surfacePosition;
	vec3 lightDirection = normalize(lightPointer);
	float shadowLength = length(lightPointer);

	float attenuation = 1.0 / (shadowLength * shadowLength);

	float shadowFactor = GetShadow(surfacePosition, normal, lightDirection, shadowLength);
	vec3 diffuse = light.color * max(0.0, shadowFactor * attenuation * dot(lightDirection, normal) / (1.0 + material.transparency));
	shading.diffuse = diffuse;
	shading.specular = BlinnPhong(collisionDirection, lightDirection, normal, material.smoothness) * diffuse;
	shading.diffuse = clamp(shading.diffuse, 0.0, 1.0);
	shading.specular = clamp(shading.specular, 0.0, 1.0);

	return shading;
}

Shading AddDirectionLight(in DirectionLight light, in vec3 surfacePosition, in vec3 collisionDirection, in vec3 normal, in Material material)
{
	Shading shading;

	float shadowLength = 10.0;
	vec3 lightDirection = -light.direction;
	float shadowFactor = GetShadow(surfacePosition, normal, lightDirection, shadowLength);
	vec3 diffuse = light.color * shadowFactor * max(0.0, dot(lightDirection, normal) / (1.0 + material.transparency));
	shading.diffuse = diffuse;
	shading.specular = BlinnPhong(collisionDirection, lightDirection, normal, material.smoothness) * diffuse;
	shading.diffuse = clamp(shading.diffuse, 0.0, 1.0);
	shading.specular = clamp(shading.specular, 0.0, 1.0);


	return shading;
}


vec3 ShadeSurface(in Trace ray, in ContactInfo hitNfo, in Surface surface, in Material material)
{
	Shading shading;

	float AO = GetAmbientOcclusion(hitNfo, surface);
	vec3 ambientLight = GetSkyGradient(surface.normal) * AO;

	shading.diffuse = ambientLight;
	shading.specular = surface.reflection;

	PointLight pointLight = GetPointLight();
	Shading pointLighting = AddPointLight(pointLight, hitNfo.position, ray.direction, surface.normal, material);
	shading.diffuse += pointLighting.diffuse;
	shading.specular += pointLighting.specular;

	DirectionLight directionalLight = GetDirectionLight();
	Shading directionLighting = AddDirectionLight(directionalLight, hitNfo.position, ray.direction, surface.normal, material);
	shading.diffuse += directionLighting.diffuse;
	shading.specular += directionLighting.specular;

	vec3 diffuseReflection = mix(shading.diffuse * material.albedo * (1.0 - material.transparency), surface.subsurface, material.transparency);

	return AddFresnel(diffuseReflection, shading.specular, surface.normal, ray.direction, material);
}
vec3 GetSceneColourSecondary(in Trace ray);

vec3 GetReflection(in Trace ray, in ContactInfo hitNfo, in Surface surface)
{
		const float lightOffSurface = 0.1;

		Trace reflectTrace;
		reflectTrace.direction = reflect(ray.direction, surface.normal);
		reflectTrace.origin = hitNfo.position;
		reflectTrace.length = 16.0;
		reflectTrace.startdistanc = lightOffSurface / abs(dot(reflectTrace.direction, surface.normal));

		return GetSceneColourSecondary(reflectTrace);
}

vec3 GetSubSurface(in Trace ray, in ContactInfo hitNfo, in Surface surface, in Material material)
{

	float lightOffSurface = 0.05;
	Trace refractTrace;
	refractTrace.direction = refract(ray.direction, surface.normal, material.reflectindx);
	refractTrace.origin = hitNfo.position;
	refractTrace.length = 16.0;
	refractTrace.startdistanc = lightOffSurface / abs(dot(refractTrace.direction, surface.normal));

	ContactInfo hitNfo2;
	insideMarch(refractTrace, hitNfo2, 32, emptyInformation);
	vec3 normal = GetNormal(hitNfo2.position, emptyInformation);

	Trace refractTrace2;
	refractTrace2.direction = refract(refractTrace.direction, normal, 1.0 / material.reflectindx);
	refractTrace2.origin = hitNfo2.position;
	refractTrace2.length = 16.0;
	refractTrace2.startdistanc = 0.0;

	float extinctionDistance = hitNfo2.distanc;
	float transparencity = material.transparency / (1.0 + distance(hitNfo2.position, hitNfo.position)*4.);
	float nonTransparency = 1.0 - transparencity;

	vec3 sceneColor = material.albedo * GetSceneColourSecondary(refractTrace2) * nonTransparency;
	vec3 materialExtinction = material.albedo;

	vec3 extinction = (1.7 / (1.0 + (materialExtinction * extinctionDistance)));

	return sceneColor * extinction;
}

vec3 GetSceneColourSecondary(in Trace ray)
{
	ContactInfo hitNfo;
	RayMarch(ray, hitNfo, 22, noTransparency);

	vec3 sceneColor;

	if (hitNfo.id.x < 0.5)
	{
		sceneColor = GetSkyGradient(ray.direction);
	}
	else
	{
		Surface surface;
		surface.normal = GetNormal(hitNfo.position, noTransparency);

		Material material = GetObjectMaterial(hitNfo);

		surface.reflection = GetSkyGradient(reflect(ray.direction, surface.normal));

		material.transparency = 0.0;

		sceneColor = ShadeSurface(ray, hitNfo, surface, material);
	}

	AddAtmosphere(sceneColor, ray, hitNfo);

	return sceneColor;
}
vec3 Reinhard(in vec3 color)
{
	return vec3(1.0, 1.0, 1.0) - exp2(-color);
}

vec4 mainImage()
{
	vec2 fragCoord = gl_FragCoord.xy;
	vec4 fragColor;
	Trace ray;

	vec2 uv = fragCoord.xy / _iResolution.xy;

	if (_TextId == 1.0) {
		vec3 text = texture(_TextTex, uv).rgb;
		return vec4(text,0.5);
	}

	vec3 lookAt = _CameraLookAt.xyz;
	vec3 position = _CameraPosition.xyz;

	float fov = _FOV;
	vec3 forwardDirection = normalize(lookAt - position);
	vec3 worldUpDirection = _CameraUp.xyz;


	vec2 viewDirectionCoord = uv * 2.0 - 1.0;

	float aspectRatio = _iResolution.x / _iResolution.y;
	viewDirectionCoord.y /= aspectRatio;

	ray.origin = position;

	vec3 rightDirection = normalize(cross(forwardDirection, worldUpDirection));
	vec3 upDirection = cross(rightDirection, forwardDirection);

	forwardDirection *= tan((90.0 - fov * 0.5) * DEG_TO_RAD);
	ray.direction = normalize(-rightDirection * viewDirectionCoord.x + upDirection * viewDirectionCoord.y + forwardDirection);
	ray.startdistanc = 0.0;
	ray.length = farClip;


	ContactInfo intersection;
	RayMarch(ray, intersection, 80, transparencyInformation);
	vec3 sceneColor;
	float d = intersection.distanc;
	if (intersection.id.x < 0.5) {
		sceneColor = GetSkyGradient(ray.direction);
	}
	else {
		Surface surface;

		surface.normal = GetNormal(intersection.position, transparencyInformation);

		Material material = GetObjectMaterial(intersection);

		//surface.reflection = GetReflection(ray, intersection, surface);
		surface.reflection = GetSkyGradient(ray.direction);

		float distanctrans = intersection.distanc;
		/*if (material.transparency > 0.0) {
			surface.subsurface = GetSubSurface(ray, intersection, surface, material);
		}*/

		sceneColor = ShadeSurface(ray, intersection, surface, material);
	}

	ApplyAtmosphere(sceneColor, ray, intersection);

	float exposure = 1.5;
	vec2 coord = (uv - 0.5) * (_iResolution.x / _iResolution.y) * 2.0;
	float falloff = 0.2;
	float rf = sqrt(dot(coord, coord)) * falloff;
	float rf2_1 = rf * rf + 1.0;
	float e = 1.2 / (rf2_1 * rf2_1);
	vec3 noise = (rand(uv + _iTime) - .5) * vec3(1.0, 1.0, 1.0) * 0.01;
	fragColor = min(max(vec4(e*Reinhard(sceneColor * exposure) + noise, 1.0), vec4(0.0, 0.0, 0.0, 1.0)), vec4(1.0, 1.0, 1.0, 1.0));
	fragColor.rgb = fragColor.rgb + filmgrain(fragColor.rgb) * 0.2;

	float cocs = (d - _Distance) * _LensCoeff / d;
	cocs = clamp(cocs, -_MaxCoC, _MaxCoC);

	fragColor.a = saturate(abs(cocs) * _RcpMaxCoC);

#ifdef DEBUG_STEPS
	fragColor.r = focus;
#endif

	if (_TextId == 3.0) {
		vec3 text = texture(_TextTex, uv).rgb;
		for (float i = 1.0; i < 4.0; i+=1.) {
			text+=texture(_TextTex, uv+i*0.0003).rgb;
		}
		text/=4.0;
		fragColor.rgb+=clamp(text,0.0,1.0);
	}

	if (_TextId == 4.0) {
		vec3 text = texture(_TextTex, uv*4.).rgb;
		for (float i = 1.0; i < 4.0; i+=1.) {
			text+=texture(_TextTex, uv*4.+i*0.0003).rrr;
		}
		text/=4.0;
		fragColor.rgb+=clamp(text,0.0,1.0);
	}

	return  fragColor;
}

void main()
{
	gl_FragColor = mainImage();
}
