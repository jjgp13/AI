using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class bfsAlgorithm : MonoBehaviour
{
    public InputField[] stacks_SS, stacks_GS;
    public Button startBtn;
    public GameObject[] stackBlocks, goalBlocks;
    public State startState, goalState;
    public int totalStates;
    public bool freeze;

    public class State
    {
        //stacks
        public int n_state;
        public int child_of;
        public float stateCost = 1;
        public int[,] stack_values = new int[3, 3];
        List<State> statesFromThis = new List<State>();

        public State(){
            n_state = 0;
            stateCost = 0;
            stack_values = new int[3, 3] { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };
        }

        public State(int[,] stackValues, int childOf, int nState, int cost){
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    stack_values[i, j] = stackValues[i, j];
            child_of = childOf;
            n_state = nState;
            stateCost += cost;
        }

        public List<State> childStates{
            get{ return statesFromThis; }
        }

        public void generatedChildsStates(ref int previosTotalStates){
            int[,] childStack = copyValues(this.stack_values);
            int valueToChange = 0;
            int i, j;
            for (i = 0; i < 3; i++) {
                for (j = 2; j >= 0; j--) {
                    if (childStack[i, j] != 0) {
                        valueToChange = childStack[i, j];
                        childStack[i, j] = 0;
                        changeValue(i, childStack, valueToChange, ref previosTotalStates);
                        childStack = copyValues(this.stack_values);
                        j = -1;
                    }
                }
            }
        }

        void changeValue(int i, int[,] tempStack, int value, ref int stateNumber){
            for (int w=0; w < 3; w++){
                int[,] childStack = copyValues(tempStack);
                if (i != w){
                    for(int z = 0; z < 3; z++){
                        if (childStack[w, z] == 0){
                            childStack[w, z] = value;
                            int tempCost = Mathf.Abs(w-z);
                            State childState = new State(childStack, this.n_state, stateNumber, tempCost);
                            stateNumber++;
                            statesFromThis.Add(childState);
                            z = 3;
                        }
                    }
                }
            }
        }

        int[,] copyValues(int[,] original){
            int[,] temp= new int[3,3];
            for(int i=0; i<3; i++)
                for (int j = 0; j < 3; j++)
                    temp[i, j] = original[i, j];
            return temp;
        }

        public void showStackValues(){
            for(int i=2; i>=0; i--) Debug.Log(this.stack_values[0, i] + " " + this.stack_values[1, i] + " " + this.stack_values[2, i]);
        }
    }

    public void getStackValues()
    {
        int xSpawnPos = -7, ySpawnPos = 0, subStringPos = 0;
        //Get start state values
        for (int i = 0; i < 3; i++){
            ySpawnPos = 0;
            string[] substringsSS = stacks_SS[i].text.Split(',');
            for (int j = 0; j < substringsSS.Length; j++){
                if (substringsSS[j] != ""){
                    startState.stack_values[i, j] = int.Parse(substringsSS[j]);
                    Instantiate(stackBlocks[startState.stack_values[i, j] - 1], new Vector2(xSpawnPos, ySpawnPos), Quaternion.identity);
                }
                ySpawnPos += 2;
            }
            //Goal state values
            ySpawnPos = 0;
            string[] substringsGS = stacks_GS[i].text.Split(',');
            for (int j = 0; j < substringsGS.Length; j++){
                if (substringsGS[j] != ""){
                    goalState.stack_values[i, j] = int.Parse(substringsGS[j]);
                    Instantiate(goalBlocks[goalState.stack_values[i, j] - 1], new Vector2(xSpawnPos + 7, ySpawnPos), Quaternion.identity);
                }
                ySpawnPos += 2;
            }
            xSpawnPos += 2;
            subStringPos += 2;
        }
    }

    public bool compareStates(State A, State B){
        for(int i=0; i < 3; i++)
            for(int j=0; j<3; j++)
                if (A.stack_values[i, j] != B.stack_values[i, j]) return false;
        return true;
    }


    private void Start()
    {
        startState = new State();
        goalState = new State();
        totalStates = 1;
    }

    void spawnCube(int i, GameObject cube){
        switch (i)
        {
            case 0:
                Instantiate(cube, new Vector2(-7, 3), Quaternion.identity);
                break;
            case 1:
                Instantiate(cube, new Vector2(-5, 3), Quaternion.identity);
                break;
            case 2:
                Instantiate(cube, new Vector2(-3, 3), Quaternion.identity);
                break;
            default:
                Debug.Log("No se encontro el elemento");
                break;
        }
    }

    public IEnumerator changeGraphicalStack(int[,] A, int[,] B)
    {
        int i = 0, j = 0, value = 0;
        for (i = 0; i < 3; i++)
        {
            for (j = 0; j < 3; j++)
            {
                if (A[i, j] != B[i, j] && A[i,j] != 0)
                {
                    value = A[i, j];
                    Destroy(GameObject.FindGameObjectWithTag(value.ToString()));
                    i = j = 3;
                }
            }
        }

        for (i = 0; i < 3; i++)
            for (j = 0; j < 3; j++)
                if (value == B[i, j]) spawnCube(i, stackBlocks[value - 1]); i = j = 3;

        yield return new WaitForSeconds(2);
    }

    public IEnumerator bfs()
    {
        Queue<State> Q = new Queue<State>();
        List<State> path = new List<State>();
        List<State> S = new List<State>();
        
        Q.Enqueue(startState);
        S.Add(startState);

        while (Q.Count > 0)
        {
            State s = Q.Dequeue();
            if (compareStates(s, goalState)){
                while (s.n_state != 0){
                    Debug.Log("State: " + s.n_state + " childOf: " + s.child_of + " Cost: " + s.stateCost);
                    s.showStackValues();
                    foreach (State visited in S){
                        if (s.child_of == visited.n_state){
                            path.Add(s);
                            s = visited;
                        }
                    }
                }
                Debug.Log("State: " + startState.n_state + " childOf: " + startState.child_of + " Cost: " + startState.stateCost);
                startState.showStackValues();
                Q.Clear();

                path.Add(startState);
                path.Reverse();
                
                int i = 0;
                do
                {
                    StartCoroutine(changeGraphicalStack(path[i].stack_values, path[i+1].stack_values));
                    yield return new WaitForSeconds(2);
                    i++;
                } while (i < path.Count-1);

                break;

            } else s.generatedChildsStates(ref totalStates);

            foreach (State child in s.childStates){
                bool isInS = false;
                foreach(State visited in S)
                    if (compareStates(child, visited)) isInS = true;
                if (!isInS){
                    Q.Enqueue(child);
                    S.Add(child);
                }
            }
        }
    }

    public IEnumerator dfs()
    {
        Stack<State> Q = new Stack<State>();
        List<State> path = new List<State>();
        List<State> S = new List<State>();

        Q.Push(startState);
        S.Add(startState);

        while (Q.Count > 0)
        {
            State s = Q.Pop();
            if (compareStates(s, goalState))
            {
                while (s.n_state != 0)
                {
                    Debug.Log("State: " + s.n_state + " childOf: " + s.child_of + " Cost: " + s.stateCost);
                    s.showStackValues();
                    foreach (State visited in S)
                    {
                        if (s.child_of == visited.n_state)
                        {
                            path.Add(s);
                            s = visited;
                        }
                    }
                }
                Debug.Log("State: " + startState.n_state + " childOf: " + startState.child_of + " Cost: " + startState.stateCost);
                startState.showStackValues();
                Q.Clear();

                path.Add(startState);
                path.Reverse();
                
                int i = 0;
                do
                {
                    StartCoroutine(changeGraphicalStack(path[i].stack_values, path[i + 1].stack_values));
                    yield return new WaitForSeconds(2);
                    i++;
                } while (i < path.Count - 1);

                break;

            }
            else s.generatedChildsStates(ref totalStates);

            foreach (State child in s.childStates)
            {
                bool isInS = false;
                foreach (State visited in S)
                    if (compareStates(child, visited)) isInS = true;
                if (!isInS)
                {
                    Q.Push(child);
                    S.Add(child);
                }
            }
        }
    }
}