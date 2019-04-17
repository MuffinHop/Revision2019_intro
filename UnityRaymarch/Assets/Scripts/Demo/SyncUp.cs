using RocketNet;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

[ExecuteInEditMode]
public class SyncUp : MonoBehaviour
{
    static public float time;
    static int row = 0;
    static float frow = 0f;
    static public Device Device;
    static public float BPMs;
    public float BPM;
    public AudioSource AudioSource;
    private Dictionary<int, RM_Object> rmObjects;
    public static List<string> RocketParameterNames;
    void Awake()
    {
#if UNITY_EDITOR
        EditorApplication.update += EditorUpdate;
#endif
        if (Application.isEditor)
        {
            Application.runInBackground = true;
        }
        if (RocketParameterNames == null)
        {
            RocketParameterNames = new List<string>();
        }
    }

#if UNITY_EDITOR
    void EditorUpdate()
    {

    }

    void OnDestroy()
    {
        EditorApplication.update -= EditorUpdate;
    }
#endif

    void Connect()
    {
        Device = new Device("Revision2019", false);
        Device.Connect();  //vain clientillä, ei playerilla

        Application.runInBackground = true;
        Cursor.visible = true;
        string nimi = Device.MakeTrackPath("SYNC");
        AudioSource.Pause();
    }
    void SetRow(int r)
    {
        frow = r;
        row = r;
        AudioSource.time = time = r / BPM * 60f / 8f;
        BPMs = BPM;
    }
    void Pause(bool a)
    {
        if (a)
        {
            AudioSource.Pause();
        }
        else
        {
            AudioSource.UnPause();
        }
    }
    void IsPlaying(bool a)
    {
        if (a)
        {
            AudioSource.Pause();
        }
        else
        {
            AudioSource.UnPause();
        }
    }
    public static float GetVal(string str)
    {
        if (Device != null)
        {
            if(RocketParameterNames == null)
            {
                RocketParameterNames = new List<string>();
            }
            if(RocketParameterNames.Contains(str) == false)
            {
                RocketParameterNames.Add(str);
            }
            return Device.GetTrack(str).GetValue(frow);
        }
        else
        {
            return 0f;
        }
    }

    void Update()
    {
        if(AudioSource == null)
        {
            return;
        }
        
        if (AudioSource.isPlaying)
        {
            time = AudioSource.time;
            frow = AudioSource.time * BPM / 60f * 8f;
            row = (int)frow;
        }

        if (Device == null || !Device.Update(row))
        {
            Connect();
            Device.SetRow += SetRow;
            Device.Pause += Pause;
        }
    }
}
