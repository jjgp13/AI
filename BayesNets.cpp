#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

class Node{
	public:
		string nodeName;
		vector<string> nodeParents;
		vector<string> nodeProbabilitiesQuery;
		vector<float> nodeProbabilitiesResult;

		void setNodeName(string node_name);
		void setNodeParents(vector<string> line);
};

void Node::setNodeName(string node_name){
	nodeName = node_name;
}

void Node::setNodeParents(vector<string> node_parents){
	for (int i = 0; i < node_parents.size(); i++) {
		nodeParents.push_back(node_parents[i]);
	}
}

//Limpiar la linea de '+','-', para los padres
string cleanProbaLine(string probaLine){
	probaLine.erase(remove(probaLine.begin(), probaLine.end(), '+'), probaLine.end());
	probaLine.erase(remove(probaLine.begin(), probaLine.end(), '-'), probaLine.end());
	return probaLine;
}

//Funcion para extraer los nodos de la linea Nodes
vector<string> getNodes(string nodesLine){
	size_t found = nodesLine.find(',');
	vector<string> vect;
	if(found!=string::npos){
		stringstream ss(nodesLine);
		string node;

	    while (getline(ss, node, ',')){
	        vect.push_back(node);
	    }
	}else{
		vect.push_back(nodesLine);
	}
	return vect;
}

string getChild(string probaLine){
	vector<string> parents;
	string child;
	
	size_t found = probaLine.find("|");
	
	if (found!=std::string::npos) child = probaLine.substr(0,probaLine.find("|"));
	else child = probaLine.substr(0,probaLine.find("="));
	
	child = cleanProbaLine(child);
	
	return child;
}

vector<string> getParents(string probaLine){
	vector<string> parents;
	size_t found = probaLine.find("|");
  	if (found!=std::string::npos){
  		//Obtener los padres, es decir, los que estan despues de | y antes de =
		string temp = probaLine.substr(probaLine.find("|")+1, probaLine.find("=") - probaLine.find("|")-1);
		parents = getNodes(cleanProbaLine(temp));
  	}
    return parents;
}

//De una linea de probabilidad, extraer el resultado
float getProNumber(string lineP){
	float n = std::stof(lineP.substr(lineP.find("=")+1, lineP.size()-lineP.find("=")));
	return n;
}

//Obtener el query de una linea de probabilidad
string getQuery(string pLine){
	string query = pLine.substr(0, pLine.find("="));
	return query;
}

//Cambiar los simbolos de la probabilidades para obtener los complementos y llenbar la tabla
string changeVarStatus(string pLine){
	if(pLine[0]=='+') pLine[0] = '-';
	else pLine[0] = '+';
	return pLine;
}

vector<string> getJointP(string query){
	vector<string> v;
	size_t found = query.find("|");
  	if (found!=std::string::npos){
  		query.replace(query.find("|"), 1, ",");
  		v = getNodes(query);
  	}else{
  		v.push_back(query);
  	}
  	return v;
}

vector<string> getConditionalV(string query){
	vector<string> v;
	size_t found = query.find("|");
  	if (found!=std::string::npos){
  		string temp;
  		temp = query.substr(query.find("|")+1, query.size()-query.find("|"));
  		v = getNodes(temp);
  	}
  	//if (v.size() == 0) cout << "No hay conditional var"<<endl;
  	return v;	
}

