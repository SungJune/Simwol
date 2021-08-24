using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CharacterStatus : MonoBehaviour
{
    // ******** 공격 장에서 사용한다 ********
    // 체력
    public int HP = 100;
    public int MaxHp= 100;

    // 공격력
    public int Power = 10;

    // 마지막에 공격한대상
    public GameObject lastAttackTarget = null;

    // ********* GUI 및 네트워크 장에서 사용 *********
    // 플레이어이름
    public string characterName = "Player";

    //  ********* 에니메이션 장에서 사용 *********
    // 상태
    public bool attacking = false; // 공격중
    public bool died = false; // 사망
}
