using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WheatDensity : MonoBehaviour
{
    void Update()
    {
        float wheatScale = SyncUp.GetVal("Wheat Scale");
        transform.localScale = new Vector3(wheatScale, -0.05f, wheatScale);
    }
}
