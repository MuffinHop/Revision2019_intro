Shader "Standard Marching"
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

#include "General/Vertex.shader"

#ifdef FRAGMENT
#include "General/Uniforms.shader"
#include "General/Defines.shader"
#include "General/Globals.shader"
#include "Types.glslinc"
#include "BasicFunctions.glslinc"
#include "General/Lighting.shader"
#include "Modifiers/Mod1.shader"
#include "Modifiers/R45.shader"
#include "SDFs/Primitive/sdBox.glslinc"
#include "General/TextureDisplacement.shader"

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

#include "General/Sky.shader"
#include "General/RayMarcher.shader"
#include "General/Shading.shader"
#include "General/Scene.shader"
#include "General/Init.shader"


#endif

		ENDGLSL
		}
	}
	FallBack "Diffuse"
}
