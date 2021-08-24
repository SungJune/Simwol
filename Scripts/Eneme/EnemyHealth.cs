using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

public class EnemyHealth : MonoBehaviour
{
    public int startingHealth = 50;
    public int currentHealth;

    //슬라임이 공격받았을때 빨갛게 하기위한 변수
    public float flashSpeed = 5f; // flashColour의 색상을 지속시키는 시간 변수  
    public Color flashColour = new Color(1f, 0f, 0f, 0.1f);

    public float sinkSpeed = 1f;

    bool isDead;
    bool isSinking;
    bool damaged;

    public static EnemyHealth instance;
    //[SerializeField]
    //private GameObject go_potion_item_prefabs; //포션

    private void Start()
    {
        Init();
       
    }

    public void Init()
    {
        instance = this;
        //몬스터가 시작할때 체력이 가득찬 상태로 다시 초기화
        currentHealth = startingHealth;

        //죽지않았고 데미지를 받지않았고 가라앉고 있지않다고 플래그르 설정
        isDead = false;
        damaged = false;
        isSinking = false;

        //몬스터의 Collider 를 Trigger 가 아니도록 변경
        //Trigger 가 true 면 지면이나 플레이어와 충돌하지 않습니다. 
        BoxCollider collider = transform.GetComponentInChildren<BoxCollider>();
        collider.isTrigger = false;

        //더이상 플레이어를 찾아 길찾기를 하지않도록 NavMeshAgent를 비활성화 
       // GetComponent<NavMeshAgent>().enabled = true;
    }

    //플레이어에게 대미지를 입었을떄 
    public void TakeDamage(int amount)
    {
        damaged = true;
        currentHealth -= amount;
        

        if (currentHealth <= 0 && !isDead)
        {
            Death();

            Debug.Log("뒤졌");
        }
        /*
        void PushBack(Vector3 playerPosition, float pushBack)
        {
            // 주인공 캐릭터의 위치와 몬스터 위치의 차이 벡터를 구합니다.
            Vector3 diff = playerPosition - transform.position;
            //주인공과 몬스터 사이의 차이를 정규화시킨다 (거리를 1 로 만드는것이 정규화)
            diff = diff / diff.sqrMagnitude;

            //현재 몬스터의 RigidBody에 힘을 가합니다. 
            //플레이어 반대방향으로 밀려나는데,pushBack 만큼 비례해서 더 밀려납니다.
            GetComponent<Rigidbody>().AddForce(diff * -10000f * pushBack);

        }
        */
    }

    //플레이어에게 대미지를입었을떄 뒤로밀림
    public IEnumerator StartDamage(int damage, Vector3 playerPosition, float delay, float pushBack)
    {
        yield return new WaitForSeconds(delay);
        try
        {
            TakeDamage(damage);
            Vector3 diff = playerPosition - transform.position;
            diff = diff / diff.sqrMagnitude;
            GetComponent<Rigidbody>().AddForce((transform.position - new Vector3(diff.x,diff.y,0f))*50f*pushBack);
        } catch (MissingReferenceException e)
        {
            Debug.Log(e.ToString());
        }
    }
   

    // Update is called once per frame
    void Update()
    {
        if (damaged)
        {
            transform.GetChild(0).GetComponent<Renderer>().material.SetColor("_OutlineColor", flashColour);
        }
        else
        {
            transform.GetChild(0).GetComponent<Renderer>().material.SetColor("_OutlineColor", Color.Lerp(transform.GetChild(0).GetComponent<Renderer>().material.GetColor
                ("_OutlineColor"), Color.black, flashSpeed * Time.deltaTime));
        }

        damaged = false;
        
        if (isSinking)
        {
            transform.Translate(- Vector3.up * sinkSpeed * Time.deltaTime);
        }
    }
    void Death()
    {
        isDead = true;
        transform.GetChild(0).GetComponent<BoxCollider>().isTrigger = true;
        StartSinking();
        //Instantiate(go_potion_item_prefabs,transform.position, Quaternion.identity);
    }
    public void StartSinking()
    {
       // GetComponent<NavMeshAgent>().enabled = false;
        GetComponent<Rigidbody>().isKinematic = true;
        isSinking = true;
        Destroy(gameObject, 2f);
    }
}
