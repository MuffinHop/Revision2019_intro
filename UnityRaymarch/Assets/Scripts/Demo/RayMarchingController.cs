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
    private List<RMMaterial> materials;
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
    string materialIDStr = "";
    int getMaterialRegister(RMMaterial material)
    {
        if (materials == null)
        {
            materials = new List<RMMaterial>();
        }
        if (materials.Contains(material))
        {
            return materials.IndexOf(material);
        }
        materials.Add(material);
        int id = materials.Count - 1;
        materialIDStr += "\nconst float material_ID" + id + " = " + id + ";\n";
        return id;
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
            var scale = gameObjects[i].transform.localScale / 2.0f;
            var rotation = gameObjects[i].transform.rotation;
            var rmObject = gameObjects[i].GetComponent<RM_Object>();
            if (rmObject.ShaderComponent == null)
            {
                Debug.LogWarning(rmObject.name + "missing shadercomponent");
                continue;
            }
            var material = gameObjects[i].GetComponent<RM_Object>().Material;
            if (material == null)
            {
                Debug.LogWarning("Material Error", gameObjects[i]);
                continue;
            }
            var functionName = rmObject.ShaderComponent.FunctionName;
            var scaleFormat = rmObject.ShaderComponent.Scale;
            var mixerFormat = rmObject.ShaderComponent.Mixer;
            int index = i * 10;
            sdf1 += "               vec3 posID" + i + " = position - vec3(_Objects[" + (index + 0) + "], _Objects[" + (index + 1) + "], _Objects[" + (index + 2) + "]);\n";
            sdf1 += "               posID" + i + "= posID" + i + "*rotationMatrix(vec3(_Objects[" + (index + 6) + "], _Objects[" + (index + 7) + "], _Objects[" + (index + 8) + "]),  _Objects[" + (index + 9) + "]);\n";
            switch (scaleFormat)
            {
                case ShaderComponent.ScaleInfo.OneDimension:
                    sdf1 += "               float id" + i + "_distance = " + functionName.ToString() + "(posID" + i + ",_Objects[" + (index + 3) + "]);\n";
                    break;
                case ShaderComponent.ScaleInfo.TwoDimension:
                    sdf1 += "               float id" + i + "_distance = " + functionName.ToString() + "(posID" + i + ", vec2(_Objects[" + (index + 3) + "], _Objects[" + (index + 4) + "]));\n";
                    break;
                case ShaderComponent.ScaleInfo.ThreeDimension:
                    sdf1 += "               float id" + i + "_distance = " + functionName.ToString() + "(posID" + i + ", vec3(_Objects[" + (index + 3) + "], _Objects[" + (index + 4) + "],_Objects[" + (index + 5) + "]));\n";
                    break;
            }
            sdf1 += "               vec4 distID" + i + " = vec4(id" + i + "_distance, material_ID" + getMaterialRegister(material) + ", position.xz + vec2(position.y, 0.0));\n";
            //if (material.albedo.a < 1.0f) {
                sdf1 += "               result = DistUnionCombineTransparent(result, distID" + i + ", transparencyPointer);\n\n";
           // } else {
            //    sdf1 += "               result = DistUnionCombine(result, distID" + i + ");\n\n";
            //}
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
            if (i != 0)
            {
                mat1 += "\n       if (hitNfo.id.x == material_ID" + i + "){\n";
            }
            mat1 += "              mat.reflectionCoefficient = " + materials[i].reflectionCoefficient.ToString(culture);
            mat1 += ";\n              mat.albedo = vec3(" + materials[i].albedo.r.ToString(culture) + "," + materials[i].albedo.g.ToString(culture) + "," + materials[i].albedo.b.ToString(culture) + ");";
            mat1 += ";\n              mat.transparency =" + (1.0f-materials[i].albedo.a).ToString(culture);
            mat1 += ";\n              mat.smoothness = " + materials[i].smoothness.ToString(culture);
            mat1 += ";\n              mat.reflectindx = " + materials[i].reflectindx.ToString(culture);
            if (i != 0)
            {
                mat1 += ";\n       }\n";
            } else
            {

                mat1 += ";\n";
            }

        }
        /*
            if (hitNfo.id.x == material_ID0)
            {
                mat.reflectionCoefficient = 0.05;
                mat.albedo = vec3(1.0, 0.4, 0.3) * (0.9 + 0.1 * texture(_iChannel1, hitNfo.position.xy).rgb);
                mat.smoothness = 0.4 - texture(_iChannel1, hitNfo.position.xy).r * 0.8;
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
            var material = GO.GetComponent<RM_Object>().Material;
            if (material != null)
            {
                getMaterialRegister(material);
            }
        }
        string fullcode = shaderBeginning;
        foreach (string part in begin)
        {
            fullcode += "\n" + GetShaderPart(part);
        }
        fullcode += "\n" + materialIDStr;
        fullcode += "\n uniform float _Objects[" + (gameObjects.Count * 10) + "];";
        var sdfTextArray = new List<TextAsset>();
        for (int i = 0; i < gameObjects.Count; i++)
        {
            var rmObject = gameObjects[i].GetComponent<RM_Object>();
            if (rmObject.ShaderComponent == null)
            {
                continue;
            }
            else if (!sdfTextArray.Contains(rmObject.ShaderComponent.TextFile))
            {
                sdfTextArray.Add(rmObject.ShaderComponent.TextFile);
            }
        }
        for (int i = 0; i < sdfTextArray.Count; i++)
        {
            var shaderCode = GetShaderPart("SDFs/Primitive/" + sdfTextArray[i].name + ".glslinc");
            fullcode += "\n" + shaderCode;
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
    void LateUpdate()
    {
        if (RM_Camera.RM_Objects == null)
        {
            RM_Camera.RM_Objects = new List<float>();
        }
        RM_Camera.RM_Objects.Clear();
        foreach (var GO in gameObjects)
        {
            RM_Camera.RM_Objects.Add(GO.transform.position.x);
            RM_Camera.RM_Objects.Add(GO.transform.position.y);
            RM_Camera.RM_Objects.Add(GO.transform.position.z);
            RM_Camera.RM_Objects.Add(GO.transform.localScale.x);
            RM_Camera.RM_Objects.Add(GO.transform.localScale.y);
            RM_Camera.RM_Objects.Add(GO.transform.localScale.z);
            RM_Camera.RM_Objects.Add(GO.transform.rotation.x);
            RM_Camera.RM_Objects.Add(GO.transform.rotation.y);
            RM_Camera.RM_Objects.Add(GO.transform.rotation.z);
            RM_Camera.RM_Objects.Add(GO.transform.rotation.w);
        }
    }

}