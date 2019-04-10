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
