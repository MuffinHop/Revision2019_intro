using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExecuteInEditMode, System.Serializable]
public class RM_Object : MonoBehaviour
{ 
    private RM_Type type;

    internal RM_Type Type { get => type; set => type = value; }

    public ShaderComponent ShaderComponent;
    public RM_Material MaterialComponent;

    private ShaderComponent compShaderComponent;
    private RM_Material compMaterialComponent;
    IEnumerator coroutine;
    private void Awake()
    {
        coroutine = Recheck();
        StartCoroutine(coroutine);
        compShaderComponent = Instantiate(ShaderComponent);
        compMaterialComponent = Instantiate(MaterialComponent);
    }
    private void OnDestroy()
    {
        StopCoroutine(coroutine);
    }
    private IEnumerator Recheck()
    {
        while (true)
        {
            if (compShaderComponent == null ||
                compShaderComponent.Mixer != ShaderComponent.Mixer ||
                compShaderComponent.Scale != ShaderComponent.Scale ||
                compShaderComponent.TextFile != ShaderComponent.TextFile)
            {
                FindObjectOfType<RayMarchingController>().OrganizeShader();
                compShaderComponent = Instantiate(ShaderComponent);
            }
            if (compMaterialComponent == null ||
                compMaterialComponent.albedo != MaterialComponent.albedo ||
                compMaterialComponent.reflectindx != MaterialComponent.reflectindx ||
                compMaterialComponent.reflectionCoefficient != MaterialComponent.reflectionCoefficient ||
                compMaterialComponent.smoothness != MaterialComponent.smoothness)
            {
                FindObjectOfType<RayMarchingController>().OrganizeShader();
                compMaterialComponent = Instantiate(MaterialComponent);
            }
            yield return new WaitForSeconds(1.2f);
        }
    }
}
