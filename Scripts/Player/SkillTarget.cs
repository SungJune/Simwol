using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SkillTarget : MonoBehaviour
{
    //스킬 공격대상에 있는적들의 리스트 
    public List<Collider> targetList;

    private void Awake()
    {
        targetList = new List<Collider>();
        DontDestroyOnLoad(gameObject);
    }

    //적 개체가 스킬반경안으로 들어올경우 targetList 에 해당개체 추가 
    private void OnTriggerEnter(Collider other)
    {
        targetList.Add(other);
    }

    //적 개체가 스킬반경안을 벗어날경우 targetList 에 해당개체 제외
    private void OnTriggerExit(Collider other)
    {
        targetList.Remove(other);
    }
}
