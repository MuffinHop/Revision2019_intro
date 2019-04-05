using RocketNet;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEditor;
using UnityEngine;

public class RM_SyncDataController : MonoBehaviour
{
    Dictionary<string, List<RM_SyncData>> _syncData;

    private void Start()
    {
        _syncData = new Dictionary<string, List<RM_SyncData>>();
    }

    public void AddData(string name, float data)
    {
        if (!Application.isPlaying) return;
        if (_syncData.ContainsKey(name) == false)
        {
            _syncData.Add(name, new List<RM_SyncData>());
        }
        var syncData = new RM_SyncData(RocketNet.Track.Key.Type.Linear, Time.frameCount / 10, data);
        _syncData[name].Add(syncData);
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

    public void Generate()
    {
        var culture = System.Globalization.CultureInfo.InvariantCulture;
        string syncCode = "         #include <math.h>\n         float RM_Objects[" + _syncData.Count + "];";
        syncCode += @"
            struct vec3 {
                    float x, y, z;
                };
";
        foreach (KeyValuePair<string, List<RM_SyncData>> item in _syncData)
        {
            syncCode += "       vec3 " + item.Key + "[" + item.Value.Count + "] = { \n";
            for (int i = 0; i < item.Value.Count; i++)
            {
                var syncval = item.Value[i];
                if (i == item.Value.Count - 1)
                {
                    syncCode += "               { " + (int)syncval.Row + ", " + syncval.Value.ToString(culture) + ", " + (int)syncval.Interpolation + " } \n";
                }
                else
                {
                    syncCode += "               { " + (int)syncval.Row + ", " + syncval.Value.ToString(culture) + ", " + (int)syncval.Interpolation + " }, \n";
                }
            }
            syncCode += "              }; \n \n";
        }
        foreach (var item in SyncUp.RocketParameterNames)
        {
            List<Track.Key> itemKey = SyncUp.Device.GetTrack(item).Keys();
            if (itemKey.Count > 0)
            {
                syncCode += "       vec3 " + item + "Array[" + itemKey.Count + "] = { \n";

                for (int i = 0; i < itemKey.Count; i++)
                {
                    var syncval = itemKey[i];
                    if (i == itemKey.Count - 1)
                    {
                        syncCode += "               {" + (int)syncval.row + ", " + syncval.value.ToString(culture) + ", " + (int)syncval.type + " } \n";
                    }
                    else
                    {
                        syncCode += "               { " + (int)syncval.row + ", " + syncval.value.ToString(culture) + ", " + (int)syncval.type + " }, \n";
                    }
                }
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
float setVal(vec3 arr[], float rrow) {
	int R_INDX = 0;
	int size = 0;
	float t = 0;
	float renVal = 0;
	vec3 RET = { 0,0,0 };
	vec3 NEXT = { 0,0,0 };
	size = sizeof(arr) / sizeof(arr[0]);
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
	    float row = second * div;
";
        int index = 0;
        foreach (KeyValuePair<string, List<RM_SyncData>> item in _syncData)
        {
            syncCode += "       RM_Objects[" + index + "] = setVal(" + item.Key + ", row ); \n";
            index++;
        }

        foreach (var item in SyncUp.RocketParameterNames)
        {
            List<Track.Key> itemKey = SyncUp.Device.GetTrack(item).Keys();
            if(itemKey.Count>0) {
                syncCode += "       " + item + " = setVal(" + item + "Array, row); \n";
            }
        }

        syncCode += @"
    }";
        WriteString("../IntroFramework/src/sync.c", syncCode);
    }
}