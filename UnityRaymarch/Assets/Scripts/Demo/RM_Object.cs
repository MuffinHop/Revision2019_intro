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

    
    public ShaderComponent ShaderComponent;
    public RM_Surface SurfaceComponent;
    public Mix Mixer;
    public float R, N;
    private Key.Type interpolation = Key.Type.Linear;

    private ShaderComponent compShaderComponent;
    private RM_Surface compSurfaceComponent;
    private Vector3 previousPosition = new Vector3(-1000000f,-1000000f,-100000f);
    private Vector3 previousScale = new Vector3(0f, 0f, 0f);
    private Vector4 previousRotation = new Vector4(-1000000f, -1000000f, -100000f, -100000f);
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
    private static int syncDataCount;
    [SerializeField]
    private int visibleSyncDataCount;

    public void SetID(int i)
    {
        ID = i;
    }
    int prevFrame = -1;
    private void Update()
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

        if (Time.frameCount % 4 == 0 && prevFrame != Time.frameCount )
        {
            prevFrame = Time.frameCount ;
            if (previousPosition != transform.position)
            {
                PositionXHistory.Add(new SyncRMObject(Time.frameCount, transform.position.x));
                PositionYHistory.Add(new SyncRMObject(Time.frameCount, transform.position.y));
                PositionZHistory.Add(new SyncRMObject(Time.frameCount, transform.position.z));
            }

            if (previousScale != transform.localScale)
            {
                ScaleXHistory.Add(new SyncRMObject(Time.frameCount, transform.localScale.x));
                ScaleYHistory.Add(new SyncRMObject(Time.frameCount, transform.localScale.y));
                ScaleZHistory.Add(new SyncRMObject(Time.frameCount, transform.localScale.z));
            }

            if (previousRotation != new Vector4(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w))
            {
                RotationXHistory.Add(new SyncRMObject(Time.frameCount, transform.rotation.x));
                RotationYHistory.Add(new SyncRMObject(Time.frameCount, transform.rotation.y));
                RotationZHistory.Add(new SyncRMObject(Time.frameCount, transform.rotation.z));
                RotationWHistory.Add(new SyncRMObject(Time.frameCount, transform.rotation.w));
            }

            previousPosition = transform.position;
            previousScale = transform.localScale;
            previousRotation = new Vector4(transform.rotation.x,transform.rotation.y,transform.rotation.z,transform.rotation.w);

            visibleSyncDataCount += 10;
        }
    }
}