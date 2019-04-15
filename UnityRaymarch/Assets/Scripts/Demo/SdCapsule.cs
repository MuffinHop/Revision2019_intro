using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using static RocketNet.Track;

[ExecuteInEditMode, System.Serializable]
public class SdCapsule : RM_Object
{
    protected Vector3 previousPosition0 = new Vector3(-1000000f, -1000000f, -100000f);
    protected Vector3 previousPosition1 = new Vector3(-1000000f, -1000000f, -100000f);

    int prevFrame = -1;
    protected new void LateUpdate()
    {
        while (transform.childCount == 0)
        {
            GameObject go = new GameObject("Child");
        }

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

        Rotatable = false;
        if (Time.frameCount % 6 == 0 && prevFrame != Time.frameCount)
        {
            transform.position = new Vector3(
                Mathf.Floor(transform.position.x * 64.0f) / 64.0f,
                Mathf.Floor(transform.position.y * 64.0f) / 64.0f,
                Mathf.Floor(transform.position.z * 64.0f) / 64.0f
                );
            prevFrame = Time.frameCount;
            if (Mathf.Abs(previousPosition.x - transform.position.x) > 0.01f)
            {
                PositionXHistory.Add(new SyncRMObject(Time.frameCount, transform.position.x));
            }
            if (Mathf.Abs(previousPosition.y - transform.position.y) > 0.01f)
            {
                PositionYHistory.Add(new SyncRMObject(Time.frameCount, transform.position.y));
            }
            if (Mathf.Abs(previousPosition.z - transform.position.z) > 0.01f)
            {
                PositionZHistory.Add(new SyncRMObject(Time.frameCount, transform.position.z));
            }

            if (Mathf.Abs(previousScale.x - transform.GetChild(0).position.x) > 0.01f)
            {
                ScaleXHistory.Add(new SyncRMObject(Time.frameCount, transform.GetChild(0).position.x));
            }
            if (Mathf.Abs(previousScale.y - transform.GetChild(0).position.y) > 0.01f)
            {
                ScaleYHistory.Add(new SyncRMObject(Time.frameCount, transform.GetChild(0).position.y));
            }
            if (Mathf.Abs(previousScale.z - transform.GetChild(0).position.z) > 0.01f)
            {
                ScaleZHistory.Add(new SyncRMObject(Time.frameCount, transform.GetChild(0).position.z));
            }

            previousPosition = transform.position;
            previousScale = transform.GetChild(0).position;

            visibleSyncDataCount += 6;
        }
    }
}