using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExecuteInEditMode]
public class AnimatorController : MonoBehaviour
{
    private int _animid = -1;
    Animator _animator;
    AnimatorClipInfo[] _currentClipInfo;
    string _clipName;

    void Awake()
    {
        _animator = GetComponent<Animator>();
        _animator.speed = 0;
    }
    
    void Update()
    {
        int animid = (int)SyncUp.GetVal("Animator AnimationID " + gameObject.name);
        if (animid != _animid)
        {
            _animid = animid;
        }

        _animator.speed = 0;
        _animator.Play(""+_animid, -1, SyncUp.GetVal("Animator Time " + gameObject.name));
    }
}
