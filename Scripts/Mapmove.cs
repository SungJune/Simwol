using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class Mapmove : MonoBehaviour
{
    public string transferMapName;
    public Animator avar;

    private void Start()
    {
        avar = GetComponent<Animator>();
    }
    private void OnTriggerEnter(Collider other)
    {
        avar.SetTrigger("Start");
        if (other.gameObject.name == "Player")
        {
            SceneManager.LoadScene(transferMapName);
        }
    }

}
