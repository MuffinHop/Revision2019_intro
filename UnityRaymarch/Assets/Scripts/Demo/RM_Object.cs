using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExecuteInEditMode, System.Serializable]
public class RM_Object : MonoBehaviour
{ 
    private RM_Type type;

    internal RM_Type Type { get => type; set => type = value; }

    public ShaderComponent ShaderComponent;
    
}
