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
    private Vector3 previousPosition;
    private Vector3 previousScale;
    private Vector4 previousRotation;
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
    private void Start()
    {
        PositionXHistory = new List<SyncRMObject>();
        PositionYHistory = new List<SyncRMObject>();
        PositionZHistory = new List<SyncRMObject>();
        ScaleXHistory = new List<SyncRMObject>();
        ScaleYHistory = new List<SyncRMObject>();
        ScaleZHistory = new List<SyncRMObject>();
        RotationXHistory = new List<SyncRMObject>();
        RotationYHistory = new List<SyncRMObject>();
        RotationZHistory = new List<SyncRMObject>();
        RotationWHistory = new List<SyncRMObject>();
    }
    public void SetID(int i)
    {
        ID = i;
    }
    int prevFrame = -1;
    private void Update()
    {
        if (Time.frameCount % 10 == 0 && prevFrame != Time.frameCount )
        {
            prevFrame = Time.frameCount ;
            PositionXHistory.Add(new SyncRMObject(Time.frameCount, transform.position.x));
            PositionYHistory.Add(new SyncRMObject(Time.frameCount, transform.position.y));
            PositionZHistory.Add(new SyncRMObject(Time.frameCount, transform.position.z));

            ScaleXHistory.Add(new SyncRMObject(Time.frameCount, transform.localScale.x));
            ScaleYHistory.Add(new SyncRMObject(Time.frameCount, transform.localScale.y));
            ScaleZHistory.Add(new SyncRMObject(Time.frameCount, transform.localScale.z));

            RotationXHistory.Add(new SyncRMObject(Time.frameCount, transform.rotation.x));
            RotationYHistory.Add(new SyncRMObject(Time.frameCount, transform.rotation.y));
            RotationZHistory.Add(new SyncRMObject(Time.frameCount, transform.rotation.z));
            RotationWHistory.Add(new SyncRMObject(Time.frameCount, transform.rotation.w));

            visibleSyncDataCount += 10;
        }
    }
}