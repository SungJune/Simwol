using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CharaAnimation : MonoBehaviour
{
    Animator animator;
    CharacterStatus status;
    Vector3 prePosition;
    bool isDown = false;
    bool attacked = false;

    public bool IsAttacked()
    {
        return attacked;
    }

    void StartAttackHit()
    {
        Debug.Log("StartAttackHit");
    }

    void EndAttackHit()
    {
        Debug.Log("EndAttackHit");
    }

    void EndAttack()
    {
        attacked = true;
    }
    // Start is called before the first frame update
    void Start()
    {
        animator = GetComponent<Animator>();
        status = GetComponent<CharacterStatus>();
    }

    // Update is called once per frame
    void Update()
    {
        Vector3 delta_position = transform.position - prePosition;
        animator.SetFloat("Speed", delta_position.magnitude / Time.deltaTime);
        if (attacked && !status.attacking)
        {
            attacked = false;
        }
        animator.SetBool("Attacking", (!attacked && status.attacking));

        if (!isDown && status.died)
        {
            isDown = true;
            animator.SetTrigger("Down");
        }
        prePosition = transform.position;
    }
}
