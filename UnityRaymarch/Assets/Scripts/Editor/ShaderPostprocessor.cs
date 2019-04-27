using UnityEngine;
using UnityEditor;

class ShaderPostprocessor : AssetPostprocessor
{
    static void OnPostprocessAllAssets(string[] importedAssets, string[] deletedAssets, string[] movedAssets, string[] movedFromAssetPaths)
    {
        foreach (string str in importedAssets)
        {
            if (str.Contains("Shaders") && !str.Contains("Megashader"))
            {
                RayMarchingController rm = GameObject.Find("Main Camera").GetComponent<RayMarchingController>();
                if (rm.GeneratedOnce) rm.RegenerateShader();
            }
        }
    }
}