//Obtener la enumeracion de las nodos. Identificando Querys, Evidence and Hidden nodes.
vector<string> getEnum(vector<Node> v, vector<string> knowNodes){
	vector<string> querys;
	for (int i = 0; i < v.size(); i++){
		//Si el nodo no tiene padres, la probabilidad se construye sola
		if(v[i].nodeParents.size() == 0){
			bool isHiddenNode = false;
			for (int j = 0; j < knowNodes.size(); j++){
				//Si el nombre del nodo esta en el joint, se conoce el estado de la variable
				if(v[i].nodeName == cleanProbaLine(knowNodes[j])){
					querys.push_back(knowNodes[j]);
					isHiddenNode = true;
					j = knowNodes.size();
				}
			}
			if(!isHiddenNode) querys.push_back(v[i].nodeName);
		}else{
			string temp;
			bool isHiddenNode = false;
			for (int j = 0; j < knowNodes.size(); j++){
				//Si el nombre del nodo esta en el joint, se conoce el estado de la variable
				if(v[i].nodeName == cleanProbaLine(knowNodes[j])){
					temp = (knowNodes[j]);
					isHiddenNode = true;
					j = knowNodes.size();
				}
			}
			if(!isHiddenNode) temp = v[i].nodeName;

			temp += '|';
			
			for (int j = 0; j < v[i].nodeParents.size(); j++){
				bool isHiddenNode = false;
				for (int k = 0; k < knowNodes.size(); k++){
					//Si el nombre del nodo esta en el joint, se conoce el estado de la variable
					if( v[i].nodeParents[j] == cleanProbaLine(knowNodes[k]) ) {
						temp += knowNodes[k];
						isHiddenNode = true;
						k = knowNodes.size();
					}
				}
				if(!isHiddenNode) temp += v[i].nodeParents[j];
				if(j != v[i].nodeParents.size()-1) temp+=',';
			}
			querys.push_back(temp);
		}
	}
	return querys;
}

//Obtener todas las permutaciones 
vector<string> getBitStrings(unsigned int n)
{
    vector<string> result;
    if(n == 0) return result;
    if (n <= 1)
    {
        result.push_back("+");
        result.push_back("-");
    }
    else
    {   // recurse, then add extra bit chars
        vector<string> sub = getBitStrings(n-1);
        for (vector<string>::const_iterator it = sub.cbegin(); it != sub.cend(); ++it){
            result.push_back(*it+'+');
            result.push_back(*it+'-');
        }
    }
    return result;
}

string myreplace(string s, string toReplace, string replaceWith){
    return(s.replace(s.find(toReplace), toReplace.length(), replaceWith));
}

vector<string> expandEnumeration(vector<string> enumVect){
	vector<string> hiddenNodes;
	//Obtener los nombres de nodes que son variables.
	for (int i = 0; i < enumVect.size(); i++){
		string child;
		vector<string> parents;
		//Extraer al hijo
		size_t found = enumVect[i].find("|");
		if (found!=std::string::npos) child = enumVect[i].substr(0, enumVect[i].find("|"));
		else{
			child = enumVect[i];
			parents = getParents(enumVect[i]);
		}
		//Agregar a hidden nodes si no empiezan con + o -
		if(child[0] != '+' && child[0] != '-') hiddenNodes.push_back(child);
		for (int j = 0; j < parents.size(); j++) 
			if(parents[j].at(0) != '+' && parents[j].at(0) != '-') 
				hiddenNodes.push_back(parents[j]);
	}

	//Aqui ya tengo los hidden nodes
	//Ahora expandir la enumeracion.
	vector<string> enumVectExpand;
	//Generar las combinaciones de los simbolos
	//Si no hay hidden nodes, no se necesita expander
	vector<string> perm = getBitStrings(hiddenNodes.size());
	if(perm.size()==0) {
		enumVectExpand = enumVect;
	}else{
		for (int i = 0; i < perm.size(); i++){
			vector<string> tempH = hiddenNodes;
			for (int j = 0; j < tempH.size(); j++){
				tempH[j].insert(0, 1,perm[i].at(j));
			}

			for (int i = 0; i < enumVect.size(); i++){
				string temp = enumVect[i];
				bool hiddenInP=false;
				for (int j = 0; j < hiddenNodes.size(); j++){
					std::size_t found = enumVect[i].find(hiddenNodes[j]);
					if (found!=string::npos){
						temp = myreplace(temp, hiddenNodes[j], tempH[j]);
						hiddenInP = true;
					}
				}
				if(!hiddenInP) temp = enumVect[i];
				enumVectExpand.push_back(temp);
			}

			for(int j = 0; j < tempH.size(); j++) tempH[j] = cleanProbaLine(tempH[j]);
		}
	}
	

	/*for (int i = 0; i < enumVectExpand.size(); i++){
		cout << enumVectExpand[i] << " * ";
	}*/
	//cout <<endl;
	return enumVectExpand;
}

