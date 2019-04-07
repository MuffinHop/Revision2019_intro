﻿using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEditor;
using UnityEngine;

public class RayMarchingController : MonoBehaviour
{
    [SerializeField]
    private List<GameObject> gameObjects;
    private List<RM_Surface> surfaces;
    private Dictionary<RM_Surface, List<RM_Object>> gomat;
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
    int getMaterialRegister(RM_Surface material)
    {
        if (surfaces == null)
        {
            surfaces = new List<RM_Surface>();
        }
        if (surfaces.Contains(material))
        {
            return surfaces.IndexOf(material);
        }
        surfaces.Add(material);
        int id = surfaces.Count - 1;
        materialIDStr += "\nconst float material_ID" + id + " = " + (id + 1) + ";\n";
        return id;
    }
    public float Radians(float angle)
    {
        return (Mathf.PI / 180.0f) * angle;
    }
    string GetDistanceFields()
    {
        List<string> includeCode = new List<string>();
        string sdf;
        string sdf0 = @"vec4 GetDistanceScene(vec3 position, in float transparencyPointer)
        {
            vec4 result = vec4(10000.0, -1.0, 0.0, 0.0);
        ";
        string sdf1 = "\n";
        var culture = System.Globalization.CultureInfo.InvariantCulture;
        int materialID = 0;
        foreach(var surface in surfaces)
        {
            sdf1 += "         float id" + materialID + "_distance = 1e9;\n";
            for (int i = 0; i < gomat[surface].Count; i++)
            {

                var rmObject = gomat[surface][i].GetComponent<RM_Object>();
                if (rmObject.ShaderComponent == null)
                {
                    Debug.LogWarning(rmObject.name + "missing shadercomponent");
                    continue;
                }
                var functionName = rmObject.ShaderComponent.FunctionName;
                var scaleFormat = rmObject.ShaderComponent.Scale;
                var mixerFormat = rmObject.Mixer;
                string mixStr = "";
                string specialFuncs = "";
                string strR = rmObject.R.ToString(culture);
                string strN = rmObject.N.ToString(culture);
                switch (mixerFormat)
                {
                    case RM_Object.Mix.Min:
                        mixStr += "min";
                        break;
                    case RM_Object.Mix.Max:
                        mixStr += "max";
                        break;
                    case RM_Object.Mix.MaxMinus:
                        mixStr += "maxMinus";
                        break;
                    case RM_Object.Mix.OpUnionRound:           // box, sphere, r
                        mixStr += "fOpUnionRound";
                        specialFuncs = "," + strR;
                        includeCode.Add("Operators/OpUnionRound.shader");
                        break;
                    case RM_Object.Mix.OpIntersectionRound:    // box, sphere, r
                        mixStr += "fOpIntersectionRound";
                        specialFuncs = "," + strR;
                        includeCode.Add("Operators/OpIntersectionRound.shader");
                        break;
                    case RM_Object.Mix.OpDifferenceRound:      // box, sphere, r
                        mixStr += "fOpDifferenceRound";
                        specialFuncs = "," + strR;
                        includeCode.Add("Operators/OpDifferenceRound.shader");
                        break;
                    case RM_Object.Mix.OpUnionChamfer:         // box, sphere, r
                        mixStr += "fOpUnionChamfer";
                        specialFuncs = "," + strR;
                        includeCode.Add("Operators/OpUnionChamfer.shader");
                        break;
                    case RM_Object.Mix.OpIntersectionChamfer:  // box, sphere, r
                        mixStr += "fOpIntersectionChamfer";
                        specialFuncs = "," + strR;
                        includeCode.Add("Operators/OpIntersectionChamfer.shader");
                        break;
                    case RM_Object.Mix.OpDifferenceChamfer:    // box, sphere, r
                        mixStr += "fOpDifferenceChamfer";
                        specialFuncs = "," + strR;
                        includeCode.Add("Operators/OpDifferenceChamfer.shader");
                        break;
                    case RM_Object.Mix.OpUnionColumns:         // box, sphere, r, n
                        mixStr += "fOpUnionColumns";
                        specialFuncs = "," + strR + "," + strN;
                        includeCode.Add("Operators/OpUnionColumns.shader");
                        break;
                    case RM_Object.Mix.OpIntersectionColumns:  // box, sphere, r, n
                        mixStr += "fOpIntersectionColumns";
                        specialFuncs = "," + strR + "," + strN;
                        includeCode.Add("Operators/OpIntersectionColumns.shader");
                        break;
                    case RM_Object.Mix.OpDifferenceColumns:    // box, sphere, r, n
                        mixStr += "fOpDifferenceColumns";
                        specialFuncs = "," + strR + "," + strN;
                        includeCode.Add("Operators/OpDifferenceColumns.shader");
                        break;
                    case RM_Object.Mix.OpUnionStairs:          // box, sphere, r, n
                        mixStr += "fOpUnionStairs";
                        specialFuncs = "," + strR + "," + strN;
                        includeCode.Add("Operators/OpUnionStairs.shader");
                        break;
                    case RM_Object.Mix.OpIntersectionStairs:   // box, sphere, r, n
                        mixStr += "fOpIntersectionStairs";
                        specialFuncs = "," + strR + "," + strN;
                        includeCode.Add("Operators/OpIntersectionStairs.shader");
                        break;
                    case RM_Object.Mix.OpDifferenceStairs:     // box, sphere, r, n
                        mixStr += "fOpDifferenceStairs";
                        specialFuncs = "," + strR + "," + strN;
                        includeCode.Add("Operators/OpDifferenceStairs.shader");
                        break;
                    case RM_Object.Mix.OpPipe:                 // box, sphere, r*0.3
                        mixStr += "fOpPipe";
                        specialFuncs = "," + strR + "," + strN;
                        includeCode.Add("Operators/OpPipe.shader");
                        break;
                    case RM_Object.Mix.OpEngrave:              // box, sphere, r*0.3
                        mixStr += "fOpEngrave";
                        specialFuncs = "," + strR + "," + strN;
                        includeCode.Add("Operators/OpEngrave.shader");
                        break;
                    case RM_Object.Mix.OpGroove:               // box, sphere ,r*0.3, r*0.3
                        mixStr += "fOpGroove";
                        specialFuncs = "," + strR + "," + strN;
                        includeCode.Add("Operators/OpGroove.shader");
                        break;
                    case RM_Object.Mix.OpTongue:               // box, sphere, r*0.3, r*0.3
                        mixStr += "fOpTongue";
                        specialFuncs = "," + strR + "," + strN;
                        includeCode.Add("Operators/OpTongue.shader");
                        break;
                    case RM_Object.Mix.OpUnionSoft:            // box, sphere, r*0.3, r*0.3
                        mixStr += "fOpUnionSoft";
                        specialFuncs = "," + strR + "," + strN;
                        includeCode.Add("Operators/OpUnionSoft.shader");
                        break;
                }
                int index = rmObject.ID * 10;
                sdf1 += "               vec3 posID" + rmObject.ID + " = position - vec3(_Objects[" + (index + 0) + "], _Objects[" + (index + 1) + "], _Objects[" + (index + 2) + "]);\n";
                sdf1 += "               posID" + rmObject.ID + "= posID" + rmObject.ID + "*rotationMatrix(vec3(_Objects[" + (index + 6) + "], _Objects[" + (index + 7) + "], _Objects[" + (index + 8) + "]),  _Objects[" + (index + 9) + "]);\n";
                string firstPart = "               id" + materialID + "_distance ";
                switch (scaleFormat)
                {
                    case ShaderComponent.ScaleInfo.OneDimension:
                        sdf1 += firstPart + " = "+ mixStr + "(" + functionName.ToString() + "(posID" + rmObject.ID + ",_Objects[" + (index + 3) + "]), id" + materialID + "_distance" + specialFuncs + ");\n";
                        break;
                    case ShaderComponent.ScaleInfo.TwoDimension:
                        sdf1 += firstPart + " = " + mixStr + "(" + functionName.ToString() + "(posID" + rmObject.ID + ", vec2(_Objects[" + (index + 3) + "], _Objects[" + (index + 4) + "])), id" + materialID + "_distance "+ specialFuncs + ");\n";
                        break;
                    case ShaderComponent.ScaleInfo.ThreeDimension:
                        sdf1 += firstPart + " = " + mixStr + "(" + functionName.ToString() + "(posID" + rmObject.ID + ", vec3(_Objects[" + (index + 3) + "], _Objects[" + (index + 4) + "],_Objects[" + (index + 5) + "])), id" + materialID + "_distance" + specialFuncs + ");\n";
                        break;
                }
            }
            sdf1 += "               vec4 distID" + materialID + " = vec4(id" + materialID + "_distance, material_ID" + materialID + ", position.xz + vec2(position.y, 0.0));\n";

            if((surface as RM_Material) != null && ((RM_Material)surface).albedo.a < 1.0f)
            {
                sdf1 += "               result = DistUnionCombineTransparent(result, distID" + materialID + ", transparencyPointer);\n\n";
            }
            else
            {
                sdf1 += "               result = DistUnionCombine(result, distID" + materialID + ");\n\n";
            }
            materialID++;
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
        string sdfIncludes = "\n";
        foreach( string sdfInclude in includeCode)
        {
            var shaderCode = GetShaderPart(sdfInclude);
            sdfIncludes += shaderCode + "\n";
        }
        sdf = sdfIncludes + sdf0 + sdf1 + sdf2;
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
        for (int i = 0; i < surfaces.Count; i++)
        {
            if ((surfaces[i] as RM_Material) != null)
            {
                var material = surfaces[i] as RM_Material;
                mat1 += "       if (hitNfo.id.x == material_ID" + i + "){\n";
                mat1 += "              mat.reflectionCoefficient = " + material.reflectionCoefficient.ToString(culture);
                mat1 += ";\n              mat.albedo = vec3(" + material.albedo.r.ToString(culture) + "," + material.albedo.g.ToString(culture) + "," + material.albedo.b.ToString(culture) + ");";
                mat1 += ";\n              mat.transparency =" + (1.0f - material.albedo.a).ToString(culture);
                mat1 += ";\n              mat.smoothness = " + material.smoothness.ToString(culture);
                mat1 += ";\n              mat.reflectindx = " + material.reflectindx.ToString(culture);
                mat1 += ";\n       }\n";
            } else if((surfaces[i] as RM_Pattern) != null)
            {
                var surface = surfaces[i] as RM_Pattern;
                var functionName = surface.FunctionName;
                mat1 += "       if (hitNfo.id.x == material_ID" + i + "){\n";
                mat1 += "              mat = " + functionName + "(hitNfo.position);\n";
                mat1 += ";\n       }\n";
            }

        }

        string mat2 = @"
            return mat;
        }";
        mat = mat0 + mat1 + mat2;
        return mat;
    }

    static void WriteString(string filename, string code)
    {
        string path = filename;

        StreamWriter writer = new StreamWriter(path, false);
        writer.WriteLine(code);
        writer.Close();

        AssetDatabase.ImportAsset(path);
    }
    private void Start()
    {
        int index = 0;
        //detect all gameobjects with raymarch objects
        var rM_Objects = FindObjectsOfType<RM_Object>();
        foreach (var rM_Object in rM_Objects)
        {
            var materialName = rM_Object.SurfaceComponent.name;
            rM_Object.SetID(index++);
        }

        OrganizeShader();
    }
    public void OrganizeShader()
    {
        string coreCode = "";
        var GOs = FindObjectsOfType<RM_Object>();
        gameObjects = new List<GameObject>();

        foreach (var GO in GOs)
        {
            gameObjects.Add(GO.gameObject);
            var material = GO.GetComponent<RM_Object>().SurfaceComponent;
            if (material != null)
            {
                getMaterialRegister(material);
            }
        }
        gomat = new Dictionary<RM_Surface, List<RM_Object>>();
        foreach ( var GO in GOs)
        {
            var rmObj = GO.GetComponent<RM_Object>();
            var material = rmObj.SurfaceComponent;
            if (!gomat.ContainsKey(material)) {
                gomat.Add(material,new List<RM_Object>() );
            }
            gomat[material].Add(rmObj);
        }
        string fullcode = shaderBeginning;

        fullcode += "\n" + GetShaderPart("General/Vertex.shader");
        fullcode += "\n" + GetShaderPart("StartFragment.shader");

        foreach (string part in begin)
        {
            coreCode += "\n" + GetShaderPart(part);
        }
        coreCode += "\n" + materialIDStr;
        coreCode += "\n uniform float _Objects[" + (gameObjects.Count * 10) + "];";
        var sdfTextArray = new List<TextAsset>();
        for (int i = 0; i < gameObjects.Count; i++)
        {
            var rmObject = gameObjects[i].GetComponent<RM_Object>();
            if (rmObject.ShaderComponent == null)
            {
                Debug.LogWarning("Missing Shader Component", rmObject.gameObject);
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
            coreCode += "\n" + shaderCode;
        }


        coreCode += "\n" + GetDistanceFields();


        for (int i = 0; i < surfaces.Count; i++)
        {
            if ((surfaces[i] as RM_Pattern) != null)
            {
                var surface = surfaces[i] as RM_Pattern;
                coreCode += GetShaderPart("Patterns/" + surface.name + ".glslinc");
            }
        }


            coreCode += "\n" + GetMaterials();
        foreach (string part in end)
        {
            coreCode += "\n" + GetShaderPart(part);
        }
        fullcode += coreCode + "\n" + shaderEnding;
        coreCode.Replace("uniform float _Objects", "varying float _Objects");


        WriteString("../IntroFramework/src/shaders/fragment.frag", "#version 130\n\n" + coreCode);
        WriteString("Assets/Shaders/Megashader.shader", fullcode);
    }
    void Update()
    {
        if (Input.GetKeyUp("space"))
        {
            OrganizeShader();
        }
    }

    // Update is called once per frame
    void LateUpdate()
    {
        if (RM_Camera.RM_Objects == null)
        {
            RM_Camera.RM_Objects = new List<float>();
        }
        RM_Camera.RM_Objects.Clear();

        foreach (var material in surfaces)
        {
            if(gomat.ContainsKey(material)) {
            for (int i = 0; i < gomat[material].Count; i++)
            {
                var GO = gomat[material][i].gameObject;
                    var rmObject = GO.GetComponent<RM_Object>();
                    var index = rmObject.ID;
                RM_Camera.RM_Objects.Add(GO.transform.position.x);
                RM_Camera.RM_Objects.Add(GO.transform.position.y);
                RM_Camera.RM_Objects.Add(GO.transform.position.z);
                RM_Camera.RM_Objects.Add(GO.transform.localScale.x / 2.0f);
                RM_Camera.RM_Objects.Add(GO.transform.localScale.y / 2.0f);
                RM_Camera.RM_Objects.Add(GO.transform.localScale.z / 2.0f);
                RM_Camera.RM_Objects.Add(GO.transform.rotation.x);
                RM_Camera.RM_Objects.Add(GO.transform.rotation.y);
                RM_Camera.RM_Objects.Add(GO.transform.rotation.z);
                RM_Camera.RM_Objects.Add(GO.transform.rotation.w);
            }
            }
        }
    }

}