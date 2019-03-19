using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

[ExecuteInEditMode, RequireComponent(typeof(Camera))]
public class RM_MenuObjects : MonoBehaviour
{
    [MenuItem("GameObject/Ray Marching/Cube (Ray Marched)", false, 10)]
    static void CreateCustomGameObject(MenuCommand menuCommand)
    {
        GameObject go = new GameObject("Cube Ray Marched");
        go.AddComponent<RM_Object>();
        GameObjectUtility.SetParentAndAlign(go, menuCommand.context as GameObject);
        Undo.RegisterCreatedObjectUndo(go, "Create " + go.name);
        Selection.activeObject = go;
    }
}
