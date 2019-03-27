using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExecuteInEditMode, System.Serializable]
public class RM_Object : MonoBehaviour
{
    public enum Mix
    {
        Min,                    // box, sphere
        Max,                    // box, sphere
        MaxMinus,               // box, sphere
        OpUnionRound,           // box, sphere, r
        OpIntersectionRound,    // box, sphere, r
        OpDifferenceRound,      // box, sphere, r
        OpUnionChamfer,         // box, sphere, r
        OpIntersectionChamfer,  // box, sphere, r
        OpDifferenceChamfer,    // box, sphere, r
        OpUnionColumns,         // box, sphere, r, n
        OpIntersectionColumns,  // box, sphere, r, n
        OpDifferenceColumns,    // box, sphere, r, n
        OpUnionStairs,          // box, sphere, r, n
        OpIntersectionStairs,   // box, sphere, r, n
        OpDifferenceStairs,     // box, sphere, r, n
        OpPipe,                 // box, sphere, r*0.3
        OpEngrave,              // box, sphere, r*0.3
        OpGroove,               // box, sphere ,r*0.3, r*0.3
        OpTongue,               // box, sphere, r*0.3, r*0.3
        OpUnionSoft             // box, sphere, r*0.3, r*0.3
    };

    private RM_Type type;

    internal RM_Type Type { get => type; set => type = value; }

    public ShaderComponent ShaderComponent;
    public RM_Material MaterialComponent;
    public Mix Mixer;
    public float R, N;

    private ShaderComponent compShaderComponent;
    private RM_Material compMaterialComponent;
}
