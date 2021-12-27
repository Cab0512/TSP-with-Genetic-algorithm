#include <iostream>
#include <fstream>
#include <algorithm> 
#include "ga.h"
using namespace std;


// Primero necesitamos crear la clase graph para trabajar con grafos, para eso usaremos las funciones habituales que se utilizan en grafos, (e trabajado un poquito con grafos antes en EDA )


Graph::Graph(int V, int vertice_inicial, bool randomg) // constructor 
{
	if(V < 1) // chequemos si la cantdad de vertices es menor que 1
	{
		cout << "Error: numero de vertices <= 0\n";
		exit(1);
	}
	
	this->V = V; // numero de vertices
	this->vertice_inicial = vertice_inicial; // vertice inicial
	this->uniones = 0; // total de  uniones  0
	
	if(randomg)
		generaG();
}





void Graph::informacionG()
{
	cout << "Informacion del grafico:\n\n";
	cout << "Numero de vertices: " << V;
	cout << "\nNumero de uniones: " << mapa_uniones.size() << "\n";
}



void Graph::generaG()
{
	vector<int> vec;
	
	// creamos el vector
	for(int i = 0; i < V; i++)
		vec.push_back(i);
	
	// generamos una permutacion aleatoria 
	random_shuffle(vec.begin(), vec.end());
	
	vertice_inicial = vec[0]; // vertice inicial 
	
	int i, dist;
	for(i = 0; i <= V; i++)
	{
		dist = rand() % (10*V) + 1; // distancia aleatoria en [1,10V]
    //se pude cambiar para otros valores deseados, como para enteros u otros ejemplos 
		
		if(i + 1 < V)
			addunion(vec[i], vec[i + 1], dist);
		else
		{
			// agregamos el vertice final
			addunion(vec[i], vec[0], dist);
			break;
		}
	}
	
	int limite_uniones = V * (V - 1); // calculamis el limite de vertices
	int tamaño_uniones = rand() % (2 * limite_uniones) + limite_uniones;
	
	// agregamos vertices aleatorios
	for(int i = 0; i < tamaño_uniones; i++)
	{
		int inicio = rand() % V; // inicio aleatorio
		int fin = rand() % V; // destino aleatoria
		dist = rand() % (10*V)+1 ; // distancia aleatoria entre[1,10V]
		if(inicio != fin)
		{
			addunion(vec[inicio], vec[fin], dist);
			addunion(vec[inicio], vec[fin], dist);
		}
	}
}



void Graph::addunion(int inicio, int fin, int distancia) // agregar una nueva union 
{
	mapa_uniones[make_pair(inicio, fin)] = distancia; // agrega uniones en el mapa de uniones
}

int Graph::existsunion(int inicio, int fin) // chequear si estan unidos
{
	map<pair<int, int>,int>::iterator it = mapa_uniones.find(make_pair(inicio, fin));
	
	if(it != mapa_uniones.end())
		return it->second; // retorna el costo 
	return -1;
}

void Graph::showG() // muestra todas las uniones del grafo
{	
	map<pair<int, int>, int>::iterator it;
	for(it = mapa_uniones.begin(); it != mapa_uniones.end(); ++it)
		cout << it->first.first << " unido vertice  " << it->first.second << " con distancia " << it->second << endl;
}

// construye el algoritmo genetico
Genetic::Genetic(Graph* graph, int tam_poblacion, int generaciones, int prob_mutacion, int numcross, bool show,bool shows)
{
	if(tam_poblacion < 1) //ver si el tamaño de la  generacion es mayor que 1 
	{
		cout << "Error: poblacion < 1\n";
		exit(1);
	}
	else if(prob_mutacion < 0 || prob_mutacion > 100) // chequeamos que el radio de mutaciones no sea estupido 
	{
		cout << "Error:  radio de mutacion debe estar en [0,100] \n";
		exit(1);
	}
	this->graph = graph;
	this->tam_poblacion = tam_poblacion;
	this->poblacion_actual = 0;
	this->generaciones = generaciones;
	this->prob_mutacion = prob_mutacion;
	this->show = show;
  this->numcross=numcross;
  this->shows=shows;
}



