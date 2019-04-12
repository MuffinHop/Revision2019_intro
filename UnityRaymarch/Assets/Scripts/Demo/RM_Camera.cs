using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExecuteInEditMode, RequireComponent(typeof(Camera))]
public class RM_Camera : MonoBehaviour
{
    [SerializeField]
    private Shader _shader;
    [SerializeField]
    private Material _material;
    [SerializeField]
    private Camera _camera;
    [SerializeField]
    private Texture iChannel0, iChannel1, iChannel2, iChannel3;
    [SerializeField]
    private RenderTexture _renderTexture;
    [SerializeField]
    private Vector2 _iResolution;

    public static List<float> RM_Objects;
    void Awake()
    {
        Debug.Log(SystemInfo.graphicsDeviceName);
        if (Application.isEditor)
        {
            _material = new Material(_shader);
        }
        _camera = GetComponent<Camera>();
        _camera.clearFlags = CameraClearFlags.Color;

        if (_renderTexture != null)
        {
            _renderTexture.Release();
            if (Screen.width > 0 && Screen.height > 0)
            {
                _renderTexture.width = Screen.width;
                _renderTexture.height = Screen.height;
                _renderTexture.Create();
                _camera.targetTexture = _renderTexture;
            }
        }
    }


    void OnRenderImage(RenderTexture src, RenderTexture dest)
    {
        if (_renderTexture != null)
        {
            if (_renderTexture.width != Screen.width && _renderTexture.height != Screen.height)
            {
                _renderTexture.Release();
                _renderTexture.width = Screen.width;
                _renderTexture.height = Screen.height;
                _renderTexture.Create();
                if (_renderTexture.width > 0 && _renderTexture.height > 0)
                {
                    _camera.targetTexture = _renderTexture;
                }
            }
            _iResolution = new Vector2(_renderTexture.width, _renderTexture.height);
        }
        else
        {
            _iResolution = new Vector2(Screen.width, Screen.height);
        }

        if (iChannel0 != null)
        {
            _material.SetTexture("_iChannel0", iChannel0);
        }
        if (iChannel1 != null)
        {
            _material.SetTexture("_iChannel1", iChannel1);
        }
        if (iChannel2 != null)
        {
            _material.SetTexture("_iChannel2", iChannel2);
        }
        if (iChannel3 != null)
        {
            _material.SetTexture("_iChannel3", iChannel3);
        }

        _material.SetVector("_iResolution", _iResolution);
        _material.SetFloat("_iTime", SyncUp.GetVal("iTime"));
        _material.SetFloat("_AspectCorrect", GetComponent<Camera>().rect.height);
        _material.SetVector("_DirectionalLight", new Vector4(SyncUp.GetVal("DirectionalLightX"), SyncUp.GetVal("DirectionalLightY"), SyncUp.GetVal("DirectionalLightZ"), 0f));
        _material.SetVector("_DirectionalLightColor", new Vector4(SyncUp.GetVal("DirectionalLightR"), SyncUp.GetVal("DirectionalLightG"), SyncUp.GetVal("DirectionalLightB"), 0f));
        _material.SetVector("_PointLightPosition", new Vector4(SyncUp.GetVal("PointLightPositionX"), SyncUp.GetVal("PointLightPositionY"), SyncUp.GetVal("PointLightPositionZ"), 0f));
        _material.SetVector("_PointLightColor", new Vector4(SyncUp.GetVal("PointLightR"), SyncUp.GetVal("PointLightG"), SyncUp.GetVal("PointLightB"), 0f));
        var cameraPosition = new Vector4(SyncUp.GetVal("CameraPositionX"), SyncUp.GetVal("CameraPositionY"), SyncUp.GetVal("CameraPositionZ"), 0f);
        var lookAt = new Vector4(SyncUp.GetVal("LookAtX"), SyncUp.GetVal("LookAtY"), SyncUp.GetVal("LookAtZ"), 0f);
        var cameraUp = new Vector4(SyncUp.GetVal("CameraUpX"), SyncUp.GetVal("CameraUpY"), SyncUp.GetVal("CameraUpZ"), 0f);
        var fov = SyncUp.GetVal("FOV");
        _camera.transform.position = cameraPosition;
        _camera.transform.up = cameraUp;
        _camera.transform.LookAt(lookAt);
        _camera.fieldOfView = fov;
        _material.SetVector("_CameraPosition", cameraPosition);
        _material.SetVector("_CameraLookAt", lookAt);
        _material.SetVector("_CameraUp", cameraUp);
        _material.SetFloat("_epsilon", SyncUp.GetVal("Epsilon"));
        _material.SetFloat("_FOV", fov * 2.0f);

        _material.SetFloat("_Distance", SyncUp.GetVal("Distance"));
        _material.SetFloat("_LensCoeff", SyncUp.GetVal("LensCoeff"));
        _material.SetFloat("_MaxCoC", SyncUp.GetVal("MaxCoC"));
        _material.SetFloat("_RcpMaxCoC", SyncUp.GetVal("RcpMaxCoC"));
        _material.SetFloat("_MarchMinimum", SyncUp.GetVal("MarchMinimum"));
        _material.SetFloat("_FarPlane", SyncUp.GetVal("FarPlane"));
        _material.SetFloat("_Step", SyncUp.GetVal("Step"));
        _material.SetFloat("fogDensity", SyncUp.GetVal("fogDensity"));
        _material.SetFloat("_TextId", SyncUp.GetVal("TextId"));




        if (RM_Objects != null && RM_Objects.Count > 0)
        {
            _material.SetFloatArray("_Objects", RM_Objects);
        }
        if (SystemInfo.graphicsDeviceName.ToUpper().Contains("NVIDIA") || SystemInfo.graphicsDeviceName.ToUpper().Contains("GEFORCE"))
        {
            _material.EnableKeyword("NVIDIA");
        }
        if (_material != null)
        {
            Graphics.Blit(src, dest, _material);
        }
    }
}