//Del chorizo, sacar cada una de sus probabilidades y buscarlo en la red.
float findQueryInNet(vector<string> querys, vector<Node> net){
	float tempRes = 1;
	float finalRes = 0;
	int netSize = net.size();
	int aux=0;
	//Para cada una de las querys, buscar su numero en la red
	for (int i = 0; i < querys.size(); i++){
		for (int j = 0; j < net.size(); j++){
			//buscar en su vector de probabilidades si la query esta.
			for (int k = 0; k < net[j].nodeProbabilitiesQuery.size(); k++){
				if(querys[i] == net[j].nodeProbabilitiesQuery[k]){
					tempRes *= net[j].nodeProbabilitiesResult[k];
					aux++;
				}
			}
		}
		if(aux == netSize){
			finalRes += tempRes;
			tempRes = 1;
			aux=0;
		}
	}
	return finalRes;
}

//La carnita
//float enumartionAlgorithm(string query, vector<Node> v){
float enumerationAlgorithm(string query, vector<Node> v){
	float result;
	//Del query obtener el jointProbability y las conditional variables.
	vector<string> jointP;
	vector<string> conditionalV;
	jointP = getJointP(query);
	conditionalV = getConditionalV(query);

	//Construir el chorizo de las enumaeraciones 
	vector<string> jointProQuerys;
	vector<string> conditionalVarQuerys;
	//Obtener el la parte de arriba y de abajo de la division de el algoritmo de enumeración
	jointProQuerys = getEnum(v, jointP);
	//Si no hay condicional var, no se calcula
	if(conditionalV.size()>0) conditionalVarQuerys = getEnum(v, conditionalV);
	
	/*for (int i = 0; i < jointProQuerys.size(); i++) cout << jointProQuerys[i] << " * ";
	cout << endl;
	cout << " / "<<endl;
	for (int i = 0; i < conditionalVarQuerys.size(); i++) cout << conditionalVarQuerys[i] << " * ";*/
	
	//cout << endl;
	//cout << "Vector expandido"<<endl;

	//Expandir las enumeraciones dada si es positivo y falso;
	jointProQuerys = expandEnumeration(jointProQuerys);
	conditionalVarQuerys = expandEnumeration(conditionalVarQuerys);
	//Calcuar el resutlado del chorizo de arriba
	float upResult = findQueryInNet(jointProQuerys, v);
	float downResult;
	//Si hay chorizo abajo, calcular el resultado y dividirlo
	//Sino el resultado solo es el de arriba;
	if(conditionalVarQuerys.size() > 0){
		downResult = findQueryInNet(conditionalVarQuerys, v);
		result = upResult / downResult;
	}else{
		result = upResult;
	}

	return result;
}

