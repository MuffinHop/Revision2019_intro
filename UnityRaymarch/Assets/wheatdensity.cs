using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class wheatdensity : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        transform.localScale = new Vector3(SyncUp.GetVal("Wheat Scale"),-0.05f, SyncUp.GetVal("Wheat Scale"));
    }
}
