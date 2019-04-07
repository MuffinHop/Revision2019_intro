using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEditor;
using UnityEngine;
[ExecuteInEditMode, Serializable, CreateAssetMenu(fileName = "ShaderComponent", menuName = "Shader Component")]
public class ShaderComponent : ScriptableObject
{ 

    public enum ScaleInfo
    {
        OneDimension,
        TwoDimension,
        ThreeDimension,
        FourDimension
    };

    public TextAsset TextFile;
    public ScaleInfo Scale;
    public string FunctionName;
    public List<TextAsset> AdditionalShaderFiles;

}
