
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
#define kFogDensity 0.01

void ApplyAtmosphere(inout vec3 col, const in  Trace ray, const in  ContactInfo hitInfo)
{

	float fogAmount = exp(hitInfo.distanc * -fogDensity /* * (1.0 + wasInWater) */);
	vec3 fog = GetSkyGradient(ray.direction);

	DirectionLight directionalLight = GetDirectionLight();
	float dirDot = clamp(dot(-directionalLight.direction, ray.direction), 0.0, 1.0);
	fog += directionalLight.color * pow(dirDot, 20.0);

	col = mix(fog, col, fogAmount);

	PointLight pointLight = GetPointLight();

	vec3 vToLight = pointLight.position - ray.origin;
	float pointDot = dot(vToLight, ray.direction);
	pointDot = clamp(pointDot, 0.0, hitInfo.distanc);

	vec3 closestPoint = ray.origin + ray.direction * pointDot;
	float fDist = length(closestPoint - pointLight.position);
	col += pointLight.color * 0.01 / (fDist * fDist);
}

