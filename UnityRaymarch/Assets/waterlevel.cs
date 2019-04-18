using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class waterlevel : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        transform.position = 
            new Vector3(transform.position.x,
            SyncUp.GetVal("Water Level"),
            transform.position.z);
    }
}

