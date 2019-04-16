
	struct ColorGradingPreset {
	  vec3 gain;
	  vec3 gamma;
	  vec3 lift;
	  vec3 presaturation;
	  vec3 colorTemperatureStrength;
	  float colorTemperature; // in K
	  float colorTemperatureBrightnessNormalization;
	};

	vec3 colorTemperatureToRGB(const in float temperature) {
	  mat3 m = (temperature <= 6500.0) ? mat3(vec3(0.0, -2902.1955373783176, -8257.7997278925690),
											  vec3(0.0, 1669.5803561666639, 2575.2827530017594),
											  vec3(1.0, 1.3302673723350029, 1.8993753891711275)) :
										 mat3(vec3(1745.0425298314172, 1216.6168361476490, -8257.7997278925690),
											  vec3(-2666.3474220535695, -2173.1012343082230, 2575.2827530017594),
											  vec3(0.55995389139931482, 0.70381203140554553, 1.8993753891711275));
	  return clamp(vec3(m[0] / (vec3(clamp(temperature, 1000.0, 40000.0)) + m[1]) + m[2]), vec3(0.0), vec3(1.0));
	}

	vec3 colorGradingProcess(const in ColorGradingPreset p, in vec3 c) {
	  float originalBrightness = dot(c, vec3(0.2126, 0.7152, 0.0722));
	  c = mix(c, c * colorTemperatureToRGB(p.colorTemperature), p.colorTemperatureStrength);
	  float newBrightness = dot(c, vec3(0.2126, 0.7152, 0.0722));
	  c *= mix(1.0, (newBrightness > 1e-6) ? (originalBrightness / newBrightness) : 1.0, p.colorTemperatureBrightnessNormalization);
	  c = mix(vec3(dot(c, vec3(0.2126, 0.7152, 0.0722))), c, p.presaturation);
	  return pow((p.gain * 2.0) * (c + (((p.lift * 2.0) - vec3(1.0)) * (vec3(1.0) - c))), vec3(0.5) / p.gamma);
	}


	

	uniform sampler2D _MainTex;
	uniform vec4 iResolution;

	uniform vec4 _Gain;
	uniform vec4 _Gamma;
	uniform vec4 _Lift;
	uniform vec4 _Presaturation;
	uniform vec4 _ColorTemperatureStrength;
	uniform float _ColorTemprature;
	uniform float _TempratureNormalization;
	float hash(float c) { return fract(sin(dot(c, 12.9898)) * 43758.5453); }
	vec4 mainImage(vec2 uv) {
		vec4 i = vec4(0.);
		float s1;
		float blr = max(texture(_MainTex, uv).w,0.011);
		for (int t = 0; t < 11; t++) {
			float s2 = s1;
			s1 = hash(float(1 - t) + dot(uv, uv));
			vec2 f = 0.01*(-1.0 + 2.0*vec2(s1, s2));
			i += textureLod(_MainTex, uv + max(blr / 2. - .004, 0.)*vec2(s1 - .5, s2 - .5), blr * 2.5);
		}
		i /= vec4(11.);
		return i ;
	}
	void main() {
		ColorGradingPreset ColorGradingPreset1 = ColorGradingPreset(
			_Gain.rgb,							// Gain
			_Gamma.rgb,							// Gamma
			_Lift.rgb,							// Lift
			_Presaturation.rgb,					// Presaturation
			_ColorTemperatureStrength.rgb,		// Color temperature strength
			_ColorTemprature,					// Color temperature (in K)
			_TempratureNormalization			// Color temperature brightness normalization factor 
		);

		vec2 uv = gl_FragCoord.xy / iResolution.xy;
		if (abs(uv.y - 0.5) > 0.46) {
			gl_FragColor = vec4(0.0);
		}
		else {
			vec3 c = mainImage(uv).xyz;
			c = colorGradingProcess(ColorGradingPreset1, c);
			gl_FragColor = vec4(c, 1.0);
		}
	}