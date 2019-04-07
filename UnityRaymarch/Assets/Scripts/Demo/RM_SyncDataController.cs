using RocketNet;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEditor;
using UnityEngine;
using static RM_Object;

public class RM_SyncDataController : MonoBehaviour
{
    Dictionary<string, List<RM_Object>> _syncData;

    private void Start()
    {
        _syncData = new Dictionary<string, List<RM_Object>>();

        //detect all gameobjects with raymarch objects
        var rM_Objects = FindObjectsOfType<RM_Object>();
        foreach (var rM_Object in rM_Objects)
        {
            var materialName = rM_Object.SurfaceComponent.name;
            if (!_syncData.ContainsKey(materialName))
            {
                _syncData.Add(materialName, new List<RM_Object>());
            }
            _syncData[materialName].Add(rM_Object);
        }

    }


    public void OnDestroy()
    {
        Generate();
    }

    void WriteString(string filename, string code)
    {
        string path = filename;

        StreamWriter writer = new StreamWriter(path, false);
        writer.WriteLine(code);
        writer.Close();
    }
    string GetSyncArray(List<SyncRMObject> History, string arrayName)
    {
        var culture = System.Globalization.CultureInfo.InvariantCulture;
        string code = "";
        code += "       vec3 " + arrayName + "[" + (History.Capacity + 2) + "] = { \n";
        SyncRMObject historyItem = History[0];
        for (int i = 0; i < History.Count; i++)
        {
            historyItem = History[i];
            code += "               { " + (int)historyItem.Row + ", " + historyItem.Value.ToString(culture) + ", 1 }, \n";
        }
        code += "               { 100000, " + historyItem.Value.ToString(culture) + ", 1 }, \n";
        code += "               { 200000, " + historyItem.Value.ToString(culture) + ", 1 } \n";
        code += "              }; \n \n";
        return code;
    }
    public void Generate()
    {
        var culture = System.Globalization.CultureInfo.InvariantCulture;

        int arrayLength = 0;
        var syncArrayStr = "";
        foreach (KeyValuePair<string, List<RM_Object>> item in _syncData)
        {
            var rmObjects = item.Value;
            foreach (var rmObject in rmObjects)
            {
                syncArrayStr += GetSyncArray(rmObject.PositionXHistory, "ID" + rmObject.ID + "ArrayPositionX");
                syncArrayStr += GetSyncArray(rmObject.PositionYHistory, "ID" + rmObject.ID + "ArrayPositionY");
                syncArrayStr += GetSyncArray(rmObject.PositionZHistory, "ID" + rmObject.ID + "ArrayPositionZ");
                syncArrayStr += GetSyncArray(rmObject.ScaleXHistory, "ID" + rmObject.ID + "ArrayScaleX");
                syncArrayStr += GetSyncArray(rmObject.ScaleYHistory, "ID" + rmObject.ID + "ArrayScaleY");
                syncArrayStr += GetSyncArray(rmObject.ScaleZHistory, "ID" + rmObject.ID + "ArrayScaleZ");
                syncArrayStr += GetSyncArray(rmObject.RotationXHistory, "ID" + rmObject.ID + "ArrayRotationX");
                syncArrayStr += GetSyncArray(rmObject.RotationYHistory, "ID" + rmObject.ID + "ArrayRotationY");
                syncArrayStr += GetSyncArray(rmObject.RotationZHistory, "ID" + rmObject.ID + "ArrayRotationZ");
                syncArrayStr += GetSyncArray(rmObject.RotationWHistory, "ID" + rmObject.ID + "ArrayRotationW");
                arrayLength += 10;
            }
        }

        string syncCode = "         #include <math.h>\n         float RM_Objects[" + arrayLength + "];";
        syncCode += @"
            struct vec3 {
                    float x, y, z;
                };
";
        syncCode += syncArrayStr;


        foreach (var item in SyncUp.RocketParameterNames)
        {
            List<Track.Key> itemKey = SyncUp.Device.GetTrack(item).Keys();
            if (itemKey.Count > 0)
            {
                syncCode += "       vec3 " + item + "Array[" + (itemKey.Count + 2) + "] = { \n";

                Track.Key syncval = itemKey[0];
                for (int i = 0; i < itemKey.Count; i++)
                {
                    syncval = itemKey[i];
                    syncCode += "               { " + (int)syncval.row + ", " + syncval.value.ToString(culture) + ", " + (int)syncval.type + " }, \n";
                }
                syncCode += "               {100000.0f, " + syncval.value.ToString(culture) + ", " + (int)syncval.type + " }, \n";
                syncCode += "               {2100000.0f, " + syncval.value.ToString(culture) + ", " + (int)syncval.type + " } \n";
                syncCode += "              }; \n";
            }
            syncCode += "       float " + item + ";\n";
        }


        syncCode += @"
#define row x
#define value y
#define type z
float rType(int r, float t) {
	switch (r) {
	case 0:
		t = 0.;
		break;
	case 2:
		t = t * t * (3. - 2. * t);
		break;
	case 3:
		t = pow(t, 2.);
		break;
    default:
        return t;
	}
	return t;
}
float setVal(vec3 arr[], float rrow, long size, long *R_INDX) {
    if(*R_INDX>0) {
        *R_INDX--;
    }
	float t = 0;
	float renVal = 0;
	vec3 RET = { 0,0,0 };
	vec3 NEXT = { 0,0,0 };
	for (long i = *R_INDX; i < size - 1; i++) {
		if (rrow <= arr[i].row) {
            if(i>0) {
			    *R_INDX = i - 1;
            }
			break;
		}
	}
	RET = arr[*R_INDX];
	NEXT = arr[*R_INDX + 1];
	t = (rrow - RET.row) / (NEXT.row - RET.row);
    t = rType(RET.type, t);
	renVal = RET.value + (NEXT.value - RET.value) * t;
	return renVal;
}

";

        string arrayCode = "";
        foreach (KeyValuePair<string, List<RM_Object>> item in _syncData)
        {
            var rmObjects = item.Value;
            foreach (var rmObject in rmObjects)
            {
                int index = rmObject.ID * 10;
                syncCode += "long pointerID" + rmObject.ID + "ArrayPositionX = 0,";
                syncCode += "pointerID" + rmObject.ID + "ArrayPositionY = 0,";
                syncCode += "pointerID" + rmObject.ID + "ArrayPositionZ = 0,";
                syncCode += "pointerID" + rmObject.ID + "ArrayScaleX = 0,";
                syncCode += "pointerID" + rmObject.ID + "ArrayScaleY = 0,";
                syncCode += "pointerID" + rmObject.ID + "ArrayScaleZ = 0,";
                syncCode += "pointerID" + rmObject.ID + "ArrayRotationX = 0,";
                syncCode += "pointerID" + rmObject.ID + "ArrayRotationY = 0,";
                syncCode += "pointerID" + rmObject.ID + "ArrayRotationZ = 0,";
                syncCode += "pointerID" + rmObject.ID + "ArrayRotationW = 0; \n";
                arrayCode += "       RM_Objects[" + index + "] = setVal(ID" + rmObject.ID + "ArrayPositionX, row, " + (rmObject.PositionXHistory.Count+2) + ", &pointerID" + rmObject.ID + "ArrayPositionX ); \n";
                index++;
                arrayCode += "       RM_Objects[" + index + "] = setVal(ID" + rmObject.ID + "ArrayPositionY, row, " + (rmObject.PositionXHistory.Count + 2) + ", &pointerID" + rmObject.ID + "ArrayPositionY ); \n";
                index++;
                arrayCode += "       RM_Objects[" + index + "] = setVal(ID" + rmObject.ID + "ArrayPositionZ, row, " + (rmObject.PositionXHistory.Count + 2) + ", &pointerID" + rmObject.ID + "ArrayPositionZ ); \n";
                index++;
                arrayCode += "       RM_Objects[" + index + "] = setVal(ID" + rmObject.ID + "ArrayScaleX, row, " + (rmObject.PositionXHistory.Count + 2) + ", &pointerID" + rmObject.ID + "ArrayScaleX ) / 2.0f; \n";
                index++;
                arrayCode += "       RM_Objects[" + index + "] = setVal(ID" + rmObject.ID + "ArrayScaleY, row, " + (rmObject.PositionXHistory.Count + 2) + ", &pointerID" + rmObject.ID + "ArrayScaleY ) / 2.0f; \n";
                index++;
                arrayCode += "       RM_Objects[" + index + "] = setVal(ID" + rmObject.ID + "ArrayScaleZ, row, " + (rmObject.PositionXHistory.Count + 2) + ", &pointerID" + rmObject.ID + "ArrayScaleZ ) / 2.0f; \n";
                index++;
                arrayCode += "       RM_Objects[" + index + "] = setVal(ID" + rmObject.ID + "ArrayRotationX, row, " + (rmObject.PositionXHistory.Count + 2) + ", &pointerID" + rmObject.ID + "ArrayRotationX ); \n";
                index++;
                arrayCode += "       RM_Objects[" + index + "] = setVal(ID" + rmObject.ID + "ArrayRotationY, row, " + (rmObject.PositionXHistory.Count + 2) + ", &pointerID" + rmObject.ID + "ArrayRotationY ); \n";
                index++;
                arrayCode += "       RM_Objects[" + index + "] = setVal(ID" + rmObject.ID + "ArrayRotationZ, row, " + (rmObject.PositionXHistory.Count + 2) + ", &pointerID" + rmObject.ID + "ArrayRotationZ ); \n";
                index++;
                arrayCode += "       RM_Objects[" + index + "] = setVal(ID" + rmObject.ID + "ArrayRotationW, row, " + (rmObject.PositionXHistory.Count + 2) + ", &pointerID" + rmObject.ID + "ArrayRotationW ); \n";
                index++;
            }
        }
        string regularSyncCode = "";
        foreach (var item in SyncUp.RocketParameterNames)
        {
            List<Track.Key> itemKey = SyncUp.Device.GetTrack(item).Keys();
            if (itemKey.Count > 0)
            {
                syncCode += "long " + item + "ArrayPointer = 0;\n";
                regularSyncCode += "       " + item + " = setVal(" + item + "Array, row, " + (_syncData.Count+2) + ", &" + item + "ArrayPointer); \n";
            }
        }
        syncCode += @"
        void Sync(float second)
        {
            float div = 4.0 * 60.0 / 120.0;
            float row = second * 60.0;

            ";
        syncCode += arrayCode;
        syncCode += "       row = second * div;\n";
        syncCode += regularSyncCode;

        syncCode += @"
    }";
        WriteString("../IntroFramework/src/sync.c", syncCode);
    }
}