int main(int argc, char* argv[]) {
	//String para almacenar una linea del archivo
	string line;
	//Estructura para guardar los strings  del archivo
	vector<string> lines;
	//Estructura para almacenar los nodos
	vector<Node> nodesVector;

	//Leer el archivo
	if(argc == 2){
		ifstream myfile (argv[1]);
		if (myfile.is_open()){
			while (getline(myfile,line)){
				lines.push_back(line);
			}
			myfile.close();
		}
	} else cout << "Usage: ./nombre_del_ejecutable nombre_del_archivo";

	//Recorrer cada una de las las lineas del archivo HIJOS Y PADRES
	for (int i = 0; i < lines.size(); i++){
	  	if(lines[i].length() != 0){
	  		//Crear los nodos con los padres y los hijos
	  		if(lines[i].compare("[Probabilities]") == 0) {
	  			i++;
	  			while( lines[i].length() != 0 && lines[i] != "[Queries]" ){
	  				string child;
	  				vector<string> parents;
	  				child = getChild(lines[i]);
	  				parents = getParents(lines[i]);
	  				//Si el hijo extraido de la linea, es diferente al de la siguiente linea
	  				//Crear el nodo, y establecer el hijo y su padres
	  				if(child != getChild(lines[i+1])){
	  					Node tempNode;
	  					tempNode.setNodeName(child);
	  					tempNode.setNodeParents(parents);
	  					nodesVector.push_back(tempNode);
	  				}
	  				i++;
	  			}
	  		}
	  	}
	}

	//Recorrer la lineas de probabilidad de nuevo para obtener las tablas de probabilidad
	for (int i = 0; i < lines.size(); i++){
	  	if(lines[i].length() != 0){
	  		//Crear los nodos con los padres y los hijos
	  		if(lines[i].compare("[Probabilities]") == 0) {
	  			i++;
	  			while( lines[i].length() != 0 && lines[i] != "[Queries]" ){
	  				//Obtener el primer elemento
	  				string node = getChild(lines[i]);
	  				for (int j = 0; j < nodesVector.size(); j++){
	  					if(node == nodesVector[j].nodeName){
	  						nodesVector[j].nodeProbabilitiesQuery.push_back(getQuery(lines[i]));
	  						nodesVector[j].nodeProbabilitiesResult.push_back(getProNumber(lines[i]));
	  						j = nodesVector.size();
	  					}
	  				}
	  				i++;
	  			}
	  		}
	  	}
	}

	///Hasta aqui ya esta establecido la red. Ahora hay que calcular las probabilidades
	//1. Sacar las probabilidades restanes por complementos
	for (int i = 0; i < nodesVector.size(); i++){
		int iniSize = nodesVector[i].nodeProbabilitiesQuery.size();
		for(int j = 0; j < iniSize; j++){
			string oppositeP;
			float result;
			oppositeP = changeVarStatus(nodesVector[i].nodeProbabilitiesQuery[j]);
			//Cambiar la probabilidad
			result = 1.0f - nodesVector[i].nodeProbabilitiesResult[j];
			//Añadir al nodo esta linea de probabilidad
			nodesVector[i].nodeProbabilitiesQuery.push_back(oppositeP);
			nodesVector[i].nodeProbabilitiesResult.push_back(result);
		}
	}


	//Buscar si la probabilidad del query ya esta dada o hay que calcularla
	for (int i = 0; i < lines.size(); i++){
	  	if(lines[i].length() != 0){
	  		//Crear los nodos con los padres y los hijos
	  		if(lines[i].compare("[Queries]") == 0) {
	  			i++;
	  			while( lines[i].length() != 0 && lines[i] != "[Queries]" ){
	  				bool ready = false;
	  				//Para cada nodo
	  				for (int j = 0; j < nodesVector.size(); j++){
	  					//Buscar en su tabla de probabilidad
	  					for(int k=0; k < nodesVector[j].nodeProbabilitiesQuery.size(); k++){
	  						//Si la probabilidad ya esta dada, imprimir, sino calcularla
	  						if(getQuery(lines[i]) == getQuery(nodesVector[j].nodeProbabilitiesQuery[k])){
	  							cout << nodesVector[j].nodeProbabilitiesResult[k] <<endl;
	  							j = nodesVector.size();
	  							ready = true;
	  							break;
	  						}	  							
	  					}
	  				}
	  				//Si la probabilidad no esta calculada utilizar el algoritmo de enumeración
	  				if(!ready) {
	  					//Calcular la probabilidad, segun el Query
	  					cout << enumerationAlgorithm(getQuery(lines[i]), nodesVector)<<endl;
	  				}
	  				i++;
	  			}
	  		}
	  	}
	}
	

	//Impresion de la red completa
	/*for (int i = 0; i < nodesVector.size(); i++){
		cout << "Node name: " << nodesVector[i].nodeName << endl;
		cout << "Parents: ";
		for (int j = 0; j < nodesVector[i].nodeParents.size(); j++){
			if(nodesVector[i].nodeParents.size() > 0){
				cout << nodesVector[i].nodeParents[j] << ", ";
			}
		}
		cout<<endl;
		for (int j = 0; j < nodesVector[i].nodeProbabilitiesQuery.size(); j++){
			if(nodesVector[i].nodeProbabilitiesQuery.size() > 0){
				cout << nodesVector[i].nodeProbabilitiesQuery[j] << " " <<nodesVector[i].nodeProbabilitiesResult[j] <<endl;
			}
		}
		cout << endl;
	}*/

	return 0;
}