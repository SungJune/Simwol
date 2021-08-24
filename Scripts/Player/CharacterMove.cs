using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// 캐릭터를 이동시킨다. 
public class CharacterMove : MonoBehaviour
{
    // 중력값
    const float GravityPower = 9.8f;

    // 목적지에 도착했다고 보는 정지거리 
    const float StoppingDistance = 0.6f;

    // 현재이동 속도
    Vector3 velocity = Vector3.zero;

    // 캐릭터 컨트롤러의 캐시 
    CharacterController characterController;

    //도착했는가 true/ false 
    public bool arrived = false;

    // 방향을 강제로 지시하는가 
    bool forceRotate = false;

    //강제로 향하게 하고싶은방향 
    Vector3 forceRotateDirecion;

    //목적지
    public Vector3 destination;

    //이동속도 
    public float walkSpeed =6.0f;

    //회전 속도 
    public float rotateionSpeed = 360.0f;
    // Start is called before the first frame update
    void Start()
    {
        characterController = GetComponent<CharacterController>();
        destination = transform.position;
    }

    // Update is called once per frame
    void Update()
    {
        //이동속도 velocity 를 갱신한다
        if (characterController.isGrounded)
        {
            // 수평면에서 이동을 고려하므로 xz만 다룬다
            Vector3 destinationXZ = destination;

            //목적지와 현재 위치 높이를 똑같이한다
            // destinationXZ 는 목적지를 현재위치와 같은높이로 하기위해 사용
            destinationXZ.y = transform.position.y;

            //****** 여기부터 XZ만으로 생각한다 ******
            //  목적지와 현재 위치 높이를 똑같이한다
            // 목적지의 방향과 거리를 구한다 
            Vector3 direction = (destinationXZ - transform.position).normalized;
            float distance = Vector3.Distance(transform.position, destinationXZ);

            //현재 속도를 보관한다 
            Vector3 currentVelocity = velocity;

            //목적지에 가까이 왔으면 도착 
            if (arrived || distance < StoppingDistance)
                arrived = true;

            // 이동속도를 구한다
            // arrived -> 목적지 도착여부
            if (arrived)
                velocity = Vector3.zero;
            else
                velocity = direction * walkSpeed;

            //부드럽게 보간 처리 
            velocity = Vector3.Lerp(currentVelocity, velocity, Mathf.Min(Time.deltaTime * 5.0f, 1.0f));
            velocity.y = 0;

            if (!forceRotate)
            {
                //바꾸고 싶은 방향으로 변경한다
                if (velocity.magnitude > 0.1f && !arrived)
                {
                    //이동하지 않았다면 방향은 변경하지않는다.
                    Quaternion characterTargetRotation = Quaternion.LookRotation(direction);
                    transform.rotation = Quaternion.RotateTowards(transform.rotation, characterTargetRotation, rotateionSpeed * Time.deltaTime);
                }
            }
            else
            {
                //강제로 방향을 지정한다.
                Quaternion characterTargetRotation = Quaternion.LookRotation(direction);
                transform.rotation = Quaternion.RotateTowards(transform.rotation, characterTargetRotation, rotateionSpeed * Time.deltaTime);
            }

        }

        //중력
        velocity += Vector3.down * GravityPower * Time.deltaTime;

        //땅에 닿아있다면 지면을 꽉누른다
        // (유니티의 CharcaterController 특성 떄문에)
        Vector3 snapGround = Vector3.zero;
        if (characterController.isGrounded)
            snapGround = Vector3.zero;

        //CharacterController 를 사용하여 움직인다 
        characterController.Move(velocity * Time.deltaTime + snapGround);
        if (characterController.velocity.magnitude < 0.1f)
            arrived = true;

        //강제로 방향변경을 해제한다
        if (forceRotate && Vector3.Dot(transform.forward, forceRotateDirecion) > 0.99f)
            forceRotate = false;
    }

    // 목적지를 설정한다 인수 destination은 목적지
    public void SetDestination(Vector3 destination)
    {
        arrived = false;
        this.destination = destination;
    }
    // 지정한 방향으로 향한다
    public void SetDirection(Vector3 direction)
    {
        forceRotateDirecion = direction;
        forceRotateDirecion.y = 0;
        forceRotateDirecion.Normalize();
        forceRotate = true;
    }

    //이동을 그만둔다
    public void StioMove()
    {
        //현재 지정을목적지로 한다
        destination = transform.position;
    }

    //목적지에 도착했는지 조사한다 (도착했다 true / 도착하지않았다 false )
    public bool Arrived()
    {
        return arrived;
    }
}
