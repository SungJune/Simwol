using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class PlayerHealth : MonoBehaviour
{
    public int startingHealth = 100; //플레이어 체력
    public int currentHealth; //현재체력
    public Slider healthSlider; // 체력게이지 ui와 연결된변수
    public Image damageImage; //플레이어 데미지 입을때 화면빨갛게만듬
    public AudioClip deathClip; //주인공이 데미지를 입었을때 재생할오디오
    public float flashSpeed = 5f;
    public Color flashColour = new Color(1f, 0f, 0f, 0.1f);

    Animator anim;
    AudioSource playerAudio;
    PlayerMoveMent playerMoveMent; //플레이어 움직임을 관리하는 스크립트 불러옴
    bool isDead; //플레이어 가 죽었는지 저장하는 플레그
    bool damaged; //플레이어가 데미지를 입었는지 저장하는 플래그 


    //오브젝트시작시 실행
    private void Awake()
    {
        anim = GetComponent<Animator>();
        playerAudio = GetComponent<AudioSource>();
        playerMoveMent = GetComponent<PlayerMoveMent>();
        currentHealth = startingHealth;
        DontDestroyOnLoad(gameObject);
    }

    private void Update()
    {
        // 이코드는 몬스터에게 공격받았을때 화면을 빨갛게 하는 역활을합니다.
        if (damaged)
        {
            //공격받자마자 DamageImage의 색상을 빨갛게(flashColour)으로 변경 
            damageImage.color = flashColour;
        }
        else
        {
            //공격받고난후 서서히 투명한 색 (Color.clear)으로 변경합니다
            damageImage.color = Color.Lerp(damageImage.color, Color.clear, flashSpeed * Time.deltaTime);
        }
        //damage 플래그로 damaged 가 true 일때 화면을 빨갛게 만드는 명령을 딱 한번만 수행하게 할수있습니다. 
        damaged = false;

    }

    public void TakeDamage(int amount)
    {
        //공격을 받으면 damaged 변수를 true 로 변경
        damaged = true;

        //공격받았을떄 amount 만큼 감소
        currentHealth -= amount;

        //체력게이지에 변경된 체력값을 표시 
        healthSlider.value = currentHealth;

        // 만약 체력이 0이면 캐릭터 사망을 나타냄
        if (currentHealth <= 0 && !isDead)
        {
            Death();
        }
        else
        {
            //죽은게 아니라면 데미지를 입었다는 Trigger 를발생
            anim.SetTrigger("Damage");
        }
    }

    void Death()
    {
        //캐릭터가 죽은거라면 true 
        isDead = true;

        //애니메이션에서 죽었다는 Trigger 발생
        anim.SetTrigger("Die");
        //플레이어 움직임을 관리하는 스크립트 비활성화
        playerMoveMent.enabled = false;
    }
    
}
