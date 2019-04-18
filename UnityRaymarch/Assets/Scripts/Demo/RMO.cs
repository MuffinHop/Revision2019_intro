using System;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

[Serializable]
public class RMO
{
    public float r;
    public float h;
    public string obj;
    public string sdf;
    public string name;
    public float[] location;
    public float[] a;
    public float[] b;
    public float[] c;

    public GameObject ToGameObject()
    {
        GameObject go = null;
        switch(sdf)
        {
            case "Cylinder":
                go = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
                go.transform.position = new Vector3(location[0], location[1], location[2]);
                break;
            case "Cube":
                go = GameObject.CreatePrimitive(PrimitiveType.Cube);
                go.transform.position = new Vector3(location[0], location[1], location[2]);
                break;
            case "Capsule":
                go = GameObject.CreatePrimitive(PrimitiveType.Capsule);
                go.transform.position = new Vector3(location[0], location[1], location[2]);
                break;
            case "Sphere":
                go = GameObject.CreatePrimitive(PrimitiveType.Sphere);
                go.transform.position = new Vector3(location[0], location[1], location[2]);
                break;
            default:
                Debug.LogWarning("Missing sdf:" + sdf + ":" + name + ":" + obj);
                go = GameObject.CreatePrimitive(PrimitiveType.Sphere);
                go.transform.position = new Vector3(location[0], location[1], location[2]);
                break;
        }
        go.transform.name = name;
        return go;
        /*
        MeshRenderer mr = go.GetComponent<MeshRenderer>();
        mr.material = new Material(Shader.Find("Diffuse"));
        var rm_object = go.AddComponent<RM_Object>();
        go.name = name;
        go.transform.position = new Vector3(x, y, z);
        go.transform.localScale = new Vector3(scalex, scaley, scalez);

        if(paramBoolean==1)
        {
            rm_object.R = param0;
            if(param1>0f)
            {
                rm_object.N = param0;
            }
        } else
        {
            if (param0 > 0f)
            {
                if (param1 > 0f)
                {
                    go.transform.localScale = new Vector3(param0, param0, param0);
                } else
                {
                    go.transform.localScale = new Vector3(param0, param1, param1);
                }
            }
        }
        if(boolean != null)
        {
            switch(boolean)
            {
                case "OpDifferenceRound":
                    rm_object.Mixer = RM_Object.Mix.OpDifferenceRound;
                    break;
            }
        }
        return go;*/
    }
}
