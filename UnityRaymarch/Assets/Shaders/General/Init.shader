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
	RayMarch(ray, intersection, 96, transparencyInformation);
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