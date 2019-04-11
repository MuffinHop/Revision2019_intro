/* File generated with Shader Minifier 1.1.6
 * http://www.ctrl-alt-test.fr
 */
#ifndef POST_INL_
# define POST_INL_

const char *post_frag =
 "#version 130\nstruct ColorGradingPreset{vec3 gain;vec3 gamma;vec3 lift;vec3 presaturation;vec3 colorTemperatureStrength;float colorTemperature;float colorTemperatureBrightnessNormalization;};"
 "vec3 colorTemperatureToRGB(const in float temperature)"
 "{"
   "mat3 m=temperature<=6500.?mat3(vec3(0.,-2902.2,-8257.8),vec3(0.,1669.58,2575.28),vec3(1.,1.33027,1.89938)):mat3(vec3(1745.04,1216.62,-8257.8),vec3(-2666.35,-2173.1,2575.28),vec3(.559954,.703812,1.89938));"
   "return clamp(vec3(m[0]/(vec3(clamp(temperature,1000.,40000.))+m[1])+m[2]),vec3(0.),vec3(1.));"
 "}"
 "vec3 colorGradingProcess(const in ColorGradingPreset p,in vec3 c)"
 "{"
   "float originalBrightness=dot(c,vec3(.2126,.7152,.0722));"
   "c=mix(c,c*colorTemperatureToRGB(p.colorTemperature),p.colorTemperatureStrength);"
   "float newBrightness=dot(c,vec3(.2126,.7152,.0722));"
   "c*=mix(1.,newBrightness>1e-06?originalBrightness/newBrightness:1.,p.colorTemperatureBrightnessNormalization);"
   "c=mix(vec3(dot(c,vec3(.2126,.7152,.0722))),c,p.presaturation);"
   "return pow(p.gain*2.*(c+(p.lift*2.-vec3(1.))*(vec3(1.)-c)),vec3(.5)/p.gamma);"
 "}"
 "uniform sampler2D _MainTex;"
 "uniform vec4 iResolution,_Gain,_Gamma,_Lift,_Presaturation,_ColorTemperatureStrength;"
 "uniform float _ColorTemprature,_TempratureNormalization;"
 "float hash(float c)"
 "{"
   "return fract(sin(dot(c,12.9898))*43758.5);"
 "}"
 "vec4 mainImage(vec2 uv)"
 "{"
   "vec4 i=vec4(0.);"
   "float s1,blr=texture(_MainTex,uv).w;"
   "for(int t=0;t<11;t++)"
     "{"
       "float s2=s1;"
       "s1=hash(float(1-t)+dot(uv,uv));"
       "vec2 f=.01*(-1.+2.*vec2(s1,s2));"
       "i+=textureLod(_MainTex,uv+max(blr/2.-.004,0.)*vec2(s1-.5,s2-.5),blr);"
     "}"
   "i/=vec4(11.);"
   "return i-blr*2.;"
 "}"
 "void main()"
 "{"
   "ColorGradingPreset ColorGradingPreset1=ColorGradingPreset(_Gain.xyz,_Gamma.xyz,_Lift.xyz,_Presaturation.xyz,_ColorTemperatureStrength.xyz,_ColorTemprature,_TempratureNormalization);"
   "vec2 uv=gl_FragCoord.xy/iResolution.xy;"
   "vec3 c=mainImage(uv).xyz;"
   "c=colorGradingProcess(ColorGradingPreset1,c);"
   "gl_FragColor=vec4(c,1.);"
 "}";

#endif // POST_INL_
