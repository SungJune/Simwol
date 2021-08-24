using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(fileName = "New Item", menuName = "New Item/item")]
public class Item : ScriptableObject
{
    public string itemName;
    public ItemType itemType;
    public Sprite itemImage; //아이탬 이미지
    public GameObject itemPrefab; //아이탬 프리팹

    //public string weponType; //무기유형
    public enum ItemType
    {
        Equipmet,
        Used,
        Ingredient,
        ETC
    }

  
}
