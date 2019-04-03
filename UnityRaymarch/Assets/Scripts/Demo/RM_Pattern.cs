using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


[ExecuteInEditMode, Serializable, CreateAssetMenu(fileName = "PatternComponent", menuName = "Pattern Component")]
public class RM_Pattern : RM_Surface
{
    public TextAsset TextFile;
    public string FunctionName;
}
