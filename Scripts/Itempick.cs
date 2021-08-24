using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Itempick : MonoBehaviour
{
    private void OnTriggerEnter(Collider other)
    {
        if (other.gameObject.name == "Player")
        {
            Destroy(gameObject);
            Debug.Log("획득함");
        }
    }
}
