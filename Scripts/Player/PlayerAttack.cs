using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public class PlayerAttack : MonoBehaviour
{
    //플레이어가 몬스터에게 주는 데미지 수치 
    // 레벨/ 경험치 파츠 업그레이드 챕터에서 캐릭터 성장 시스템을 도입하면 변경될 예정
   
    public int NormalDamage = 10;
    public int SkillDamage = 30;
    public int DashDamage = 30;

    //캐릭터의 공격반경
    //타겟의  Trigger로 어떤 몬스터가 공격반경안에 들어왔는지 판정
    public NormalTarget normalTarget;
    public SkillTarget skillTarget;
    EnemyHealth enemyHealth;
    public static PlayerAttack instance;

    private void Start()
    {
        instance = this;
        enemyHealth = GetComponent<EnemyHealth>();
        DontDestroyOnLoad(gameObject);
    }

    public void NormalAttack()
    {
        //normal Target에 붙어있는 Trigger Collider 에 들어있는 몬스터의 리스트를 조회
        List<Collider> targetList = new List<Collider>(normalTarget.targetList);

        //타겟 리스트 안에있는 몬스터를 foreach 문으로 하나하나 조회
        foreach (Collider one in targetList)
        {
            //타겟의 게임오브젝에 EnemyHealth 라는 스크립트를 가져옵니다. 
            EnemyHealth enemy = one.GetComponent<EnemyHealth>();

            //만약 EnemyHealth 스크립트가 있다면 몬스터이므로 몬스터에게 데미지를 줍니다 
            if (enemy != null)
            {
                //몬스터에게 데미지를 얼마 줄지 얼마나 뒤로 밀려나게할지 (PushBack)
                StartCoroutine(enemy.StartDamage(NormalDamage, transform.position, 0.5f, 0.5f));
            }
        }
    }

    public void DashAttack()
    {
        //normalTarget에 붙어있느 Trigger Collider에 들어있는 몬스터의 리스트를 조회
        List<Collider> targetList = new List<Collider>(skillTarget.targetList);

        //타깃 리스트안에있는 몬스터를 Foreach 문으로 하나하나 조회 
        foreach (Collider one in targetList)
        {
            //타겟의 게임오브젝에 EnemyHealth 라는 스크립트를 가져옵니다. 
            EnemyHealth enemy = one.GetComponent<EnemyHealth>();

            //만약 EnemyHealth 스크립트가 있다면 몬스터이므로 몬스터에게 데미지를 줍니다 
            if (enemy != null)
            {
                //몬스터에게 데미지를 얼마 줄지 얼마나 뒤로 밀려나게할지 (PushBack)
                StartCoroutine(enemy.StartDamage(DashDamage, transform.position, 1f, 2f));
            }
        }
    }
    public void SkillAttack()
    {
        //normalTarget 에 붙어있는 Trigger Colider에 들어있는 몬스터를 조회 
        List<Collider> targetList = new List<Collider>(skillTarget.targetList);

        //타겟 리스트 에 있는 몬스터를 foreach 문으로 하나하나 조회
        foreach (Collider one in targetList)
        {
            //타겟의 게임오브젝트에 EnemyHealth 라는 스크립트를 가져온다
            EnemyHealth enemy = GetComponent<EnemyHealth>();

            //만약 EnemyHealth 스크립트가 있다면 몬스터이므로 몬스터에게 대미지를준다
            if (enemy != null)
            {
                //몬스터에게 데미지를 얼마 줄지 얼마나 뒤로 밀려나게할지 (PushBack)
                StartCoroutine(enemy.StartDamage(SkillDamage, transform.position, 1f, 2f));
            }
        }
    }
}
