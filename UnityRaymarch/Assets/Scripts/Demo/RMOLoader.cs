using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEditor;
using UnityEngine;

public class RMOLoader : MonoBehaviour
{
    [SerializeField]
    private string rmoPath;

    private RMOCollection rmoCollection;

    [MenuItem("Assets/Load RMO(json)")]
    static public void LoadRMO()
    {
        var selection = Selection.activeObject;
        var rmoCollection = JsonUtility.FromJson<RMOCollection>(selection.ToString());
        Debug.Log(rmoCollection);
        var parent = new GameObject();
        parent.name = "RayMarched Object RMO";
        foreach(var sample in rmoCollection.Data)
        {
            GameObject go = sample.ToGameObject();
            go.transform.parent = parent.transform;
        }
    }
}
