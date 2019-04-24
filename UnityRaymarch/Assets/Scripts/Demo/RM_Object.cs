using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using static RocketNet.Track;

[System.Serializable]
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

    
    public ShaderComponent ShaderComponent;
    public RM_Surface SurfaceComponent;
    public Mix Mixer;
    [Range(0f,2f)]
    public float R, N;
    public bool Centered;
    public bool Rotatable;
    protected Key.Type interpolation = Key.Type.Linear;

    protected ShaderComponent compShaderComponent;
    protected RM_Surface compSurfaceComponent;
    protected Vector3 previousPosition = new Vector3(-1000000f,-1000000f,-100000f);
    protected Vector3 previousScale = new Vector3(0f, 0f, 0f);
    protected Vector4 previousRotation = new Vector4(-1000000f, -1000000f, -100000f, -100000f);
    public class SyncRMObject
    {
        public int Row;
        public float Value;
        public SyncRMObject(int row, float value)
        {
            Row = row;
            Value = value;
        }
    }
    public List<SyncRMObject> PositionXHistory;
    public List<SyncRMObject> PositionYHistory;
    public List<SyncRMObject> PositionZHistory;
    public List<SyncRMObject> ScaleXHistory;
    public List<SyncRMObject> ScaleYHistory;
    public List<SyncRMObject> ScaleZHistory;
    public List<SyncRMObject> RotationXHistory;
    public List<SyncRMObject> RotationYHistory;
    public List<SyncRMObject> RotationZHistory;
    public List<SyncRMObject> RotationWHistory;
    public int ID;
    protected static int syncDataCount;
    [SerializeField]
    protected int visibleSyncDataCount;

    public void SetID(int i)
    {
        ID = i;
    }
    protected int prevFrame = -1;
    protected void LateUpdate()
    {
        if (PositionXHistory == null)
        {
            PositionXHistory = new List<SyncRMObject>();
        }
        if (PositionYHistory == null)
        {
            PositionYHistory = new List<SyncRMObject>();
        }
        if (PositionZHistory == null)
        {
            PositionZHistory = new List<SyncRMObject>();
        }
        if (ScaleXHistory == null)
        {
            ScaleXHistory = new List<SyncRMObject>();
        }
        if (ScaleYHistory == null)
        {
            ScaleYHistory = new List<SyncRMObject>();
        }
        if (ScaleZHistory == null)
        {
            ScaleZHistory = new List<SyncRMObject>();
        }
        if (RotationXHistory == null)
        {
            RotationXHistory = new List<SyncRMObject>();
        }
        if (RotationYHistory == null)
        {
            RotationYHistory = new List<SyncRMObject>();
        }
        if (RotationZHistory == null)
        {
            RotationZHistory = new List<SyncRMObject>();
        }
        if (RotationWHistory == null)
        {
            RotationWHistory = new List<SyncRMObject>();
        }

        if (Time.frameCount % 12 == 0 && prevFrame != Time.frameCount )
        {
            prevFrame = Time.frameCount ;
            transform.position = new Vector3(
                Mathf.Floor(transform.position.x * 64.0f) / 64.0f,
                Mathf.Floor(transform.position.y * 64.0f) / 64.0f,
                Mathf.Floor(transform.position.z * 64.0f) / 64.0f
                );
            if (Mathf.Abs(previousPosition.x-transform.position.x) > 0.1f)
            {
                PositionXHistory.Add(new SyncRMObject((int)(SyncUp.time * 60.0f), transform.position.x));
            }
            if (Mathf.Abs(previousPosition.y - transform.position.y) > 0.1f)
            {
                PositionYHistory.Add(new SyncRMObject((int)(SyncUp.time * 60.0f), transform.position.y));
            }
            if (Mathf.Abs(previousPosition.z - transform.position.z) > 0.1f)
            {
                PositionZHistory.Add(new SyncRMObject((int)(SyncUp.time * 60.0f), transform.position.z));
            }
            var temp = transform.parent;
            transform.parent = null;
            if (previousScale != transform.localScale)
            {
                ScaleXHistory.Add(new SyncRMObject((int)(SyncUp.time * 60.0f), transform.localScale.x));
                ScaleYHistory.Add(new SyncRMObject((int)(SyncUp.time * 60.0f), transform.localScale.y));
                ScaleZHistory.Add(new SyncRMObject((int)(SyncUp.time * 60.0f), transform.localScale.z));
            }

            if (Mathf.Abs(previousRotation.x - transform.rotation.x) > 0.1f)
            {
                RotationXHistory.Add(new SyncRMObject(Time.frameCount, transform.rotation.x));
            }
            if (Mathf.Abs(previousRotation.y - transform.rotation.y) > 0.1f)
            {
                RotationYHistory.Add(new SyncRMObject(Time.frameCount, transform.rotation.y));
            }
            if (Mathf.Abs(previousRotation.z - transform.rotation.z) > 0.1f)
            {
                RotationZHistory.Add(new SyncRMObject(Time.frameCount, transform.rotation.z));
            }
            if (Mathf.Abs(previousRotation.w - transform.rotation.w) > 0.1f)
            {
                RotationWHistory.Add(new SyncRMObject(Time.frameCount, transform.rotation.w));
            }

            previousPosition = transform.position;
            previousScale = transform.localScale;
            previousRotation = new Vector4(transform.rotation.x,transform.rotation.y,transform.rotation.z,transform.rotation.w);

            transform.parent = temp;
            visibleSyncDataCount += 10;
        }
    }

    public void OnValidate() {
        RayMarchingController rm = GameObject.Find("Main Camera").GetComponent<RayMarchingController>();
        if (rm.GeneratedOnce) rm.RegenerateShader();
    }
}