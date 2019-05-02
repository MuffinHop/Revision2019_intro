using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExecuteInEditMode, Serializable, CreateAssetMenu(fileName = "TextViewerComponent", menuName = "Text Viewer Component")]
public class TextViewerComponent : RM_Surface
{
    public string[] textSceneNames;
}
