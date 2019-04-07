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
        code += "       vec3 " + arrayName + "[" + (History.Capacity + 1) + "] = { \n";
        for (int i = 0; i < History.Count; i++)
        {
            var historyItem = History[i];
            code += "               { " + (int)historyItem.Row + ", " + historyItem.Value.ToString(culture) + ", 1 }, \n";
        }
        code += "               { 100000, 100000, 1 } \n";
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
            foreach (var rmObject in rmObjects) {
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
                syncCode += "       vec3 " + item + "Array[" + (itemKey.Count + 1) + "] = { \n";
                
                for (int i = 0; i < itemKey.Count; i++)
                {
                    var syncval = itemKey[i];
                    syncCode += "               { " + (int)syncval.row + ", " + syncval.value.ToString(culture) + ", " + (int)syncval.type + " }, \n";
                }
                syncCode += "               {100000.0f, 0.0f, 0.0f } \n";
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
float setVal(vec3 arr[], float rrow, int size) {
	int R_INDX = 0;
	float t = 0;
	float renVal = 0;
	vec3 RET = { 0,0,0 };
	vec3 NEXT = { 0,0,0 };
	for (int i = 0; i < size; i++) {
		if (rrow <= arr[i].row) {
			R_INDX = i - 1;
			break;
		}
	}
	RET = arr[R_INDX];
	NEXT = arr[R_INDX + 1];
	t = (rrow - RET.row) / (NEXT.row - RET.row);
	renVal = RET.value + (NEXT.value - RET.value) * rType(RET.type, t);
	return renVal;
}
    void Sync( float second)
    {
	    float div = 4.0 * 60.0 / 120.0;
	    float row = second * 60.0;

";
        foreach (KeyValuePair<string, List<RM_Object>> item in _syncData)
        {
            var rmObjects = item.Value;
            foreach (var rmObject in rmObjects)
            {
                int index = rmObject.ID * 10;
                syncCode += "       RM_Objects[" + index + "] = setVal(ID" + rmObject.ID + "ArrayPositionX, row, " + rmObject.PositionXHistory.Count + " ); \n";
                index++;
                syncCode += "       RM_Objects[" + index + "] = setVal(ID" + rmObject.ID + "ArrayPositionY, row, " + rmObject.PositionYHistory.Count + " ); \n";
                index++;
                syncCode += "       RM_Objects[" + index + "] = setVal(ID" + rmObject.ID + "ArrayPositionZ, row, " + rmObject.PositionZHistory.Count + " ); \n";
                index++;
                syncCode += "       RM_Objects[" + index + "] = setVal(ID" + rmObject.ID + "ArrayScaleX, row, " + rmObject.ScaleXHistory.Count + " ) / 2.0f; \n";
                index++;
                syncCode += "       RM_Objects[" + index + "] = setVal(ID" + rmObject.ID + "ArrayScaleY, row, " + rmObject.ScaleYHistory.Count + " ) / 2.0f; \n";
                index++;
                syncCode += "       RM_Objects[" + index + "] = setVal(ID" + rmObject.ID + "ArrayScaleZ, row, " + rmObject.ScaleZHistory.Count + " ) / 2.0f; \n";
                index++;
                syncCode += "       RM_Objects[" + index + "] = setVal(ID" + rmObject.ID + "ArrayRotationX, row, " + rmObject.RotationXHistory.Count + " ); \n";
                index++;
                syncCode += "       RM_Objects[" + index + "] = setVal(ID" + rmObject.ID + "ArrayRotationY, row, " + rmObject.RotationYHistory.Count + " ); \n";
                index++;
                syncCode += "       RM_Objects[" + index + "] = setVal(ID" + rmObject.ID + "ArrayRotationZ, row, " + rmObject.RotationZHistory.Count + " ); \n";
                index++;
                syncCode += "       RM_Objects[" + index + "] = setVal(ID" + rmObject.ID + "ArrayRotationW, row, " + rmObject.RotationWHistory.Count + " ); \n";
                index++;
            }
        }

        syncCode += "       row = second * div;\n";
        foreach (var item in SyncUp.RocketParameterNames)
        {
            List<Track.Key> itemKey = SyncUp.Device.GetTrack(item).Keys();
            if (itemKey.Count > 0)
            {
                syncCode += "       " + item + " = setVal(" + item + "Array, row, " + _syncData.Count + " ); \n";
            }
        }

        syncCode += @"
    }";
        WriteString("../IntroFramework/src/sync.c", syncCode);
    }
}