using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using static RocketNet.Track;

[ExecuteInEditMode, System.Serializable]
public class RM_Object : MonoBehaviour
{
    public enum Mix
    {
        Min,                    // box, sphere
        Max,                    // box, sphere
        MaxMinus,               // box, sphere
        OpUnionRound,           // box, sphere, r
        OpIntersectionRound,    // box, sphere, r
        OpDifferenceRound,      // box, sphere, r
        OpUnionChamfer,         // box, sphere, r
        OpIntersectionChamfer,  // box, sphere, r
        OpDifferenceChamfer,    // box, sphere, r
        OpUnionColumns,         // box, sphere, r, n
        OpIntersectionColumns,  // box, sphere, r, n
        OpDifferenceColumns,    // box, sphere, r, n
        OpUnionStairs,          // box, sphere, r, n
        OpIntersectionStairs,   // box, sphere, r, n
        OpDifferenceStairs,     // box, sphere, r, n
        OpPipe,                 // box, sphere, r*0.3
        OpEngrave,              // box, sphere, r*0.3
        OpGroove,               // box, sphere ,r*0.3, r*0.3
        OpTongue,               // box, sphere, r*0.3, r*0.3
        OpUnionSoft             // box, sphere, r*0.3, r*0.3
    };
    [SerializeField]
    private RM_Type type;
    
    public ShaderComponent ShaderComponent;
    public RM_Material MaterialComponent;
    public Mix Mixer;
    public float R, N;
    private Key.Type interpolation = Key.Type.Linear;

    private ShaderComponent compShaderComponent;
    private RM_Material compMaterialComponent;
    private Vector3 previousPosition;
    private Vector3 previousScale;
    private Vector4 previousRotation;
    private int id;
    [SerializeField]
    private RM_SyncDataController syncDataController;
    private static int syncDataCount;
    [SerializeField]
    private int visibleSyncDataCount;
    private void Start()
    {
        syncDataController = FindObjectOfType<RM_SyncDataController>();
    }
    public void SetID(int i)
    {
        id = i;
    }
    private void Update()
    {
        if (Time.frameCount % 10 == 0)
        {
            //position
            if (Mathf.Abs(previousPosition.x - transform.position.x) > 0.01f)
            {
                syncDataController.AddData("rm"+ id+"_position_x", transform.position.x);
                previousPosition.x = transform.position.x;
                syncDataCount++;
            }
            if (Mathf.Abs(previousPosition.y - transform.position.y) > 0.01f)
            {
                syncDataController.AddData("rm" + id + "_position_y", transform.position.y);
                previousPosition.y = transform.position.y;
                syncDataCount++;
            }
            if (Mathf.Abs(previousPosition.z - transform.position.z) > 0.01f)
            {
                syncDataController.AddData("rm" + id + "_position_z", transform.position.z);
                previousPosition.z = transform.position.z;
                syncDataCount++;
            }

            //scale
            if (Mathf.Abs(previousScale.x - transform.localScale.x) > 0.01f)
            {
                syncDataController.AddData("rm" + id + "_scale_x", transform.localScale.x);
                previousScale.x = transform.localScale.x;
                syncDataCount++;
            }
            if (Mathf.Abs(previousScale.y - transform.localScale.y) > 0.01f)
            {
                syncDataController.AddData("rm" + id + "_scale_y", transform.localScale.y);
                previousScale.y = transform.localScale.y;
                syncDataCount++;
            }
            if (Mathf.Abs(previousScale.z - transform.localScale.z) > 0.01f)
            {
                syncDataController.AddData("rm" + id + "_scale_z", transform.localScale.z);
                previousScale.z = transform.localScale.z;
                syncDataCount++;
            }

            //rotation
            if (Mathf.Abs(previousRotation.x - transform.rotation.x) > 0.01f)
            {
                syncDataController.AddData("rm" + id + "_rotation_x", transform.rotation.x);
                previousRotation.x = transform.rotation.x;
                syncDataCount++;
            }
            if (Mathf.Abs(previousRotation.y - transform.rotation.y) > 0.01f)
            {
                syncDataController.AddData("rm" + id + "_rotation_y", transform.rotation.y);
                previousRotation.y = transform.rotation.y;
                syncDataCount++;
            }
            if (Mathf.Abs(previousRotation.z - transform.rotation.z) > 0.01f)
            {
                syncDataController.AddData("rm" + id + "_rotation_z", transform.rotation.z);
                previousRotation.z = transform.rotation.z;
                syncDataCount++;
            }
            if (Mathf.Abs(previousRotation.z - transform.rotation.z) > 0.01f)
            {
                syncDataController.AddData("rm" + id + "_rotation_w", transform.rotation.w);
                previousRotation.w = transform.rotation.w;
                syncDataCount++;
            }

            visibleSyncDataCount = syncDataCount;
        }
    }
}