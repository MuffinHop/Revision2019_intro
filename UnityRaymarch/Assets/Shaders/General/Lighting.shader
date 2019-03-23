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
