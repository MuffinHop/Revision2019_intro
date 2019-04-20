using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using static RocketNet.Track;

[System.Serializable]
public class SdCapsule : RM_Object
{
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
        if (Time.frameCount % 12 == 0 && prevFrame != Time.frameCount)
        {
            Vector3 pos = new Vector3(
                Mathf.Floor(transform.position.x * 64.0f) / 64.0f,
                Mathf.Floor(transform.position.y * 64.0f) / 64.0f,
                Mathf.Floor(transform.position.z * 64.0f) / 64.0f
                );
            prevFrame = Time.frameCount;
            if (Mathf.Abs(previousPosition.x - pos.x) > 0.01f)
            {
                PositionXHistory.Add(new SyncRMObject((int)(SyncUp.time * 60.0f), pos.x));
            }
            if (Mathf.Abs(previousPosition.y - pos.y) > 0.01f)
            {
                PositionYHistory.Add(new SyncRMObject((int)(SyncUp.time * 60.0f), pos.y));
            }
            if (Mathf.Abs(previousPosition.z - pos.z) > 0.01f)
            {
                PositionZHistory.Add(new SyncRMObject((int)(SyncUp.time * 60.0f), pos.z));
            }

            Vector3 npos = new Vector3(
                Mathf.Floor(transform.GetChild(0).position.x * 64.0f) / 64.0f,
                Mathf.Floor(transform.GetChild(0).position.y * 64.0f) / 64.0f,
                Mathf.Floor(transform.GetChild(0).position.z * 64.0f) / 64.0f
                );
            if (Mathf.Abs(previousScale.x - npos.x) > 0.01f)
            {
                ScaleXHistory.Add(new SyncRMObject((int)(SyncUp.time * 60.0f), npos.x * 2.0f));
            }
            if (Mathf.Abs(previousScale.y - npos.y) > 0.01f)
            {
                ScaleYHistory.Add(new SyncRMObject((int)(SyncUp.time * 60.0f), npos.y * 2.0f));
            }
            if (Mathf.Abs(previousScale.z - npos.z) > 0.01f)
            {
                ScaleZHistory.Add(new SyncRMObject((int)(SyncUp.time * 60.0f), npos.z * 2.0f));
            }

            previousPosition = pos;
            previousScale = npos;

            visibleSyncDataCount += 6;
        }
    }
}