// chequeamos si es una solucion valida (que los vertices esten unidos de lo contrario devuelve -1
int Genetic::Solval(vector<int>& solucion)
{
	int total_cost = 0;
	set<int> set_solucion;
	
	//chequeamos si no contiene elementos repetidos (por si acaso)
	for(int i = 0; i < graph->V; i++)
		set_solucion.insert(solucion[i]);
	
	if(set_solucion.size() != (unsigned)graph->V)
		return -1;

	// chequeo si las conecciones estan hechas
	for(int i = 0; i < graph->V; i++)
	{
		if(i + 1 <  graph->V)
		{
			int cost = graph->existsunion(solucion[i], solucion[i+1]);
			
			// chequeamos si hay solucion
			if(cost == -1)
				return -1;
			else
				total_cost += cost;
		}
		else
		{
			int cost = graph->existsunion(solucion[i], solucion[0]);
			
			// chequeamos si existe conección
			if(cost == -1)
				return -1;
			else
				total_cost += cost;
			break;
		}
	}
	return total_cost;
}


bool Genetic::Cromosoma(const vector<int> & v)
{
	// chequeamos si existe una poblacion
	for(vector<pair<vector<int>, int> >::iterator it=poblacion.begin(); it!=poblacion.end(); ++it)
	{
		const vector<int>& vec = (*it).first; // obtenemos el vector
		if(equal(v.begin(), v.end(), vec.begin())) // comparamos el vector
			return true;
	}
	return false;
}


void Genetic::poblacioninicial() // generamos la poblacion inicial
{
	vector<int> parent;
	
	// insertamos el indice inicial ()
	parent.push_back(graph->vertice_inicial);
	
	// creamos el padre 
	for(int i = 0; i < graph->V; i++)
	{
		if(i != graph->vertice_inicial)
			parent.push_back(i);
	}
		
	int total_cost = Solval(parent);
	
	if(total_cost != -1) // chequeamos si el padre es valido
	{
		poblacion.push_back(make_pair(parent, total_cost)); // insertamos la poblacion 
		poblacion_actual++; // incrementamos el tamaño de la poblacion
	}
	
	// cramos permutaciones alaeatirio "generation" veces
	for(int i = 0; i < generaciones; i++) //Porque no poner un while? pq no sabemos si hay suficientes soluciones factibles como para rellenar las soluciones iniciales
	{
		// permutacion aleatorio, funcion shuffle es de ayuda
		random_shuffle(parent.begin() + 1, parent.begin() + (rand() % (graph->V - 1) + 1));
		
		int total_cost = Solval(parent); // chequeamos si la solucion es valida
		
		// chequeamos si la permutacion es valida y si la solucion no esta
		if(total_cost != -1 && !Cromosoma(parent))
		{
		poblacion.push_back(make_pair(parent, total_cost)); // agrega en la solucion iniciaul
			poblacion_actual++; // incrementamos la poblacion actual
		}
		if(poblacion_actual == tam_poblacion) // chequamos el tamaño de la poblacion, si tenemos los necesarios breakeamos
			break;
	}
	
	//chequea si la poblacion inicial es  0
	if(poblacion_actual == 0)
		cout << "\nPoblacion inicial vacia :( intente  correr denuevo el algoritmo ...";
	else
		sort(poblacion.begin(), poblacion.end(), sort_pred()); // ordena la poblacion
}


void Genetic::showP()
{
	cout << "\nMuestra las soluciones...\n\n";
	for(vector<pair<vector<int>, int> >::iterator it=poblacion.begin(); it!=poblacion.end(); ++it)
	{
		const vector<int>& vec = (*it).first; // obtenemos el vector
		
		for(int i = 0; i < graph->V; i++)
			cout << vec[i] << " ";
		cout << graph->vertice_inicial;
		cout << "\n Costo: " << (*it).second << "\n\n";
	}
	cout << "\nTamaño Poblacion: " << poblacion_actual << endl;
}


