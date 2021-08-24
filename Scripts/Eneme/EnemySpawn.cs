using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EnemySpawn : MonoBehaviour
{
    public GameObject enemy;
    public float intervalTime =10f;
    public Transform[] spawnPools;

    // Start is called before the first frame update
    void Start()
    {
        InvokeRepeating("Spawn", intervalTime, intervalTime);    
    }
    void Spawn()
    {
        int spawnPoolIndex = Random.Range(0, spawnPools.Length);
        Instantiate(enemy, spawnPools[spawnPoolIndex].position, spawnPools[spawnPoolIndex].rotation);
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
