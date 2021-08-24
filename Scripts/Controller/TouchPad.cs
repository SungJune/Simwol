using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using UnityEngine.UI;

public class TouchPad : MonoBehaviour
{
    //_touchPad 오브젝트연결
    private RectTransform _touchPad;

    //터치 입력중 방향컨트롤러의 영역안에있는 입력을 구분하기위한 아이디
    private int _touchId = -1;

    //입력이 시작되는 좌표 
    private Vector3 _startPos = Vector3.zero;

    //방향 컨트롤러가 원으로 움직이는 반지름
    public float _dragRadius = 60f;

    //방향키가 변경되면 플레이어에게 신호를 전달하기위해
    //PlayerMoveMent 스크립트와 연결
    public PlayerMoveMent _player;

    //버튼이 눌렸는지 체크하는 bool 변수
    private bool _buttonPressed = false;

    // Start is called before the first frame update
    void Start()
    {
        //터치패드 의 RectTransform 를 가져온다
        _touchPad = GetComponent<RectTransform>();
        //터치패드의 좌표를 가져온다 움직임의 기준값이됨
        _startPos = _touchPad.position;
        DontDestroyOnLoad(gameObject);
    }
    public void ButtonDown()
    {
        //버튼이 눌렸는지 확인
        _buttonPressed = true;
 
    }
    public void ButtonUp()
    {
        _buttonPressed = false;
        
        //핸들이 떼어 졌을때 터치패드와 좌표를 원래 지형으로 복귀
        HandleInput(_startPos);
    }
     void FixedUpdate()
     {
        //모바일에서는 터치패드 방식으로 여러터치입력을 받아처리 
        HandleTouchInput();
        //모바일이 아닌 pc  나 유니티에디터 상에서 작동할떄는 터치입력이아닌
        // 마우스로 입력을 받는다
        #if UNITY_EDITOR || UNITY_STANDALONE_OSX || UNITY_STANDALONE_WIN || UNITY_WEBPLAYER
        HandleInput(Input.mousePosition);
        #endif
     }

    void HandleTouchInput()
    {
        //터치아이디 (touchid)를 매기기 위한변수
        int i=0;
        //터치입력은 한번에 여러개가 들어올수있으므로 터치가 하나이상 입력되면
        //실행되도록 한다
        if (Input.touchCount > 0)
        {
            //각각의 터치 입력을 하나씩 조회합니다.
            foreach (Touch touch in Input.touches)
            {
                // 터치 아이디 (touchid)를 매기기위해 번호를 1씩증가
                i++;
                //현재 터치 입력의 x,y 좌표를 구한다
                Vector3 touchPos = new Vector3(touch.position.x, touch.position.y);

                //터치 입력이 방금 시작되었다면 혹은 TouchPhase.Began 이면 
                if (touch.phase == TouchPhase.Began)
                {
                    //그리고 터치 좌표가 현재 방향키 범위내에있다면
                    if (touch.position.x <= (_startPos.x + _dragRadius))
                    {
                        //이 터치 아이디 기준으로 방향컨트롤러를 조작
                        _touchId = i;
                    }
                }

                //터치 입력이 움직였거나 가만히있는 상황이라면
                if (touch.phase == TouchPhase.Moved || touch.phase == TouchPhase.Stationary)
                {
                    //터치 아이디로 지정된 경우에만
                    if (_touchId == i)
                    {
                        //좌표입력을 받는다.
                        HandleInput(touchPos);
                    }
                }
                //터치입력이끝났는데
                if (touch.phase == TouchPhase.Ended)
                {
                    //입력받고자 했던 터치아이디라면
                    if (_touchId == i)
                    {
                        //터치아이디를 해제한다.
                        _touchId = -1;
                    }
                }
                
            }

        }
       
    }
    void HandleInput(Vector3 input)
    {
        //버튼이 눌러진 상황이라면 
        if (_buttonPressed)
        {
            //방향컨트롤러 기준 좌표로부터 입력받은 좌표가 얼마나 떨어져있는지 구한다
            Vector3 diffVector = (input - _startPos);

            //입력 지정과 기준좌표의 거리를 비교 하여 최대치보다 크면
            if (diffVector.sqrMagnitude > _dragRadius * _dragRadius)
            {
                //방향 벡터의 거리를 1로 만듭니다
                diffVector.Normalize();
                //그리고 방향 컨트롤러는 최대치만큼만 움직이게 한다.
                _touchPad.position = _startPos + diffVector * _dragRadius;
            }
            else //입력 지점과 기준좌표가 최대치보다 크지않다면
            {
                //현재입력좌표에 방향키를 이동시킵니다. 
                _touchPad.position = input;
            }
        }
        else
        {
            //버튼에 손이 때어지면 방향키를 원래 위치로 되돌려 놓습니다.
            _touchPad.position = _startPos;
        }
        //방향키와 기준 지점의 차이를 구한다
        Vector3 diff = _touchPad.position - _startPos;
        //방향키의 방향을 유지한채로 거리 를나누어 방향만 나누어 구한다
        Vector2 normDiff = new Vector3(diff.x / _dragRadius, diff.y / _dragRadius);

        if (_player != null)
        {
            //플레이어가 연결되어 있으면 플레이어에게 변경된 좌표를 전달
            _player.OnStickChanged(normDiff);
        }
    }
}
