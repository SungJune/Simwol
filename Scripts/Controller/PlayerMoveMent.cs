using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerMoveMent : MonoBehaviour
{
    protected Animator avatar;
    protected PlayerAttack playerAttack;
    float lastAttackTime, lastSkillTime, lastDashTime;
    public bool attacking = false;
    public bool dashing = false;
    float h, v;
    private void Start()
    {
        avatar = GetComponent<Animator>();
        playerAttack = GetComponent<PlayerAttack>();
       // DontDestroyOnLoad(gameObject);
    }

    public void OnStickChanged(Vector2 stickPos)
    {
        h = stickPos.x;
        v = stickPos.y;
    }

   

    public void OnAttackDown()
    {
        attacking = true;
        avatar.SetBool("Combo", true);
        StartCoroutine(StartAttack());
    }
    public void OnAttackUp()
    {
        avatar.SetBool("Combo", false);
        attacking = false;
    }
    IEnumerator StartAttack()
    {
        if (Time.time - lastAttackTime > 1f)
        {
            lastAttackTime = Time.time;
            while (attacking)
            {
                avatar.SetTrigger("AttackStart");
                playerAttack.NormalAttack();
                yield return new WaitForSeconds(1f);
            }
        }
    }

    public void OnSkillDown()
    {
        if (Time.time - lastSkillTime > 1f)
        {
            avatar.SetBool("Skill", true);
            lastSkillTime = Time.time;
            playerAttack.SkillAttack();
        }
       
    }
    public void OnSkillUp()
    {
        avatar.SetBool("Skill", false);
    }
    public void OnDashDown()
    {
        lastDashTime = Time.time;
        dashing = true;
        avatar.SetTrigger("Dash");
        playerAttack.DashAttack();
    }
    public void OnDashUp()
    {
        dashing = false;
    }

    private void Update()
    {
        if (avatar)
        {
            float back = 1f;
            if (v < 0f) back = -1f;
            avatar.SetFloat("Speed", (h * h + v * v));
            Rigidbody rigidbody = GetComponent<Rigidbody>();

            if (rigidbody)
            {
                Vector3 speed = rigidbody.velocity;
                speed.x = 4 * h;
                speed.z = 4 * v;
                rigidbody.velocity = speed;
                if (h != 0 && v != 0)
                {
                    transform.rotation = Quaternion.LookRotation(new Vector3(h, 0f, v));
                }
            }
        }
    }
}
            
           


    /*

  float h, v; //가로 세로 방향 
    Rigidbody rigidbody;
    protected Animator avatar;
    float speed = 3f;
    Vector3 movement;

    // Start is called before the first frame update
    void Start()
    {
        rigidbody = GetComponent<Rigidbody>();
        avatar = GetComponent<Animator>();
    }
    public void Move()
    {
        h = Input.GetAxisRaw("Horizontal"); //좌우 입력 -1 이 왼쪽 1이 오른쪽
        v = Input.GetAxisRaw("Vertical"); //상하 입력 -1 이아래 1이 위쪽
        Vector3 move = new Vector3(h, 0, v);//캐릭터가 보는방향을 가르킴
        transform.rotation = Quaternion.LookRotation(move.normalized);//해당방향으로 회전
       
    }

    private void FixedUpdate()
    {
        Move();

        //  dir.x = 0;//몸체 방향은 y 축만 회전하면 되므로 x,z축은 강제고정
        //  dir.z = 0;
        //transform.rotation = dir; //현재 객체의 방향을 생성한 Quaternion 변수로맞춤

        if (avatar)
        {
            if (h == 0 && v == 0)
            {
                avatar.SetBool("Move", false);
                return;
            }
            else
            {
                avatar.SetBool("Move", true);
            }

            /*
            if (Input.GetKey(KeyCode.UpArrow))
            {
                this.transform.Translate(Vector3.forward * speed * Time.deltaTime);
                avatar.SetBool("Move", true);
            }


            if (Input.GetKey(KeyCode.DownArrow))
            {
                this.transform.Translate(Vector3.back * speed * Time.deltaTime);
                avatar.SetBool("Move", true);

            }


            if (Input.GetKey(KeyCode.RightArrow))
            {
                this.transform.Translate(-Vector3.right * speed * Time.deltaTime);
                avatar.SetBool("Move", true);
            }


            if (Input.GetKey(KeyCode.LeftArrow))
            {
                this.transform.Translate(-Vector3.left * speed * Time.deltaTime);
                avatar.SetBool("Move", true);
            }
            */




/*
  private void Update()
    {
        float h = Input.GetAxisRaw("Horizontal") * -1.0f;
        float v = Input.GetAxisRaw("Vertical") * -1.0f;
        movement = new Vector3(h, 0.0f, v).normalized * speed * Time.deltaTime;
    }

    private void FixedUpdate()
    {
        Move();
    }
    public void Move()
    {
        if (movement.magnitude < 0.01f)
        {
            avatar.SetBool("Move", false);
            return;
        }
        rigidbody.velocity = movement;
        rigidbody.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(movement),0.15f);
        avatar.SetBool("Move", true);
    }

 */



    /*







        if (avatar)
            {
                float back = 1f;
                if (v < 0f) back =- 1f;
                //애니메이터에 전달하는값은 속도
                avatar.SetFloat("Speed", (h * h + v * v));
                Rigidbody rigidbody = GetComponent<Rigidbody>();
                if (rigidbody)
                {
                    Vector3 speed = rigidbody.velocity;
                    speed.x = 4 * h;
                    speed.z = 4 * v;
                    rigidbody.velocity = speed;
                    if (h != 0f && v != 0f)
                    {
                        transform.rotation = Quaternion.LookRotation(new Vector3(h, 0f, v));
                    }
                }
            }


          h = Input.GetAxisRaw("Horizontal"); //좌우입력 -1이 왼쪽 1이 오른쪽
            v = Input.GetAxisRaw("Vertical");
            Vector3 move = new Vector3(h, 0, v);
            rigidbody.MovePosition(rigidbody.position + move * speed * Time.deltaTime);
            if (h == 0 && v == 0)
            {
                avatar.SetBool("Move", false);
            }
            else
            {
                avatar.SetBool("Move", true);
            }


         */
