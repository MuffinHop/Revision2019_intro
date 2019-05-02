using System;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

[Serializable]
public class RMOData
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
                var rm_object = go.AddComponent<SDFObject>();
                go.transform.localScale = new Vector3(r, h, 0.1f);
                break;
            case "Cube":
                go = GameObject.CreatePrimitive(PrimitiveType.Cube);
                go.transform.position = new Vector3(location[0], location[1], location[2]);
                var rm_object2 = go.AddComponent<SDFObject>();
                if (c != null)
                    go.transform.localScale = new Vector3(c[0],c[1],c[2]);
                else
                    go.transform.localScale = new Vector3(0.1f, 0.1f, 0.1f);
                break;
            case "Capsule":
                var go2 = GameObject.CreatePrimitive(PrimitiveType.Capsule);
                go = GameObject.CreatePrimitive(PrimitiveType.Capsule);
                go.transform.position = new Vector3(location[0], location[1], location[2]);
                var rm_object3 = go.AddComponent<SDFDualPartObject>();
                go2.transform.position = new Vector3(b[0], b[1], b[2]);
                go2.transform.parent = go.transform;
                go.transform.localScale = new Vector3(0.1f, 0.1f, 0.1f);
                go2.transform.localScale = new Vector3(0.1f, 0.1f, 0.1f);
                rm_object3.R = r;
                break;
            case "Sphere":
                go = GameObject.CreatePrimitive(PrimitiveType.Sphere);
                go.transform.position = new Vector3(location[0], location[1], location[2]);
                go.transform.localScale = new Vector3(r,r,r);
                var rm_object4 = go.AddComponent<SDFObject>();
                break;
            default:
                Debug.LogWarning("Missing sdf:" + sdf + ":" + name + ":" + obj);
                go = GameObject.CreatePrimitive(PrimitiveType.Sphere);
                go.transform.position = new Vector3(location[0], location[1], location[2]);
                go.transform.localScale = new Vector3(0.1f, 0.1f, 0.1f);
                var rm_object5 = go.AddComponent<SDFObject>();
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
