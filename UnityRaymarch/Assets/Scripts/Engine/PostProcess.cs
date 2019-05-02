using System.Collections;
using System.Collections.Generic;
using UnityEngine;
[ExecuteInEditMode]
public class PostProcess : MonoBehaviour
{
    public Shader shader;
    private Material material;
    private void Start()
    {
        material = new Material(shader);
    }

    void OnRenderImage(RenderTexture src, RenderTexture dest)
    {
        material.SetVector("_Gain", new Vector4(SyncUp.GetVal("Gain_R"), SyncUp.GetVal("Gain_G"), SyncUp.GetVal("Gain_B"),0f));
        material.SetVector("_Gamma", new Vector4(SyncUp.GetVal("Gamma_R"), SyncUp.GetVal("Gamma_G"), SyncUp.GetVal("Gamma_B"), 0f));
        material.SetVector("_Lift", new Vector4(SyncUp.GetVal("Lift_R"), SyncUp.GetVal("Lift_G"), SyncUp.GetVal("Lift_B"), 0f));
        material.SetVector("_Presaturation", new Vector4(SyncUp.GetVal("Presaturation_R"), SyncUp.GetVal("Presaturation_G"), SyncUp.GetVal("Presaturation_B"), 0f));
        material.SetVector("_ColorTemperatureStrength", new Vector4(SyncUp.GetVal("ColorTemperatureStrength_R"), SyncUp.GetVal("ColorTemperatureStrength_G"), SyncUp.GetVal("ColorTemperatureStrength_B"), 0f));
        material.SetFloat("_ColorTemprature", SyncUp.GetVal("ColorTemprature"));
        material.SetFloat("_TempratureNormalization", SyncUp.GetVal("TempratureNormalization"));
        material.SetVector("iResolution", new Vector4(src.width, src.height, src.width, src.height));
        Graphics.Blit(src, dest, material);
    }
}
