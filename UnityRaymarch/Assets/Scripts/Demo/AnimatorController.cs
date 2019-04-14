using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExecuteInEditMode]
public class AnimatorController : MonoBehaviour
{
    private int _animid;
    void Update()
    {
        var animator = GetComponent<Animator>();
        int animid = (int)SyncUp.GetVal("Animator AnimationID " + gameObject.name);
        if (animid != _animid)
        {
            animator.StopPlayback();
            animator.speed = SyncUp.GetVal("Animator Speed " + gameObject.name);
            animator.SetInteger("AnimId", animid);
            animator.StartPlayback();
            _animid = animid;
        }
    }
}