// Insetamos el vector usando binary search
void Genetic::Binary(vector<int>& child, int total_cost)
{
	int imin = 0;
	int imax = poblacion_actual - 1;
	
	while(imax >= imin)
	{
		int imid = imin + (imax - imin) / 2;
		
		if(total_cost == poblacion[imid].second)
		{
			poblacion.insert(poblacion.begin() + imid, make_pair(child, total_cost));
			return;
		}
		else if(total_cost > poblacion[imid].second)
			imin = imid + 1;
		else
			imax = imid - 1;
	}
	poblacion.insert(poblacion.begin() + imin, make_pair(child, total_cost));
}



void Genetic::crossOver(vector<int>& parent1, vector<int>& parent2)
{
	vector<int> child1, child2;
	
	// mapa  genes, chequamos si ya fueron seleccionad0s
	map<int, int> genes1, genes2;
	
	for(int i = 0; i < graph->V; i++)
	{
		// inicialmente los genes no fueron usados
		genes1[parent1[i]] = 0;
		genes2[parent2[i]] = 0;
	}
	
	// generamos puntos aleatorios
	int point1 = rand() % (graph->V - 1) + 1;
	int point2 = rand() % (graph->V - point1) + point1;
	
	// ajustamos los puntos si son iguales
	if(point1 == point2)
	{
		if(point1 - 1 > 1)
			point1--;
		else if(point2 + 1 < graph->V)
			point2++;
		else
		{
			// point1 o point2 ?? lo escogemos aleatorio...
			int point = rand() % 10 + 1; // numero entre 1 a 10
			if(point <= 5)
				point1--;
			else
				point2++;
		}
	}
	
	// generamos hijos
	
	//hasta el punto 1, el niño 1 recibe genes del padre 1
  // y child2 recibe genes del parent2
	for(int i = 0; i < point1; i++)
	{
		// agrega genes
		child1.push_back(parent1[i]);
		child2.push_back(parent2[i]);
		// mmarca los genes usados 
		genes1[parent1[i]] = 1;
		genes2[parent2[i]] = 1;
	}
	
	// marca los genes restantes
	for(int i = point2 + 1; i < graph->V; i++)
	{
		genes1[parent1[i]] = 1;
		genes2[parent2[i]] = 1;
	}
		
	// aquí está la subcadena invertida
  // child1 recibe genes del parent2 y
	// child2 recibe genes del parent1
	for(int i = point2; i >= point1; i--)
	{
		if(genes1[parent2[i]] == 0) // si el gen no es usado 
		{
			child1.push_back(parent2[i]);
			genes1[parent2[i]] = 1; // marcamos elgen	
		}
		else
		{
			// si el gen ya se usa, elige el gen que no se usa
			for(map<int, int>::iterator it = genes1.begin(); it != genes1.end(); ++it)
			{
				if(it->second == 0) // comprueba si no se utiliza
				{
					child1.push_back(it->first);
					genes1[it->first] = 1; // se marca como usado
					break; // se deja el loop
				}
			}
		}
		//hacemos lo mismo pero para el gen 2
		if(genes2[parent1[i]] == 0) 
		{
			child2.push_back(parent1[i]);
			genes2[parent1[i]] = 1;
		}
		else
		{
			for(map<int, int>::iterator it = genes2.begin(); it != genes2.end(); ++it)
			{
				if(it->second == 0) 
				{
					child2.push_back(it->first);
					genes2[it->first] = 1; 
					break; 
				}
			}
		}
	}
	
	// genes remanentes: el niño1 recibe genes del padre1
// y child2 recibe genes del parent2
	for(int i = point2 + 1; i < graph->V; i++)
	{
		child1.push_back(parent1[i]);
		child2.push_back(parent2[i]);
	}
		
	// mutaciones 
	int mutation = rand() % 100 + 1; // numero aleatorio entre  [1,100]
	if(mutation <= prob_mutacion) // chequeamos si el numero aleatorio <= radio de mutacion 
	{
		// mutacion: cambio de dos genes 
		
		int index_gene1, index_gene2;
		index_gene1 = rand() % (graph->V - 1) + 1;
		index_gene2 = rand() % (graph->V - 1) + 1;
		
		// para el child 1
		int aux = child1[index_gene1];
		child1[index_gene1] = child1[index_gene2];
		child1[index_gene2] = aux;
		
		// para el  child2
		aux = child2[index_gene1];
		child2[index_gene1] = child2[index_gene2];
		child2[index_gene2] = aux;
	}
	
	int dist_total_child1 = Solval(child1);
	int dist_total_child2 = Solval(child2);
	
    // comprueba si es una solución válida y no existe en la población
	if(dist_total_child1 != -1 && !Cromosoma(child1))
	{
		// agrega un niño en la población
		Binary(child1, dist_total_child1); // binary search para insertar
		poblacion_actual++; // incrementar la poblacion actual
	}
	
	// chequeamos otra vez.
	if(dist_total_child2 != -1 && !Cromosoma(child2))
	{
		// agregamos un child en la poblacion
		Binary(child2, dist_total_child2); // binary search para incertar
		poblacion_actual++; // incrementar la poblacion actual
	}
}


