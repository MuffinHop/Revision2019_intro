using System;
using UnityEngine;

[ExecuteInEditMode, Serializable, CreateAssetMenu(fileName = "MaterialComponent", menuName = "Material Component")]
public class RMMaterial : ScriptableObject
{
    [Range(0.0f,1.0f)]
    public float reflectionCoefficient;
    public Color albedo;
    [Range(0.0f, 1.0f)]
    public float smoothness;
    [Range(0.0f, 1.0f)]
    public float reflectindx;
}
