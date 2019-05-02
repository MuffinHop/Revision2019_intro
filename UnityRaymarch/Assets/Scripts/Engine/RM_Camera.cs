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

    private int _objMin = 0;
    private int _objMax = 64;

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

    private int _iChannel0_ID = Shader.PropertyToID("_iChannel0");
    private int _iChannel1_ID = Shader.PropertyToID("_iChannel1");
    private int _iChannel2_ID = Shader.PropertyToID("_iChannel2");
    private int _iChannel3_ID = Shader.PropertyToID("_iChannel3");
    private int _iResolution_ID = Shader.PropertyToID("_iResolution");
    private int _iTime_ID = Shader.PropertyToID("_iTime");
    private int _AspectCorrect_ID = Shader.PropertyToID("_AspectCorrect");
    private int _DirectionalLight_ID = Shader.PropertyToID("_DirectionalLight");
    private int _DirectionalLightColor_ID = Shader.PropertyToID("_DirectionalLightColor");
    private int _PointLightPosition_ID = Shader.PropertyToID("_PointLightPosition");
    private int _PointLightColor_ID = Shader.PropertyToID("_PointLightColor");
    private int _CameraPosition_ID = Shader.PropertyToID("_CameraPosition");
    private int _CameraLookAt_ID = Shader.PropertyToID("_CameraLookAt");
    private int _CameraUp_ID = Shader.PropertyToID("_CameraUp");
    private int _epsilon_ID = Shader.PropertyToID("_epsilon");
    private int _FOV_ID = Shader.PropertyToID("_FOV");
    private int _Distance_ID = Shader.PropertyToID("_Distance");
    private int _LensCoeff_ID = Shader.PropertyToID("_LensCoeff");
    private int _MaxCoC_ID = Shader.PropertyToID("_MaxCoC");
    private int _RcpMaxCoC_ID = Shader.PropertyToID("_RcpMaxCoC");
    private int _MarchMinimum_ID = Shader.PropertyToID("_MarchMinimum");
    private int _FarPlane_ID = Shader.PropertyToID("_FarPlane");
    private int _Step_ID = Shader.PropertyToID("_Step");
    private int _fogDensity_ID = Shader.PropertyToID("fogDensity");
    private int _TextId_ID = Shader.PropertyToID("_TextId");
    private int _iMouse_ID = Shader.PropertyToID("_iMouse");
    private int _Environment_ID = Shader.PropertyToID("_Environment");
    private int _StepIncreaseByDistance_ID = Shader.PropertyToID("_StepIncreaseByDistance");
    private int _StepIncreaseMax_ID = Shader.PropertyToID("_StepIncreaseMax");
    private int _Objects_ID = Shader.PropertyToID("_Objects");
    
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
            _material.SetTexture(_iChannel0_ID, iChannel0);
        }
        if (iChannel1 != null)
        {
            _material.SetTexture(_iChannel1_ID, iChannel1);
        }
        if (iChannel2 != null)
        {
            _material.SetTexture(_iChannel2_ID, iChannel2);
        }
        if (iChannel3 != null)
        {
            _material.SetTexture(_iChannel3_ID, iChannel3);
        }

        _material.SetVector(_iResolution_ID, _iResolution);
        _material.SetFloat(_iTime_ID, SyncUp.GetVal("iTime"));
        _material.SetFloat(_AspectCorrect_ID, GetComponent<Camera>().rect.height);
        _material.SetVector(_DirectionalLight_ID, new Vector4(SyncUp.GetVal("DirectionalLightX"), SyncUp.GetVal("DirectionalLightY"), SyncUp.GetVal("DirectionalLightZ"), 0f));
        _material.SetVector(_DirectionalLightColor_ID, new Vector4(SyncUp.GetVal("DirectionalLightR"), SyncUp.GetVal("DirectionalLightG"), SyncUp.GetVal("DirectionalLightB"), 0f));
        _material.SetVector(_PointLightPosition_ID, new Vector4(SyncUp.GetVal("PointLightPositionX"), SyncUp.GetVal("PointLightPositionY"), SyncUp.GetVal("PointLightPositionZ"), 0f));
        _material.SetVector(_PointLightColor_ID, new Vector4(SyncUp.GetVal("PointLightR"), SyncUp.GetVal("PointLightG"), SyncUp.GetVal("PointLightB"), 0f));
        var cameraPosition = new Vector4(SyncUp.GetVal("CameraPositionX"), SyncUp.GetVal("CameraPositionY"), SyncUp.GetVal("CameraPositionZ"), 0f);
        var lookAt = new Vector4(SyncUp.GetVal("LookAtX"), SyncUp.GetVal("LookAtY"), SyncUp.GetVal("LookAtZ"), 0f);
        var cameraUp = new Vector4(SyncUp.GetVal("CameraUpX"), SyncUp.GetVal("CameraUpY"), SyncUp.GetVal("CameraUpZ"), 0f);
        var fov = SyncUp.GetVal("FOV");
        _camera.transform.position = cameraPosition;
        _camera.transform.up = cameraUp;
        _camera.transform.LookAt(lookAt);
        _camera.fieldOfView = fov;
        _material.SetVector(_CameraPosition_ID, cameraPosition);
        _material.SetVector(_CameraLookAt_ID, lookAt);
        _material.SetVector(_CameraUp_ID, cameraUp);
        _material.SetFloat(_epsilon_ID, SyncUp.GetVal("Epsilon"));
        _material.SetFloat(_FOV_ID, fov * 2.0f);
        _material.SetFloat(_Distance_ID, SyncUp.GetVal("Distance"));
        _material.SetFloat(_LensCoeff_ID, SyncUp.GetVal("LensCoeff"));
        _material.SetFloat(_MaxCoC_ID, SyncUp.GetVal("MaxCoC"));
        _material.SetFloat(_RcpMaxCoC_ID, SyncUp.GetVal("RcpMaxCoC"));
        _material.SetFloat(_MarchMinimum_ID, SyncUp.GetVal("MarchMinimum"));
        _material.SetFloat(_FarPlane_ID, SyncUp.GetVal("FarPlane"));
        _material.SetFloat(_Step_ID, SyncUp.GetVal("Step"));
        _material.SetFloat(_fogDensity_ID, SyncUp.GetVal("fogDensity"));
        _material.SetFloat(_TextId_ID, SyncUp.GetVal("TextId"));
        _material.SetVector(_iMouse_ID, new Vector4(SyncUp.GetVal("iMouseX"), SyncUp.GetVal("iMouseY"), 0f, 0f));
        _material.SetFloat(_Environment_ID, SyncUp.GetVal("Environment"));
        _material.SetFloat(_StepIncreaseByDistance_ID, SyncUp.GetVal("StepIncreaseByDistance"));
        _material.SetFloat(_StepIncreaseMax_ID, SyncUp.GetVal("StepIncreaseMax"));

        if (RM_Objects != null && RM_Objects.Count > 0)
        {
            _material.SetFloatArray(_Objects_ID, RM_Objects);
        }

        if (_material != null)
        {
            Graphics.Blit(src, dest, _material);
        }

        var objMax = SyncUp.GetVal("ObjMax");
        var objMin = SyncUp.GetVal("ObjMin");
        _material.SetFloat("OBJMAX", objMax);
        _material.SetFloat("OBJMIN", objMin);
    }
}