// corre el algoritmo genetico 
void Genetic::run()
{  
  
	poblacioninicial(); // crea la poblacion inicial
	if(show == true)
		showP(); // muestra la poblacion
	if(poblacion_actual == 0)
		return;
  string nombreArchivo = "datos.txt";
  ofstream archivo;
// Abrimos el archivo
  archivo.open(nombreArchivo.c_str(), fstream::out);
	for(int i = 0; i < generaciones; i++)
	{
		int  old_size_population = poblacion_actual;
		
		/* selecciona a dos padres (si existen) que participarán
del proceso de reproducción */
    for (int j=1; j<numcross;j++){
		if(poblacion_actual >= 2)
		{	
			if(poblacion_actual == 2)
			{
				// aplicando crossover en los padres
				crossOver(poblacion[0].first, poblacion[1].first);
			}
			else
			{
				// real_size_population > 2
				
				int parent1, parent2;
			
				do
				{
					// seleccionar dos padres al azar
					parent1 = rand() % poblacion_actual;
					parent2 = rand() % poblacion_actual;
				}while(parent1 == parent2);
				
				// Aplicar cruce en los dos padres(m
				crossOver(poblacion[parent1].first, poblacion[parent2].first);
			}}
      	else // poblacion contiene solamente 1 padre 
		{
			// aplicar crossover con los dos padres 
			crossOver(poblacion[0].first, poblacion[0].first);
			
			if(poblacion_actual > tam_poblacion)
			{
				poblacion.pop_back();// elimina al peor padre de la población(
				poblacion_actual--; // disminuya la poblacion actual
			}
			
			
// obtiene la diferencia para comprobar si la población creció 
			
		} 
		}
    int dif_poblacion = poblacion_actual - tam_poblacion;
			
      while(dif_poblacion>0){
				if(dif_poblacion > 1)
				{
					
					poblacion.pop_back(); //Intente con pop y no me funciono asi que use popback
					poblacion.pop_back();// elimina a los dos peores padres de la población); 
          dif_poblacion=dif_poblacion-2;
					
					
// disminuye la población de tamaño real en 2 unidades
					poblacion_actual -= 2;
				}
			else if(dif_poblacion == 1)
			{
				if(poblacion_actual > tam_poblacion)
				{
					poblacion.pop_back(); // elimina al peor padre de la población(
					poblacion_actual--; //disminuye la poblacion actual
				}
        dif_poblacion=dif_poblacion-1;
			}
    
      }
    if (i==generaciones-1){
       archivo << poblacion[0].second;}
    else{
      archivo << poblacion[0].second<<endl;
    }
    if (shows){
    cout << "\nIteración "<< i <<" Costo: " << poblacion[0].second;} 
	}
	archivo.close();
	if(show == true)
		showP(); // muestra la poblacion
	
	cout << "\nMejor solucion: ";
	const vector<int>& vec = poblacion[0].first;
	for(int i = 0; i < graph->V; i++)
		cout << vec[i] << " ";
	cout << graph->vertice_inicial;
	cout << "\n Costo: " << poblacion[0].second;
}


int Genetic::BestSol()
{
	if(poblacion_actual > 0)
		return poblacion[0].second;
	return -1;
}