using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEditor;
using UnityEngine;
[ExecuteInEditMode]
public class ShaderComponent : MonoBehaviour
{
    public enum Mix {
        Min,                    // box, sphere
        Max,                    // box, sphere
        MinMinus,               // box, sphere
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
    public TextAsset TextFile;
    public Mix Mixer;

    void asdf()
    {
        //hack
        string path = AssetDatabase.GetAssetPath(TextFile);

        //Read the text from directly from the test.txt file
        StreamReader reader = new StreamReader(path);
        Debug.Log(reader.ReadToEnd()); 
        reader.Close();
    }
}
