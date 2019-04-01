using RocketNet;
using UnityEngine;
using static RocketNet.Track;
using static RocketNet.Track.Key;

public class RM_SyncData
{
    public Type Interpolation;
    public int Row;
    public float Value;
    public RM_SyncData(Type interpolation, int row, float value)
    {
        Interpolation = interpolation;
        Row = row;
        Value = value;
    }
}
