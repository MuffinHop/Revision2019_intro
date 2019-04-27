using UnityEngine;
using UnityEditor;
using UnityEditor.SceneManagement;
using UnityEngine.SceneManagement;

public class KeyboardShortcuts : ScriptableObject {

    [MenuItem("Edit/Refresh Shaders _F5")]
    static void RefreshShaders() {
        RayMarchingController rm = GameObject.Find("Main Camera").GetComponent<RayMarchingController>();
        if (rm.GeneratedOnce) rm.RegenerateShader();
    }
}