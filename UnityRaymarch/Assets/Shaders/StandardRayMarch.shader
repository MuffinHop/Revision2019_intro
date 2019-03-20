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


#define fogDensity 0.01


	uniform vec4 _DirectionalLight;
	uniform vec4 _DirectionalLightColor;
	uniform vec4 _PointLightPosition;
	uniform vec4 _PointLightColor;
	uniform vec4 _CameraPosition;
	uniform vec4 _CameraLookAt;
	uniform vec4 _CameraUp;
	uniform float _FOV;
	uniform float _Objects[10];

	out vec4 positionition_in_object_coordinates;

#ifdef VERTEX     

	void main()
	{
		positionition_in_object_coordinates = gl_Vertex;
		gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	}

#endif

#ifdef FRAGMENT
#define kRayMarchMaxIter 123


#define ENABLE_FOG
#define ENABLE_REFLECTIONS
#define ENABLE_SHADOWS
#define ENABLE_TRANSPARENCY
#define ENABLE_AO
#define DOUBLE_SIDED_TRANSPARENCY



	float inWater;
	float wasInWater;

	const float PI = 3.14159265359;
	const float DEG_TO_RAD = PI / 180.0;
	const float TWOPI = PI * 2.0;
	const float TAU = PI * 2.0;
	const float PHI = (sqrt(5)*0.5 + 0.5);
	const float farClip = 32.0;
#define saturate(x) clamp(x, 0, 1)

	const float material_ID0 = 1.0;
	const float material_ID1 = 2.0;
	const float material_ID2 = 3.0;

	const float noTransparency = -1.0;
	const float transparencyInformation = 1.0;
	const float emptyInformation = 0.0;

#include "Types.glslinc"
#include "BasicFunctions.glslinc"
#include "Modifiers/Mod1.glslinc"
#include "Modifiers/R45.glslinc"
#include "SDFs/Primitive/sdBox.glslinc"

	vec4 DistUnionCombine(in vec4 v1, in vec4 v2)
	{
		return mix(v1, v2, step(v2.x, v1.x));
	}
	vec4 DistUnionCombineTransparent(in vec4 v1, in vec4 v2, in float transparencyPointer)
	{
		vec4 vScaled = vec4(v2.x * (transparencyPointer * 2.0 - 1.0), v2.yzw);
		return mix(v1, vScaled, step(vScaled.x, v1.x) * step(0.0, transparencyPointer));
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
	vec4 GetDistanceScene(vec3 position, in float transparencyPointer)
	{
		vec4 result = vec4(10000.0, -1.0, 0.0, 0.0);
		vec3 p1 = position + vec3(_Objects[1], _Objects[2], _Objects[3]);
		float material_ID0_distance = sdBox(p1, vec3(_Objects[7], _Objects[8], _Objects[9]));
		vec4 vDistFloor = vec4(material_ID0_distance, material_ID0, position.xz + vec2(position.y, 0.0));

		result = DistUnionCombine(result, vDistFloor);
		return result;
	}
	Material GetObjectMaterial(in ContactInfo hitNfo) {
		Material mat;
		if (hitNfo.id.x == material_ID0) {
			mat.reflectionCoefficient = 0.05;
			mat.albedo = vec3(1.0, 0.4, 0.3) * (0.9 + 0.1*texture(_iChannel1, hitNfo.position.xy).rgb);
			mat.reflectivity = 0.4 - texture(_iChannel1, hitNfo.position.xy).r*0.8;
			mat.transparency = 0.021;
			mat.reflectindx = 0.6 / 1.3330;
		}
		return mat;
	}

	PointLight GetPointLight()
	{
		PointLight result;
		result.position = _PointLightPosition.xyz;
		result.color = _PointLightColor.xyz;

#if defined(NVIDIA)
		result.position = -result.position;
#endif
		return result;
	}

	DirectionLight GetDirectionLight()
	{
		DirectionLight result;
		result.direction = _DirectionalLight.xyz;
		result.color = _DirectionalLightColor.xyz;
#if defined(NVIDIA)
		result.direction = -result.direction;
		result.color *= 0.123;
#endif
		return result;
	}
	vec3 GetSkyGradient(vec3 rayDirection)
	{
		vec3 lightDir = GetDirectionLight().direction;
		lightDir = normalize(lightDir);
		float directLight = max(dot(rayDirection, lightDir),0.0);
		vec3 backdrop = min(max(pow(directLight, 41.0) * vec3(1.8, 1.1, .9) * 0.6, 0.01), 1.);
		backdrop += min(max(pow(directLight,2.5) * vec3(0.8, 0.9, 1.0) * 1.6, 0.01), 1.);
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

	float SchlicksApproximation(in vec3 reflectionToView, in vec3 viewDirection, in float reflectionCoefficient, in float smoothFactor)
	{
		float r1 = dot(reflectionToView, -viewDirection);
		r1 = clamp((1.0 - r1), 0.0, 1.0);
		float r1Pow = pow(r1, 5.0);
		return reflectionCoefficient + (1.0 - reflectionCoefficient) * r1Pow * smoothFactor;
	}

	vec3 AddFresnel(in vec3 diffuse, in vec3 specular, in vec3 normal, in vec3 viewDirection, in Material material)
	{
		vec3 reflection = reflect(viewDirection, normal);
		vec3 reflectionToView = normalize(reflection + -viewDirection);
		float fresnelApprox = SchlicksApproximation(reflectionToView, viewDirection, material.reflectionCoefficient, material.reflectivity * 0.9 + 0.1);
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
		vec3 forwardDirection = -normalize(lookAt - position);
		vec3 worldUpDirection = -_CameraUp.xyz;


		vec2 viewDirectionCoord = uv * 2.0 - 1.0;

		float aspectRatio = _iResolution.x / _iResolution.y;
		viewDirectionCoord.y /= aspectRatio;

		ray.origin = position;

		vec3 rightDirection = normalize(cross(forwardDirection, worldUpDirection));
		vec3 upDirection = cross(rightDirection, forwardDirection);

		forwardDirection *= tan((90.0 - fov * 0.5) * DEG_TO_RAD);
		ray.direction = normalize(rightDirection * viewDirectionCoord.x + upDirection * viewDirectionCoord.y + forwardDirection);
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
		vec3 noise = (rand(uv + _iTime)-.5) * vec3(1.0, 1.0, 1.0) * 0.01;
		fragColor = min(max(vec4(e*Reinhard(sceneColor * exposure) + noise, 1.0), vec4(0.0,0.0,0.0,1.0)), vec4(1.0,1.0,1.0,1.0));
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
