using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AgentBehavior : MonoBehaviour {

    //Velocidad del agente
    public float speed;
    //Estados del agente
    public bool forwardState, backState, rotating;
    //Referencia al sensor
    public GameObject sensor;
    public Material[] colores;

	// Use this for initialization
	void Start () {
        forwardState = true;
        backState = false;
	}
	
	// Update is called once per frame
	void Update () {
        //Si el estado es hacia adelente mover al agente hacia adelante
        if (forwardState) transform.Translate(Vector3.forward * speed * Time.deltaTime);            
        //Si el estado es hacia atras, moverlo hacia atras
        if (backState) transform.Translate(Vector3.back * 1.0f * Time.deltaTime);
    }

    void OnCollisionEnter(Collision col)
    {
        //Cuando se registre una colision con el agente, mover hacia atras
        if(col.gameObject.name == "Wall") StartCoroutine(goBack());
    }

    IEnumerator goBack()
    {
        gameObject.GetComponent<Renderer>().material = colores[1];
        forwardState = false;
        backState = true;
        yield return new WaitForSeconds(1);
        backState = false;
        StartCoroutine(rotate(0));
    }

    public IEnumerator rotate(int side)
    {
        
        //Cambiar el estado a rotar
        rotating = true;
        int count = 0;
        //Giro hacia la derecha 1
        if (side == 1)
        {
            //Tiempo para avanzar al agente un poco más
            yield return new WaitForSeconds(0.5f);
            forwardState = false;
        }

        //Movimiento de rotación, 90 grados.
        while (count < 90)
        {
            if (side==0) transform.Rotate(Vector3.down, 1);
            if (side==1) transform.Rotate(Vector3.up, 1);
            count++;
            yield return new WaitForSeconds(0.01f);
        }

        //Giro hacia la izquierda 0
        if (side == 0)
        {
            yield return new WaitForSeconds(0.5f);
            gameObject.GetComponent<Renderer>().material = colores[0];
        }
        //Reiniciar el movimiento hacia adelante
        forwardState = true;
        //Quitar el estado de giro
        rotating = false;
    }
   
}
