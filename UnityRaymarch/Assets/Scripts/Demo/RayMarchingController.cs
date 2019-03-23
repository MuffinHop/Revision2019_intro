using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEditor;
using UnityEngine;

public class RayMarchingController : MonoBehaviour
{
    [SerializeField]
    private List<GameObject> gameObjects;
    private List<RM_Material> materials;
    string shaderBeginning = @"Shader ""Standard Trace Marching""
{
	Properties{
		_MainTex(""Texture"", 2D) = ""white"" {}
		_iChannel0(""_iChannel0"", 2D) = ""white"" {}
		_iChannel1(""_iChannel1"", 2D) = ""white"" {}
		_iChannel2(""_iChannel2"", 2D) = ""white"" {}
		_iChannel3(""_iChannel3"", 2D) = ""white"" {}
		_iTime(""_iTime"", Float) = 0
		_iFrame(""_iFrame"", Float) = 0
		_Value(""Value"", Float) = 0
		_iMouse(""_iMouse"",Vector) = (0.0,0.0,0.0,0.0)
		_iResolution(""_iResolution"",Vector) = (0.0,0.0,0.0,0.0)
		_Light(""_Light"",Vector) = (0.0,0.0,0.0,0.0)
	}
		SubShader{
		Pass{
		GLSLPROGRAM

#pragma multi_compile NVIDIA


";
    string[] begin = {
    "General/Vertex.shader",
    "StartFragment.shader",
    "General/Uniforms.shader",
    "General/Defines.shader",
    "General/Globals.shader",
    "Types.glslinc",
    "BasicFunctions.glslinc",
    "General/Lighting.shader",
    "Modifiers/Mod1.shader",
    "Modifiers/R45.shader",
    "SDFs/Primitive/sdBox.glslinc",
    "General/TextureDisplacement.shader"
};
    string[] end = {
    "General/Sky.shader",
    "General/RayMarcher.shader",
    "General/Shading.shader",
    "General/Scene.shader",
    "General/Init.shader"
};

    string shaderEnding = @"#endif

		ENDGLSL
		}
	}
	FallBack ""Diffuse""
}";

    string GetShaderPart(string file)
    {
        string path = "Assets/Shaders/" + file;
        StreamReader reader = new StreamReader(path);
        string code = reader.ReadToEnd();
        reader.Close();
        return code;
    }

    int getMaterialRegister(RM_Material material)
    {
        if (materials == null)
        {
            materials = new List<RM_Material>();
        }
        if (materials.Contains(material))
        {
            return materials.IndexOf(material);
        }
        materials.Add(material);
        return materials.Count - 1;
    }
    public float Radians(float angle)
    {
        return (Mathf.PI / 180.0f) * angle;
    }
    string GetDistanceFields()
    {
        string sdf;
        string sdf0 = @"vec4 GetDistanceScene(vec3 position, in float transparencyPointer)
        {
            vec4 result = vec4(10000.0, -1.0, 0.0, 0.0);
        ";
        string sdf1 = "\n";
        var culture = System.Globalization.CultureInfo.InvariantCulture;
        for (int i = 0; i < gameObjects.Count; i++)
        {
            var position = gameObjects[i].transform.position;
            var scale = gameObjects[i].transform.localScale;
            var rotation = gameObjects[i].transform.rotation;
            sdf1 += "               vec3 posID" + i + " = position + vec3(" + position.x.ToString(culture) + ", " + position.y.ToString(culture) + "," + position.z.ToString(culture) + ");\n";
            sdf1 += "               posID" + i + "= posID" + i + "*rotationMatrix(vec3(" + rotation.x.ToString(culture) + "," + rotation.y.ToString(culture) + "," + rotation.z.ToString(culture) + "), "+ rotation.w.ToString(culture) + ");\n";
            sdf1 += "               float id" + i + "_distance = sdBox(posID" + i + ", vec3(" + scale.x.ToString(culture) + ", " + scale.y.ToString(culture) + "," + scale.z.ToString(culture) + "));\n";
            sdf1 += "               vec4 distID" + i + " = vec4(id" + i + "_distance, material_ID" + getMaterialRegister(gameObjects[i].GetComponent<RM_Material>()) + ", position.xz + vec2(position.y, 0.0));\n";
            sdf1 += "               result = DistUnionCombine(result, distID" + i + ");\n\n";
        }
        /*
            vec3 p1 = position + vec3(_Objects[1], _Objects[2], _Objects[3]);
            float material_ID0_distance = sdBox(p1, vec3(_Objects[7], _Objects[8], _Objects[9]));
            vec4 distFloor = vec4(material_ID0_distance, material_ID0, position.xz + vec2(position.y, 0.0));
            result = DistUnionCombine(result, distFloor);
        */

        string sdf2 = @"
            return result;
        }";
        sdf = sdf0 + sdf1 + sdf2;
        return sdf;
    }
    string GetMaterials()
    {
        string mat;
        string mat0 = @" Material GetObjectMaterial(in ContactInfo hitNfo)
        {
            Material mat;
            ";

        string mat1 = "\n";
        var culture = System.Globalization.CultureInfo.InvariantCulture;
        for (int i = 0; i < materials.Count; i++)
        {
            mat1 += "       if (hitNfo.id.x == material_ID" + i + "){\n";
            mat1 += "              mat.reflectionCoefficient = " + materials[i].reflectionCoefficient.ToString(culture);
            mat1 += ";\n              mat.albedo = vec3(" + materials[i].albedo.r.ToString(culture) + "," + materials[i].albedo.g.ToString(culture) + "," + materials[i].albedo.b.ToString(culture) + ");";
            mat1 += ";\n              mat.transparency =" + materials[i].albedo.a.ToString(culture);
            mat1 += ";\n              mat.reflectivity = " + materials[i].reflectivity.ToString(culture);
            mat1 += ";\n              mat.reflectindx = " + materials[i].reflectindx.ToString(culture);
            mat1 += ";\n       }\n";

        }
        /*
            if (hitNfo.id.x == material_ID0)
            {
                mat.reflectionCoefficient = 0.05;
                mat.albedo = vec3(1.0, 0.4, 0.3) * (0.9 + 0.1 * texture(_iChannel1, hitNfo.position.xy).rgb);
                mat.reflectivity = 0.4 - texture(_iChannel1, hitNfo.position.xy).r * 0.8;
                mat.transparency = 0.021;
                mat.reflectindx = 0.6 / 1.3330;
            }*/

        string mat2 = @"
            return mat;
        }";
        mat = mat0 + mat1 + mat2;
        return mat;
    }

    static void WriteString(string filename, string code)
    {
        string path = "Assets/Shaders/" + filename + ".shader";

        StreamWriter writer = new StreamWriter(path, false);
        writer.WriteLine(code);
        writer.Close();

        AssetDatabase.ImportAsset(path);
    }
    void Start()
    {
        var GOs = FindObjectsOfType<RM_Object>();
        gameObjects = new List<GameObject>();
        foreach (var GO in GOs)
        {
            gameObjects.Add(GO.gameObject);
        }
        string fullcode = shaderBeginning;
        foreach (string part in begin)
        {
            fullcode += "\n" + GetShaderPart(part);
        }
        fullcode += "\n" + GetDistanceFields();
        fullcode += "\n" + GetMaterials();
        foreach (string part in end)
        {
            fullcode += "\n" + GetShaderPart(part);
        }
        fullcode += "\n" + shaderEnding;
        Debug.Log(fullcode);
        WriteString("Megashader", fullcode);
    }

    // Update is called once per frame
    void Update()
    {

    }
}
