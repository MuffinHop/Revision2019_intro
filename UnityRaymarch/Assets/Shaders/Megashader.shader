Shader "Standard Trace Marching"
{
	Properties{
		_MainTex("Texture", 2D) = "white" {}
		_iChannel0("_iChannel0", 2D) = "white" {}
		_iChannel1("_iChannel1", 2D) = "white" {}
		_iChannel2("_iChannel2", 2D) = "white" {}
		_iChannel3("_iChannel3", 2D) = "white" {}
		_iTime("_iTime", Float) = 0
		_iFrame("_iFrame", Float) = 0
		_Value("Value", Float) = 0
		_iMouse("_iMouse",Vector) = (0.0,0.0,0.0,0.0)
		_iResolution("_iResolution",Vector) = (0.0,0.0,0.0,0.0)
		_Light("_Light",Vector) = (0.0,0.0,0.0,0.0)
	}
		SubShader{
		Pass{
		GLSLPROGRAM

#pragma multi_compile NVIDIA



#ifdef VERTEX     
out vec4 positionition_in_object_coordinates;
void main()
{
	positionition_in_object_coordinates = gl_Vertex;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
#endif
#ifdef FRAGMENT
uniform sampler2D _MainTex;
uniform sampler2D _iChannel0;
uniform sampler2D _iChannel1;
uniform sampler2D _iChannel2;
uniform sampler2D _iChannel3;
uniform float _iTime;
uniform float _Value;
uniform float _iFrame;
uniform vec4 _iMouse;
uniform vec4 _iResolution;
uniform vec4 _Light;
uniform vec4 _DirectionalLight;
uniform vec4 _DirectionalLightColor;
uniform vec4 _PointLightPosition;
uniform vec4 _PointLightColor;
uniform vec4 _CameraPosition;
uniform vec4 _CameraLookAt;
uniform vec4 _CameraUp;
uniform float _FOV;

#define kRayMarchMaxIter 123
#define fogDensity 0.01
#define ENABLE_FOG
#define ENABLE_REFLECTIONS
#define ENABLE_SHADOWS
#define ENABLE_TRANSPARENCY
#define ENABLE_AO
#define DOUBLE_SIDED_TRANSPARENCY
#define saturate(x) clamp(x, 0, 1)

float inWater;
float wasInWater;

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
	float reflectivity;
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

#if defined(NVIDIA)
	result.position = -result.position;
#endif
	return result;
}

DirectionLight GetDirectionLight() {
	DirectionLight result;
	result.direction = _DirectionalLight.xyz;
	result.color = _DirectionalLightColor.xyz;
#if defined(NVIDIA)
	result.direction = -result.direction;
	result.color *= 0.123;
#endif
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

const float material_ID0 = 0;

const float material_ID1 = 1;

const float material_ID2 = 2;

const float material_ID3 = 3;

const float material_ID4 = 4;

const float material_ID5 = 5;

const float material_ID6 = 6;

const float material_ID7 = 7;

const float material_ID8 = 8;

const float material_ID9 = 9;

const float material_ID10 = 10;

const float material_ID11 = 11;

const float material_ID12 = 12;

const float material_ID13 = 13;

const float material_ID14 = 14;

const float material_ID15 = 15;

const float material_ID16 = 16;

const float material_ID17 = 17;

 uniform float _Objects[180];
float fSphere(vec3 p, float r) {
	return length(p) - r;
}
// Box: correct distance to corners
float fBox(vec3 p, vec3 b) {
	vec3 d = abs(p) - b;
	return length(max(d, vec3(0))) + vmax(min(d, vec3(0)));
}
vec4 GetDistanceScene(vec3 position, in float transparencyPointer)
        {
            vec4 result = vec4(10000.0, -1.0, 0.0, 0.0);
        
               vec3 posID0 = position - vec3(_Objects[0], _Objects[1], _Objects[2]);
               posID0= posID0*rotationMatrix(vec3(_Objects[6], _Objects[7], _Objects[8]),  _Objects[9]);
               float id0_distance = fSphere(posID0,_Objects[3]);
               vec4 distID0 = vec4(id0_distance, material_ID0, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombineTransparent(result, distID0, transparencyPointer);

               vec3 posID1 = position - vec3(_Objects[10], _Objects[11], _Objects[12]);
               posID1= posID1*rotationMatrix(vec3(_Objects[16], _Objects[17], _Objects[18]),  _Objects[19]);
               float id1_distance = fSphere(posID1,_Objects[13]);
               vec4 distID1 = vec4(id1_distance, material_ID1, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombine(result, distID1);

               vec3 posID2 = position - vec3(_Objects[20], _Objects[21], _Objects[22]);
               posID2= posID2*rotationMatrix(vec3(_Objects[26], _Objects[27], _Objects[28]),  _Objects[29]);
               float id2_distance = fSphere(posID2,_Objects[23]);
               vec4 distID2 = vec4(id2_distance, material_ID2, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombine(result, distID2);

               vec3 posID3 = position - vec3(_Objects[30], _Objects[31], _Objects[32]);
               posID3= posID3*rotationMatrix(vec3(_Objects[36], _Objects[37], _Objects[38]),  _Objects[39]);
               float id3_distance = fSphere(posID3,_Objects[33]);
               vec4 distID3 = vec4(id3_distance, material_ID3, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombine(result, distID3);

               vec3 posID4 = position - vec3(_Objects[40], _Objects[41], _Objects[42]);
               posID4= posID4*rotationMatrix(vec3(_Objects[46], _Objects[47], _Objects[48]),  _Objects[49]);
               float id4_distance = fSphere(posID4,_Objects[43]);
               vec4 distID4 = vec4(id4_distance, material_ID4, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombine(result, distID4);

               vec3 posID5 = position - vec3(_Objects[50], _Objects[51], _Objects[52]);
               posID5= posID5*rotationMatrix(vec3(_Objects[56], _Objects[57], _Objects[58]),  _Objects[59]);
               float id5_distance = fSphere(posID5,_Objects[53]);
               vec4 distID5 = vec4(id5_distance, material_ID5, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombineTransparent(result, distID5, transparencyPointer);

               vec3 posID6 = position - vec3(_Objects[60], _Objects[61], _Objects[62]);
               posID6= posID6*rotationMatrix(vec3(_Objects[66], _Objects[67], _Objects[68]),  _Objects[69]);
               float id6_distance = fSphere(posID6,_Objects[63]);
               vec4 distID6 = vec4(id6_distance, material_ID6, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombineTransparent(result, distID6, transparencyPointer);

               vec3 posID7 = position - vec3(_Objects[70], _Objects[71], _Objects[72]);
               posID7= posID7*rotationMatrix(vec3(_Objects[76], _Objects[77], _Objects[78]),  _Objects[79]);
               float id7_distance = fSphere(posID7,_Objects[73]);
               vec4 distID7 = vec4(id7_distance, material_ID7, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombineTransparent(result, distID7, transparencyPointer);

               vec3 posID8 = position - vec3(_Objects[80], _Objects[81], _Objects[82]);
               posID8= posID8*rotationMatrix(vec3(_Objects[86], _Objects[87], _Objects[88]),  _Objects[89]);
               float id8_distance = fSphere(posID8,_Objects[83]);
               vec4 distID8 = vec4(id8_distance, material_ID8, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombineTransparent(result, distID8, transparencyPointer);

               vec3 posID9 = position - vec3(_Objects[90], _Objects[91], _Objects[92]);
               posID9= posID9*rotationMatrix(vec3(_Objects[96], _Objects[97], _Objects[98]),  _Objects[99]);
               float id9_distance = fSphere(posID9,_Objects[93]);
               vec4 distID9 = vec4(id9_distance, material_ID9, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombine(result, distID9);

               vec3 posID10 = position - vec3(_Objects[100], _Objects[101], _Objects[102]);
               posID10= posID10*rotationMatrix(vec3(_Objects[106], _Objects[107], _Objects[108]),  _Objects[109]);
               float id10_distance = fSphere(posID10,_Objects[103]);
               vec4 distID10 = vec4(id10_distance, material_ID10, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombine(result, distID10);

               vec3 posID11 = position - vec3(_Objects[110], _Objects[111], _Objects[112]);
               posID11= posID11*rotationMatrix(vec3(_Objects[116], _Objects[117], _Objects[118]),  _Objects[119]);
               float id11_distance = fSphere(posID11,_Objects[113]);
               vec4 distID11 = vec4(id11_distance, material_ID11, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombine(result, distID11);

               vec3 posID12 = position - vec3(_Objects[120], _Objects[121], _Objects[122]);
               posID12= posID12*rotationMatrix(vec3(_Objects[126], _Objects[127], _Objects[128]),  _Objects[129]);
               float id12_distance = fSphere(posID12,_Objects[123]);
               vec4 distID12 = vec4(id12_distance, material_ID12, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombine(result, distID12);

               vec3 posID13 = position - vec3(_Objects[130], _Objects[131], _Objects[132]);
               posID13= posID13*rotationMatrix(vec3(_Objects[136], _Objects[137], _Objects[138]),  _Objects[139]);
               float id13_distance = fSphere(posID13,_Objects[133]);
               vec4 distID13 = vec4(id13_distance, material_ID13, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombineTransparent(result, distID13, transparencyPointer);

               vec3 posID14 = position - vec3(_Objects[140], _Objects[141], _Objects[142]);
               posID14= posID14*rotationMatrix(vec3(_Objects[146], _Objects[147], _Objects[148]),  _Objects[149]);
               float id14_distance = fBox(posID14, vec3(_Objects[143], _Objects[144],_Objects[145]));
               vec4 distID14 = vec4(id14_distance, material_ID14, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombine(result, distID14);

               vec3 posID15 = position - vec3(_Objects[150], _Objects[151], _Objects[152]);
               posID15= posID15*rotationMatrix(vec3(_Objects[156], _Objects[157], _Objects[158]),  _Objects[159]);
               float id15_distance = fBox(posID15, vec3(_Objects[153], _Objects[154],_Objects[155]));
               vec4 distID15 = vec4(id15_distance, material_ID15, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombine(result, distID15);

               vec3 posID16 = position - vec3(_Objects[160], _Objects[161], _Objects[162]);
               posID16= posID16*rotationMatrix(vec3(_Objects[166], _Objects[167], _Objects[168]),  _Objects[169]);
               float id16_distance = fSphere(posID16,_Objects[163]);
               vec4 distID16 = vec4(id16_distance, material_ID16, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombine(result, distID16);

               vec3 posID17 = position - vec3(_Objects[170], _Objects[171], _Objects[172]);
               posID17= posID17*rotationMatrix(vec3(_Objects[176], _Objects[177], _Objects[178]),  _Objects[179]);
               float id17_distance = fBox(posID17, vec3(_Objects[173], _Objects[174],_Objects[175]));
               vec4 distID17 = vec4(id17_distance, material_ID17, position.xz + vec2(position.y, 0.0));
               result = DistUnionCombine(result, distID17);


            return result;
        }
 Material GetObjectMaterial(in ContactInfo hitNfo)
        {
            Material mat;
            
       if (hitNfo.id.x == material_ID0){
              mat.reflectionCoefficient = 0.05;
              mat.albedo = vec3(0.8867924,0.7999618,0.7236561);;
              mat.transparency =0.1686274;
              mat.reflectivity = 0.2;
              mat.reflectindx = 0.45;
       }
       if (hitNfo.id.x == material_ID1){
              mat.reflectionCoefficient = 0.05;
              mat.albedo = vec3(0.7830189,0.7830189,0.7830189);;
              mat.transparency =0;
              mat.reflectivity = 0.4;
              mat.reflectindx = 0.45;
       }
       if (hitNfo.id.x == material_ID2){
              mat.reflectionCoefficient = 0.05;
              mat.albedo = vec3(0.7830189,0.7830189,0.7830189);;
              mat.transparency =0;
              mat.reflectivity = 0.4;
              mat.reflectindx = 0.45;
       }
       if (hitNfo.id.x == material_ID3){
              mat.reflectionCoefficient = 0.05;
              mat.albedo = vec3(0.7830189,0.7830189,0.7830189);;
              mat.transparency =0;
              mat.reflectivity = 0.4;
              mat.reflectindx = 0.45;
       }
       if (hitNfo.id.x == material_ID4){
              mat.reflectionCoefficient = 0.05;
              mat.albedo = vec3(0.7830189,0.7830189,0.7830189);;
              mat.transparency =0;
              mat.reflectivity = 0.4;
              mat.reflectindx = 0.45;
       }
       if (hitNfo.id.x == material_ID5){
              mat.reflectionCoefficient = 0.05;
              mat.albedo = vec3(0.509434,0.509434,0.509434);;
              mat.transparency =0.2509804;
              mat.reflectivity = 0.4;
              mat.reflectindx = 0.45;
       }
       if (hitNfo.id.x == material_ID6){
              mat.reflectionCoefficient = 0.05;
              mat.albedo = vec3(0.509434,0.509434,0.509434);;
              mat.transparency =0.2509804;
              mat.reflectivity = 0.4;
              mat.reflectindx = 0.45;
       }
       if (hitNfo.id.x == material_ID7){
              mat.reflectionCoefficient = 0.05;
              mat.albedo = vec3(0.509434,0.509434,0.509434);;
              mat.transparency =0.2509804;
              mat.reflectivity = 0.4;
              mat.reflectindx = 0.45;
       }
       if (hitNfo.id.x == material_ID8){
              mat.reflectionCoefficient = 0.1;
              mat.albedo = vec3(0.6603774,0.2772339,0.2772339);;
              mat.transparency =0.3294117;
              mat.reflectivity = 0;
              mat.reflectindx = 0.2;
       }
       if (hitNfo.id.x == material_ID9){
              mat.reflectionCoefficient = 0.05;
              mat.albedo = vec3(0.7830189,0.7830189,0.7830189);;
              mat.transparency =0;
              mat.reflectivity = 0.4;
              mat.reflectindx = 0.45;
       }
       if (hitNfo.id.x == material_ID10){
              mat.reflectionCoefficient = 0.05;
              mat.albedo = vec3(0.7830189,0.7830189,0.7830189);;
              mat.transparency =0;
              mat.reflectivity = 0.4;
              mat.reflectindx = 0.45;
       }
       if (hitNfo.id.x == material_ID11){
              mat.reflectionCoefficient = 0.05;
              mat.albedo = vec3(0.7830189,0.7830189,0.7830189);;
              mat.transparency =0;
              mat.reflectivity = 0.4;
              mat.reflectindx = 0.45;
       }
       if (hitNfo.id.x == material_ID12){
              mat.reflectionCoefficient = 0.05;
              mat.albedo = vec3(0.7830189,0.7830189,0.7830189);;
              mat.transparency =0;
              mat.reflectivity = 0.4;
              mat.reflectindx = 0.45;
       }
       if (hitNfo.id.x == material_ID13){
              mat.reflectionCoefficient = 0.05;
              mat.albedo = vec3(0.8867924,0.7999618,0.7236561);;
              mat.transparency =0.1686274;
              mat.reflectivity = 0.2;
              mat.reflectindx = 0.45;
       }
       if (hitNfo.id.x == material_ID14){
              mat.reflectionCoefficient = 0.05;
              mat.albedo = vec3(0.7830189,0.7830189,0.7830189);;
              mat.transparency =0;
              mat.reflectivity = 0.4;
              mat.reflectindx = 0.45;
       }
       if (hitNfo.id.x == material_ID15){
              mat.reflectionCoefficient = 0.05;
              mat.albedo = vec3(0.7830189,0.7830189,0.7830189);;
              mat.transparency =0;
              mat.reflectivity = 0.4;
              mat.reflectindx = 0.45;
       }
       if (hitNfo.id.x == material_ID16){
              mat.reflectionCoefficient = 0.05;
              mat.albedo = vec3(0.7830189,0.7830189,0.7830189);;
              mat.transparency =0;
              mat.reflectivity = 0.4;
              mat.reflectindx = 0.45;
       }
       if (hitNfo.id.x == material_ID17){
              mat.reflectionCoefficient = 0.05;
              mat.albedo = vec3(0.7830189,0.7830189,0.7830189);;
              mat.transparency =0;
              mat.reflectivity = 0.4;
              mat.reflectindx = 0.45;
       }

            return mat;
        }

vec3 GetSkyGradient(vec3 rayDirection)
{
	vec3 lightDir = GetDirectionLight().direction;
	lightDir = normalize(lightDir);
	float directLight = max(dot(rayDirection, lightDir), 0.0);
	vec3 backdrop = min(max(pow(directLight, 41.0) * vec3(1.8, 1.1, .9) * 0.6, 0.01), 1.);
	backdrop += min(max(pow(directLight, 2.5) * vec3(0.8, 0.9, 1.0) * 1.6, 0.01), 1.);
	backdrop += (min(max(pow(directLight, 81.0) * 1.6, 0.01), 1.)*1.1);
	return max(backdrop, vec3(0.8, 0.9, 1.0)*0.125);
}

vec3 GetNormal(in vec3 position, in float transparencyPointer)
{
	float delta = 0.025;
	vec3 offset[] = {
		vec3(delta, -delta, -delta),
		vec3(-delta, -delta, delta),
		vec3(-delta, delta, -delta),
		vec3(delta, delta, delta)
	};
	float f1 = GetDistanceScene(position + offset[0], transparencyPointer).x;
	float f2 = GetDistanceScene(position + offset[1], transparencyPointer).x;
	float f3 = GetDistanceScene(position + offset[2], transparencyPointer).x;
	float f4 = GetDistanceScene(position + offset[3], transparencyPointer).x;
	vec3 normal = normalize(offset[0] * f1 + offset[1] * f2 + offset[2] * f3 + offset[3] * f4);
	return normal;
}

void RayMarch(in Trace ray, out ContactInfo result, int maxIter, float transparencyPointer)
{
	ContactInfo originalResult = result;
	result.distanc = ray.startdistanc;
	result.id.x = 0.0;
	for (int i = 0;i <= kRayMarchMaxIter;i++)
	{
		result.position = ray.origin + ray.direction * result.distanc;
		vec4 sceneDistance = GetDistanceScene(result.position, transparencyPointer);

		if (inWater == 0. && (i < 1) && (sceneDistance.y == material_ID2) && (sceneDistance.x < 0.001)) {
			inWater = 1.;
			wasInWater = inWater;
		}
		else {
			result.id = sceneDistance.yzw;
			result.distanc = result.distanc + sceneDistance.x / 2.0;
		}
	}
	if (result.distanc >= ray.length)
	{
		result.distanc = 1000.0;
		result.position = ray.origin + ray.direction * result.distanc;
		result.id.x = 0.0;
	}
}

void insideMarch(in Trace ray, out ContactInfo result, int maxIter, float transparencyPointer)
{
	result.distanc = ray.startdistanc;
	result.id.x = 0.0;

	for (int i = 0;i <= kRayMarchMaxIter / 3;i++)
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
	for (int i = 0; i < 32; i++)
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
#ifdef ENABLE_SHADOWS
	Trace shadowTrace;
	shadowTrace.direction = lightDirection;
	shadowTrace.origin = position;
	float shadowBias = 0.05;
	shadowTrace.startdistanc = shadowBias / abs(dot(lightDirection, normal));
	shadowTrace.length = lightDistance - shadowTrace.startdistanc;

	ContactInfo shadowIntersect;
	RayMarch(shadowTrace, shadowIntersect, 32, transparencyInformation);

	float shadow = step(0.0, shadowIntersect.distanc) * step(lightDistance, shadowIntersect.distanc);

	return shadow * traceToLight(position, normal, lightDirection, lightDistance);
#else
	return 1.0;
#endif
}

float GetAmbientOcclusion(in ContactInfo intersection, in Surface surface)
{
#ifdef ENABLE_AO  
	vec3 position = intersection.position;
	vec3 normal = surface.normal;

	float AO = 1.0;

	float sdfDistance = 0.0;
	for (int i = 0; i <= 5; i++)
	{
		sdfDistance += 0.1;

		vec4 sceneDistance = GetDistanceScene(position + normal * sdfDistance, transparencyInformation);

		AO *= 1.0 - max(0.0, (sdfDistance - sceneDistance.x) * 0.4 / sdfDistance);
	}

	return AO;
#else
	return 1.0;
#endif  
}

void AddAtmosphere(inout vec3 col, in Trace ray, in ContactInfo hitNfo)
{
	float fFogAmount = exp(hitNfo.distanc * -fogDensity * (1.0 + wasInWater));
	vec3 fogColor = GetSkyGradient(ray.direction);

	DirectionLight directionalLight = GetDirectionLight();
	float fDirDot = clamp(dot(-directionalLight.direction, ray.direction), 0.0, 1.0);
	fogColor += directionalLight.color * pow(fDirDot, 20.0);

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
	float smoothFactor = material.reflectivity * 0.9 + 0.1;
	// 
	float r1 = dot(reflectionToView, -viewDirection);
	r1 = clamp((1.0 - r1), 0.0, 1.0);
	float r1Pow = pow(r1, 5.0);
	float fresnelApprox = reflectionCoefficient + (1.0 - reflectionCoefficient) * r1Pow * smoothFactor;

	return mix(diffuse, specular, fresnelApprox);
}

float BlinnPhong(in vec3 collisionDirection, in vec3 lightDirection, in vec3 normal, in float reflection)
{
	vec3 reflectionToView = normalize(lightDirection - collisionDirection);
	float n_dot_h = max(0.0, dot(reflectionToView, normal));

	float specularPower = exp2(4.0 + 6.0 * reflection);
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
	shading.specular = BlinnPhong(collisionDirection, lightDirection, normal, material.reflectivity) * diffuse;

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
	shading.specular = BlinnPhong(collisionDirection, lightDirection, normal, material.reflectivity) * diffuse;

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
#ifdef ENABLE_REFLECTIONS  
	{
		const float lightOffSurface = 0.1;

		Trace reflectTrace;
		reflectTrace.direction = reflect(ray.direction, surface.normal);
		reflectTrace.origin = hitNfo.position;
		reflectTrace.length = 16.0;
		reflectTrace.startdistanc = lightOffSurface / abs(dot(reflectTrace.direction, surface.normal));

		return GetSceneColourSecondary(reflectTrace);
	}
#else
	return GetSkyGradient(reflect(ray.direction, surface.normal));
#endif
}

vec3 GetSubSurface(in Trace ray, in ContactInfo hitNfo, in Surface surface, in Material material)
{
	inWater = 0.;

#ifdef ENABLE_TRANSPARENCY 
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
#else
	return GetSkyGradient(reflect(ray.direction, surface.normal));
#endif
}

vec3 GetSceneColourSecondary(in Trace ray)
{
	ContactInfo hitNfo;
	inWater = 0.;
	RayMarch(ray, hitNfo, 64, noTransparency);

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

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
	Trace ray;
	inWater = 0.;

	vec2 uv = fragCoord.xy / _iResolution.xy;

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

	ray.direction.y *= 0.85;


	ContactInfo intersection;
	RayMarch(ray, intersection, 12, transparencyInformation);
	vec3 sceneColor;

	if (intersection.id.x < 0.5) {
		sceneColor = GetSkyGradient(ray.direction);
	}
	else {
		Surface surface;

		surface.normal = GetNormal(intersection.position, transparencyInformation);

		Material material = GetObjectMaterial(intersection);

		surface.reflection = GetReflection(ray, intersection, surface);

		float distanctrans = intersection.distanc;
		if (material.transparency > 0.0) {
			surface.subsurface = GetSubSurface(ray, intersection, surface, material);
		}

		sceneColor = ShadeSurface(ray, intersection, surface, material);
	}

	AddAtmosphere(sceneColor, ray, intersection);

	float exposure = 1.5;
	vec2 coord = (uv - 0.5) * (_iResolution.x / _iResolution.y) * 2.0;
	float falloff = 0.2;
	float rf = sqrt(dot(coord, coord)) * falloff;
	float rf2_1 = rf * rf + 1.0;
	float e = 1.2 / (rf2_1 * rf2_1);
	vec3 noise = (rand(uv + _iTime) - .5) * vec3(1.0, 1.0, 1.0) * 0.01;
	fragColor = min(max(vec4(e*Reinhard(sceneColor * exposure) + noise, 1.0), vec4(0.0, 0.0, 0.0, 1.0)), vec4(1.0, 1.0, 1.0, 1.0));
}

void main()
{
	vec4 fragColor;
	vec2 fragCoord;
	fragCoord = gl_FragCoord.xy;
	mainImage(fragColor, fragCoord);
	gl_FragColor = fragColor;
}
#endif

		ENDGLSL
		}
	}
	FallBack "Diffuse"
}
