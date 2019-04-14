using System;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

[Serializable]
public class RMO
{
    public string name;
    public string sdf;
    public float x;
    public float y;
    public float z;
    public float scalex;
    public float scaley;
    public float scalez;
    public float param0;
    public float param1;
    public int paramBoolean;
    public string boolean;
    public GameObject ToGameObject()
    {
        GameObject go = GameObject.CreatePrimitive(PrimitiveType.Sphere);
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
        return go;
    }
}
