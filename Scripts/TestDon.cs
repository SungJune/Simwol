using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TestDon : MonoBehaviour
{
    private void Awake()
    {
        DontDestroyOnLoad(gameObject);
    }
}
