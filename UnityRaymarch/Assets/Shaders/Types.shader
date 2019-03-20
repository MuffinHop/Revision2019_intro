﻿
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