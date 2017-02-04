using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Sensor : MonoBehaviour {

    bool wall;

    void Update()
    {
        Debug.Log(wall);
    }

    private void OnTriggerExit(Collider other)
    {
        wall = false;
        //Si no esta rotando, empezar la corrutina de rotate
        StartCoroutine(checkWall());
    }

    private void OnTriggerStay(Collider other)
    {
        wall = true;
    }

    private void OnTriggerEnter(Collider other)
    {
        if(!transform.GetComponentInParent<AgentBehavior>().rotating) transform.GetComponentInParent<Renderer>().material = transform.GetComponentInParent<AgentBehavior>().colores[0];
    }

    IEnumerator checkWall()
    {
        yield return new WaitForSeconds(0.1f);
        if (!wall)
            if (!transform.GetComponentInParent<AgentBehavior>().rotating)
            {
                transform.GetComponentInParent<Renderer>().material = transform.GetComponentInParent<AgentBehavior>().colores[2];
                StartCoroutine(transform.GetComponentInParent<AgentBehavior>().rotate(1));
            }
                
    }
}
