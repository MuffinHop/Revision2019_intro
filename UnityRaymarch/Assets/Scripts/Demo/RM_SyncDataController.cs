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
        string syncCode = @"
    varying float RM_Objects[108];
";
        foreach (KeyValuePair<string, List<RM_SyncData>> item in _syncData)
        {
            syncCode += "       vec3 " + item.Key + "[" + item.Value.Count + "] = vec3[" + item.Value.Count + "]( \n";
            for (int i = 0; i < item.Value.Count; i++)
            {
                var syncval = item.Value[i];
                if (i == item.Value.Count - 1)
                {
                    syncCode += "               vec3( " + (int)syncval.Row + ", " + syncval.Value.ToString(culture) + ", " + (int)syncval.Interpolation + " ) \n";
                }
                else
                {
                    syncCode += "               vec3( " + (int)syncval.Row + ", " + syncval.Value.ToString(culture) + ", " + (int)syncval.Interpolation + " ), \n";
                }
            }
            syncCode += "              ); \n";
        }
        foreach (var item in SyncUp.RocketParameterNames)
        {
            List<Track.Key> itemKey = SyncUp.Device.GetTrack(item).Keys();

            syncCode += "       vec3 " + item + "[" + itemKey.Count + "] = vec3[" + itemKey.Count + "]( \n";

            for (int i = 0; i < itemKey.Count; i++)
            {
                var syncval = itemKey[i];
                if (i == itemKey.Count - 1)
                {
                    syncCode += "               vec3( " + (int)syncval.row + ", " + syncval.value.ToString(culture) + ", " + (int)syncval.type + " ) \n";
                } else
                {
                    syncCode += "               vec3( " + (int)syncval.row + ", " + syncval.value.ToString(culture) + ", " + (int)syncval.type + " ), \n";
                }
            }
            syncCode += "              ); \n";

        }
        syncCode += @"

	int R_INDX;
	vec3 RET;
	float fRET;
#define row x
#define value y
#define type z
#define findClosest(arr,currow) { for(int i=0; i<arr.length(); i++) { if(currow<=arr[i].row) { R_INDX = i-1; RET=arr[R_INDX]; break; } } }
#define rType(r, t) { switch (int(r)) { case 0: t = 0.; break; case 2: t = t * t * (3. - 2. * t); break; case 3: t = pow(t, 2.); break; } }
#define setVal(arr,row,target) { findClosest(arr,row); float t = (row - RET.row) / (arr[R_INDX+1].row - RET.row); rType(RET.type,t); float renVal = RET.value + (arr[R_INDX+1].value - RET.value) * t; target = renVal; }
uniform float seconds;
    void main(void)
    {
	    float div = 4.0 * 60.0 / 120.0;
	    float row = seconds * div;
";
        int index = 0;
        foreach (KeyValuePair<string, List<RM_SyncData>> item in _syncData)
        {
            syncCode += "       setVal(" + item.Key + ", row, RM_Objects[" + index + "]); \n";
            index++;
        }
        syncCode += @"
       gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    }";
        WriteString("../IntroFramework/src/shaders/sync.vert", syncCode);
    }
}


