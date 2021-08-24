using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class NormalTarget : MonoBehaviour
{
    //공격대상에있는 적들의리스트 
    public List<Collider> targetList;

    private void Awake()
    {
        targetList = new List<Collider>();
        DontDestroyOnLoad(gameObject);
    }

    //적 개체가 공격반경안에들어오면 TargetList 에 해당 개체를추가 
    private void OnTriggerEnter(Collider other)
    {
        targetList.Add(other);
    }

    //적 개체가 공격반경을 벗어나면 TargetList 제외
    private void OnTriggerExit(Collider other)
    {
        targetList.Remove(other);
    }
}
