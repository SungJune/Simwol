using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class ActionControler : MonoBehaviour
{
    [SerializeField]
    public float range; // 습득가능한거리
    private bool pickupActivated = false; // 습득가능할시 true
    private RaycastHit hitinfo; //충돌체 정보저장

    //아이템 레이어에만 반응하도록 레이어 마스크 설정
    [SerializeField]
    private LayerMask layerMask;

    //필요 컴포넌트 
   // [SerializeField]
    //private Text actionText;

    // Update is called once per frame
    void Update()
    {
        CheckItem();
        TryAction();
    }
    private void TryAction()
    {
        CheckItem();
        CanPickUp();
    }
    private void CanPickUp()
    {
        if (pickupActivated)
        {
            if (hitinfo.transform != null)
            {
                Debug.Log(hitinfo.transform.GetComponent<ItemPicUp>().item.itemName + " 획득 ");
                Destroy(hitinfo.transform.gameObject);
                InfoDisappear();
            }
        }
    }

    private void CheckItem()
    {
        if (Physics.Raycast(transform.position,
            transform.TransformDirection(Vector3.forward), out hitinfo, range, layerMask))
        {
            if (hitinfo.transform.tag == "Item")
            {
                ItemInfoAppear();
            }
        }
        else
            InfoDisappear();
    }

    private void ItemInfoAppear()
    {
        pickupActivated = true;
        gameObject.SetActive(true);
        //actionText.gameObject.SetActive(true);
       //actionText.text = hitinfo.transform.GetComponent<ItemPicUp>().item.itemName + " 획득 " + "<color =yellow>" +"</color>";
    }
    private void InfoDisappear()
    {
        pickupActivated = false;
        //gameObject.SetActive(false);
       // actionText.gameObject.SetActive(false);
    }
}
