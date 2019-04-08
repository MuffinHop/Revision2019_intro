vec3 GetNormal(in vec3 position, in float transparencyPointer)
{
	float delta = 0.025;
	vec3 offset[4];

	offset[0] = vec3(delta, -delta, -delta);
	offset[1] = vec3(-delta, -delta, delta);
	offset[2] = vec3(-delta, delta, -delta);
	offset[3] = vec3(delta, delta, delta);

	deform = 1;
	float f1 = GetDistanceScene(position + offset[0], transparencyPointer).x;
	float f2 = GetDistanceScene(position + offset[1], transparencyPointer).x;
	float f3 = GetDistanceScene(position + offset[2], transparencyPointer).x;
	float f4 = GetDistanceScene(position + offset[3], transparencyPointer).x;
	vec3 normal = normalize(offset[0] * f1 + offset[1] * f2 + offset[2] * f3 + offset[3] * f4);
	deform = 0;
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
	deform = 0;
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

			float cocs = max(result.distanc - _Distance,0.0) * _LensCoeff / result.distanc;
			cocs = min(cocs, _MaxCoC);


			result.id = sceneDistance.yzw;
#ifdef DEBUG_STEPS
			result.distanc = result.distanc + sceneDistance.x;
#else 
			result.distanc = result.distanc + sceneDistance.x * max(cocs, _MarchMinimum);
#endif
		//}
		
		if (sceneDistance.x < 0.001 + float(maxIter)*0.00001) {
			sceneDistance = GetDistanceScene(result.position, transparencyPointer);
#ifdef DEBUG_STEPS
			focus = cocs;
			result.distanc = result.distanc + sceneDistance.x;
#else 
			result.distanc = result.distanc + sceneDistance.x * 100.0;
#endif
			break;
		}


			if (result.distanc > _FarPlane) {
				result.distanc = 1000.0;
				result.position = ray.origin + ray.direction * result.distanc;
				result.id.x = 0.0;
				break;
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
	deform = 0;
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
	for (int i = 0; i < 24; i++)
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
}

float GetAmbientOcclusion(in ContactInfo intersection, in Surface surface)
{
	vec3 position = intersection.position;
	vec3 normal = surface.normal;

	float AO = 1.0;

	float sdfDistance = 0.0;
	deform = 1;
	for (int i = 0; i <= 5; i++)
	{
		sdfDistance += 0.1;
		vec4 sceneDistance = GetDistanceScene(position + normal * sdfDistance, transparencyInformation);
		AO *= 1.0 - max(0.0, (sdfDistance - sceneDistance.x) * 0.4 / sdfDistance);
	}
	deform = 0;

	return AO;
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