using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExecuteInEditMode]
public class RM_Object : MonoBehaviour
{
    private RM_Type type;

    internal RM_Type Type { get => type; set => type = value; }

    public void PassToMaterial(Material material)
    {

    }
    private void LateUpdate()
    {
        var position = transform.position;
        var rotation = transform.rotation.eulerAngles;
        var scale = transform.localScale;
        //10
        RM_Camera.RM_Objects.Add((float)Type);
        RM_Camera.RM_Objects.Add(position.x);
        RM_Camera.RM_Objects.Add(position.y);
        RM_Camera.RM_Objects.Add(position.z);
        RM_Camera.RM_Objects.Add(rotation.x);
        RM_Camera.RM_Objects.Add(rotation.y);
        RM_Camera.RM_Objects.Add(rotation.z);
        RM_Camera.RM_Objects.Add(scale.x);
        RM_Camera.RM_Objects.Add(scale.y);
        RM_Camera.RM_Objects.Add(scale.z);
    }
}
