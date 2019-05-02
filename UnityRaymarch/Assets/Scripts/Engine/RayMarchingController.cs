using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEditor;
using UnityEngine;

public class RayMarchingController : MonoBehaviour
{
    private List<RM_Surface> surfaces;
    private Dictionary<RM_Surface, List<SDFObject>> gomat;
    public Dictionary<RM_Surface, List<SDFObject>> GetSortedObjectList()
    {
        return gomat;
    }
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
        if (System.IO.File.Exists(path))
        {
            StreamReader reader = new StreamReader(path);
            string code = reader.ReadToEnd();
            reader.Close();
            return code;
        }
        else
        {
            return null;
        }
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
        if (surfaces == null)
        {
            surfaces = new List<RM_Surface>();
        }
        List<string> includeCode = new List<string>();
        string sdf;
        string sdf0 = @"vec4 GetDistanceScene(vec3 position, in float transparencyPointer)
        {
            vec4 result = vec4(10000.0, -1.0, 0.0, 0.0);
        ";
        string sdf1 = "\n";
        var culture = System.Globalization.CultureInfo.InvariantCulture;
        int materialID = 0;
        foreach (var surface in surfaces)
        {
            sdf1 += "         float id" + materialID + "_distance = 1e9;\n";
            for (int i = 0; i < gomat[surface].Count; i++)
            {
                var rmObject = gomat[surface][i];
                if (rmObject.SDFComponent == null)
                {
                    Debug.LogWarning(rmObject.name + "missing shadercomponent", rmObject.gameObject);
                    continue;
                }
                var functionName = rmObject.SDFComponent.FunctionName;
                var scaleFormat = rmObject.SDFComponent.Scale;
                var mixerFormat = rmObject.Mixer;
                string mixStr = "";
                string specialFuncs = "";
                string strR = rmObject.R.ToString(culture);
                string strN = rmObject.N.ToString(culture);
                if ((rmObject as SDFDualPartObject) == null)
                {
                    switch (mixerFormat)
                    {
                        case SDFObject.Mix.Min:
                            mixStr += "min";
                            break;
                        case SDFObject.Mix.Max:
                            mixStr += "max";
                            break;
                        case SDFObject.Mix.MaxMinus:
                            mixStr += "maxMinus";
                            break;
                        case SDFObject.Mix.OpUnionRound:           // box, sphere, r
                            mixStr += "fOpUnionRound";
                            specialFuncs = "," + strR;
                            includeCode.Add("Operators/OpUnionRound.shader");
                            break;
                        case SDFObject.Mix.OpIntersectionRound:    // box, sphere, r
                            mixStr += "fOpIntersectionRound";
                            specialFuncs = "," + strR;
                            includeCode.Add("Operators/OpIntersectionRound.shader");
                            break;
                        case SDFObject.Mix.OpDifferenceRound:      // box, sphere, r
                            mixStr += "fOpDifferenceRound";
                            specialFuncs = "," + strR;
                            includeCode.Add("Operators/OpDifferenceRound.shader");
                            break;
                        case SDFObject.Mix.OpUnionChamfer:         // box, sphere, r
                            mixStr += "fOpUnionChamfer";
                            specialFuncs = "," + strR;
                            includeCode.Add("Operators/OpUnionChamfer.shader");
                            break;
                        case SDFObject.Mix.OpIntersectionChamfer:  // box, sphere, r
                            mixStr += "fOpIntersectionChamfer";
                            specialFuncs = "," + strR;
                            includeCode.Add("Operators/OpIntersectionChamfer.shader");
                            break;
                        case SDFObject.Mix.OpDifferenceChamfer:    // box, sphere, r
                            mixStr += "fOpDifferenceChamfer";
                            specialFuncs = "," + strR;
                            includeCode.Add("Operators/OpDifferenceChamfer.shader");
                            break;
                        case SDFObject.Mix.OpUnionColumns:         // box, sphere, r, n
                            mixStr += "fOpUnionColumns";
                            specialFuncs = "," + strR + "," + strN;
                            includeCode.Add("Operators/OpUnionColumns.shader");
                            break;
                        case SDFObject.Mix.OpIntersectionColumns:  // box, sphere, r, n
                            mixStr += "fOpIntersectionColumns";
                            specialFuncs = "," + strR + "," + strN;
                            includeCode.Add("Operators/OpIntersectionColumns.shader");
                            break;
                        case SDFObject.Mix.OpDifferenceColumns:    // box, sphere, r, n
                            mixStr += "fOpDifferenceColumns";
                            specialFuncs = "," + strR + "," + strN;
                            includeCode.Add("Operators/OpDifferenceColumns.shader");
                            break;
                        case SDFObject.Mix.OpUnionStairs:          // box, sphere, r, n
                            mixStr += "fOpUnionStairs";
                            specialFuncs = "," + strR + "," + strN;
                            includeCode.Add("Operators/OpUnionStairs.shader");
                            break;
                        case SDFObject.Mix.OpIntersectionStairs:   // box, sphere, r, n
                            mixStr += "fOpIntersectionStairs";
                            specialFuncs = "," + strR + "," + strN;
                            includeCode.Add("Operators/OpIntersectionStairs.shader");
                            break;
                        case SDFObject.Mix.OpDifferenceStairs:     // box, sphere, r, n
                            mixStr += "fOpDifferenceStairs";
                            specialFuncs = "," + strR + "," + strN;
                            includeCode.Add("Operators/OpDifferenceStairs.shader");
                            break;
                        case SDFObject.Mix.OpPipe:                 // box, sphere, r*0.3
                            mixStr += "fOpPipe";
                            specialFuncs = "," + strR + "," + strN;
                            includeCode.Add("Operators/OpPipe.shader");
                            break;
                        case SDFObject.Mix.OpEngrave:              // box, sphere, r*0.3
                            mixStr += "fOpEngrave";
                            specialFuncs = "," + strR + "," + strN;
                            includeCode.Add("Operators/OpEngrave.shader");
                            break;
                        case SDFObject.Mix.OpGroove:               // box, sphere ,r*0.3, r*0.3
                            mixStr += "fOpGroove";
                            specialFuncs = "," + strR + "," + strN;
                            includeCode.Add("Operators/OpGroove.shader");
                            break;
                        case SDFObject.Mix.OpTongue:               // box, sphere, r*0.3, r*0.3
                            mixStr += "fOpTongue";
                            specialFuncs = "," + strR + "," + strN;
                            includeCode.Add("Operators/OpTongue.shader");
                            break;
                        case SDFObject.Mix.OpUnionSoft:            // box, sphere, r*0.3, r*0.3
                            mixStr += "fOpUnionSoft";
                            specialFuncs = "," + strR + "," + strN;
                            includeCode.Add("Operators/OpUnionSoft.shader");
                            break;
                    }
                    sdf1 += "       if (OBJMAX > " + rmObject.ID + " && " + rmObject.ID + " > OBJMIN) { \n";
                    sdf1 += "\n//" + rmObject.name + "\n";
                    int index = rmObject.ID * 10;
                    if (!rmObject.Centered)
                    {
                        sdf1 += "               vec3 posID" + rmObject.ID + " = position - vec3(_Objects[" + (index + 0) + "], _Objects[" + (index + 1) + "], _Objects[" + (index + 2) + "]);\n";
                    }
                    else
                    {
                        sdf1 += "               #define posID" + rmObject.ID + " position\n";
                    }
                    if (rmObject.Rotatable)
                    {
                        sdf1 += "               posID" + rmObject.ID + "= RotateQuaternion(vec4(_Objects[" + (index + 6) + "], _Objects[" + (index + 7) + "], _Objects[" + (index + 8) + "], - _Objects[" + (index + 9) + "]))*posID" + rmObject.ID + ";\n";
                    }
                    string firstPart = "               id" + materialID + "_distance ";


                    switch (scaleFormat)
                    {
                        case SDFComponent.ScaleInfo.OneDimension:
                            sdf1 += firstPart + " = " + mixStr + "(" + functionName.ToString() + "(posID" + rmObject.ID + ",_Objects[" + (index + 3) + "]), id" + materialID + "_distance" + specialFuncs + ");\n";
                            break;
                        case SDFComponent.ScaleInfo.TwoDimension:
                            sdf1 += firstPart + " = " + mixStr + "(" + functionName.ToString() + "(posID" + rmObject.ID + ", vec2(_Objects[" + (index + 3) + "], _Objects[" + (index + 4) + "])), id" + materialID + "_distance " + specialFuncs + ");\n";
                            break;
                        case SDFComponent.ScaleInfo.ThreeDimension:
                            sdf1 += firstPart + " = " + mixStr + "(" + functionName.ToString() + "(posID" + rmObject.ID + ", vec3(_Objects[" + (index + 3) + "], _Objects[" + (index + 4) + "],_Objects[" + (index + 5) + "])), id" + materialID + "_distance" + specialFuncs + ");\n";
                            break;
                    }
                    sdf1 += "       }\n";


                }
                else //SdCapsule
                {
                    switch (mixerFormat)
                    {
                        case SDFObject.Mix.Min:
                            mixStr += "min";
                            break;
                        case SDFObject.Mix.Max:
                            mixStr += "max";
                            break;
                        case SDFObject.Mix.MaxMinus:
                            mixStr += "maxMinus";
                            break;
                        case SDFObject.Mix.OpUnionRound:           // box, sphere, r
                            mixStr += "fOpUnionRound";
                            specialFuncs = "," + strN;
                            includeCode.Add("Operators/OpUnionRound.shader");
                            break;
                        case SDFObject.Mix.OpIntersectionRound:    // box, sphere, r
                            mixStr += "fOpIntersectionRound";
                            specialFuncs = "," + strN;
                            includeCode.Add("Operators/OpIntersectionRound.shader");
                            break;
                        case SDFObject.Mix.OpDifferenceRound:      // box, sphere, r
                            mixStr += "fOpDifferenceRound";
                            specialFuncs = "," + strN;
                            includeCode.Add("Operators/OpDifferenceRound.shader");
                            break;
                        case SDFObject.Mix.OpUnionChamfer:         // box, sphere, r
                            mixStr += "fOpUnionChamfer";
                            specialFuncs = "," + strN;
                            includeCode.Add("Operators/OpUnionChamfer.shader");
                            break;
                        case SDFObject.Mix.OpIntersectionChamfer:  // box, sphere, r
                            mixStr += "fOpIntersectionChamfer";
                            specialFuncs = "," + strN;
                            includeCode.Add("Operators/OpIntersectionChamfer.shader");
                            break;
                        case SDFObject.Mix.OpDifferenceChamfer:    // box, sphere, r
                            mixStr += "fOpDifferenceChamfer";
                            specialFuncs = "," + strN;
                            includeCode.Add("Operators/OpDifferenceChamfer.shader");
                            break;
                    }
                    sdf1 += "       if (OBJMAX > " + rmObject.ID + " && " + rmObject.ID + " > OBJMIN) { \n";
                    sdf1 += "\n//" + rmObject.name + "\n";
                    int index = rmObject.ID * 10;
                    string firstPart = "               id" + materialID + "_distance ";


                    sdf1 += firstPart + " = " + mixStr + "(sdCapsule(position, vec3(_Objects[" + (index + 0) + "], _Objects[" + (index + 1) + "],_Objects[" + (index + 2) + "]), vec3(_Objects[" + (index + 3) + "], _Objects[" + (index + 4) + "],_Objects[" + (index + 5) + "]), " + strR + "), id" + materialID + "_distance" + specialFuncs + ");\n";
                    sdf1 += "       }\n";


                }
            }
            sdf1 += "               vec4 distID" + materialID + " = vec4(id" + materialID + "_distance, material_ID" + materialID + ", position.xz + vec2(position.y, 0.0));\n";

            if ((surface as RM_Material) != null && ((RM_Material)surface).albedo.a < 1.0f)
            {
                sdf1 += "               result = DistUnionCombineTransparent(result, distID" + materialID + ", transparencyPointer);\n\n";
            }
            else
            {
                sdf1 += "               result = DistUnionCombine(result, distID" + materialID + ");\n\n";
            }
            materialID++;
        }

        string sdf2 = @"
            return result;
        }";
        string sdfIncludes = "\n";
        List<string> includedModifiers = new List<string>();
        foreach (string sdfInclude in includeCode)
        {
            if (!includedModifiers.Contains(sdfInclude))
            {
                includedModifiers.Add(sdfInclude);
                var shaderCode = GetShaderPart(sdfInclude);
                sdfIncludes += shaderCode + "\n";
            }
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
            }
            else if ((surfaces[i] as RM_Pattern) != null)
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
        RegenerateShader();
    }

    public bool GeneratedOnce = false;

    public void RegenerateShader()
    {
        int index = 0;
        gomat = null;
        surfaces = null;
        //detect all gameobjects with raymarch objects
        materialIDStr = "";
        var rM_Objects = FindObjectsOfType<SDFObject>();
        foreach (var rM_Object in rM_Objects)
        {
            var surfaceComponent = rM_Object.SurfaceComponent;
            var materialName = surfaceComponent.name;
            rM_Object.SetID(index++);
            getMaterialRegister(surfaceComponent);
        }

        OrganizeShader();
        GeneratedOnce = true;
    }

    public void OrganizeShader()
    {
        string coreCode = "";
        var rmObjects = FindObjectsOfType<SDFObject>();

        int validRMObjects = 0;
        gomat = new Dictionary<RM_Surface, List<SDFObject>>();
        foreach (var GO in rmObjects)
        {
            var material = GO.SurfaceComponent;
            if (!gomat.ContainsKey(material))
            {
                gomat.Add(material, new List<SDFObject>());
            }
            gomat[material].Add(GO);
            validRMObjects++;
        }
        string fullcode = shaderBeginning;

        fullcode += "\n" + GetShaderPart("General/Vertex.shader");
        fullcode += "\n" + GetShaderPart("StartFragment.shader");

        foreach (string part in begin)
        {
            coreCode += "\n" + GetShaderPart(part);
        }
        coreCode += "\n" + materialIDStr;
        coreCode += "\n uniform float _Objects[" + (validRMObjects * 10) + "];";
        var sdfTextArray = new List<TextAsset>();
        foreach (var rmObject in rmObjects)
        {
            if (rmObject.SDFComponent == null)
            {
                Debug.LogWarning("Missing Shader Component", rmObject.gameObject);
                continue;
            }
            var shaderComponent = rmObject.SDFComponent;
            foreach (var additionalShaderFile in shaderComponent.AdditionalShaderFiles)
            {
                if (!sdfTextArray.Contains(additionalShaderFile) && additionalShaderFile != null)
                {
                    sdfTextArray.Add(additionalShaderFile);
                }
                else if (additionalShaderFile == null)
                {
                    Debug.LogWarning("Shader Component *Additional* Text File Missing", rmObject);
                }
            }
            var textFile = rmObject.SDFComponent.TextFile;
            if (!sdfTextArray.Contains(textFile) && textFile != null)
            {
                sdfTextArray.Add(textFile);
            }
            else if (textFile == null)
            {
                Debug.LogWarning("Shader Component Text File Missing", rmObject);
            }
        }

        for (int i = 0; i < sdfTextArray.Count; i++)
        {
            var shaderPath0 = "SDFs/Primitive/" + sdfTextArray[i].name + ".glslinc";
            var shaderPath1 = "SDFs/Custom/" + sdfTextArray[i].name + ".glslinc";
            var shaderCode = GetShaderPart(shaderPath0);
            if (shaderCode == null)
            {
                shaderCode = GetShaderPart(shaderPath1);
            }
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
            foreach (var material in surfaces)
            {
                if (gomat.ContainsKey(material))
                {


                    // add gomat[material].Count * 10 zero's to RM_Camera.RM_Objects
                    for (int i = 0; i < gomat[material].Count; i++)
                    {
                        //lazylazylazylazylazylazylazylazy
                        for (int j = 0; j < 10; j++)
                        {
                            RM_Camera.RM_Objects.Add(0);
                        }
                    }


                }
            }
        }

        foreach (var material in surfaces)
        {
            if (gomat.ContainsKey(material))
            {
                for (int i = 0; i < gomat[material].Count; i++)
                {
                    var GO = gomat[material][i].gameObject;
                    var rmObject = GO.GetComponent<SDFObject>();
                    var index = rmObject.ID;
                    var temp = GO.transform.parent;
                    GO.transform.parent = null;
                    if ((rmObject as SDFDualPartObject) == null)
                    {
                        RM_Camera.RM_Objects[index * 10 + 0] = GO.transform.position.x;
                        RM_Camera.RM_Objects[index * 10 + 1] = GO.transform.position.y;
                        RM_Camera.RM_Objects[index * 10 + 2] = GO.transform.position.z;
                        RM_Camera.RM_Objects[index * 10 + 3] = GO.transform.localScale.x / 2.0f;
                        RM_Camera.RM_Objects[index * 10 + 4] = GO.transform.localScale.y / 2.0f;
                        RM_Camera.RM_Objects[index * 10 + 5] = GO.transform.localScale.z / 2.0f;
                        RM_Camera.RM_Objects[index * 10 + 6] = GO.transform.rotation.x;
                        RM_Camera.RM_Objects[index * 10 + 7] = GO.transform.rotation.y;
                        RM_Camera.RM_Objects[index * 10 + 8] = GO.transform.rotation.z;
                        RM_Camera.RM_Objects[index * 10 + 9] = GO.transform.rotation.w;
                    } else // e.g. sd capsule
                    {
                        RM_Camera.RM_Objects[index * 10 + 0] = GO.transform.position.x;
                        RM_Camera.RM_Objects[index * 10 + 1] = GO.transform.position.y;
                        RM_Camera.RM_Objects[index * 10 + 2] = GO.transform.position.z;
                        RM_Camera.RM_Objects[index * 10 + 3] = GO.transform.GetChild(0).position.x;
                        RM_Camera.RM_Objects[index * 10 + 4] = GO.transform.GetChild(0).position.y;
                        RM_Camera.RM_Objects[index * 10 + 5] = GO.transform.GetChild(0).position.z;
                        RM_Camera.RM_Objects[index * 10 + 6] = GO.transform.rotation.x;
                        RM_Camera.RM_Objects[index * 10 + 7] = GO.transform.rotation.y;
                        RM_Camera.RM_Objects[index * 10 + 8] = GO.transform.rotation.z;
                        RM_Camera.RM_Objects[index * 10 + 9] = GO.transform.rotation.w;
                    }
                    GO.transform.parent = temp;
                }
            }
        }
    }

}