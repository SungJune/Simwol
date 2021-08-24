using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FollowingCamera1 : MonoBehaviour
{
    public float distanceAway = 7f;
    public float distanceUp = 4f;

    //따라다닐 객체 설정
    public Transform follow;


    private void Awake()
    {
        DontDestroyOnLoad(gameObject);
    }

    private void LateUpdate()
    {
        //카메라의 위치를 distanceUp 만큼위에 distanceAway만큼 앞에 위치
        transform.position = follow.position + Vector3.up * distanceUp - Vector3.forward * distanceAway;
    }
}
