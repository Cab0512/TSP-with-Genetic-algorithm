#include <iostream>
#include <fstream>
#include <algorithm> // funciones sort, next_permutation
#include "tsp.h"
using namespace std;


Graph::Graph(int V, int initial_vertex, bool random_graph) // constructor 
{
	if(V < 1) // chequemos si la cantdad de vertices es menor que 1
	{
		cout << "Error: numero de vertices <= 0\n";
		exit(1);
	}
	
	this->V = V; // numero de vertices
	this->initial_vertex = initial_vertex; // vertice inicial
	this->total_edges = 0; // total de  uniones, 0 inicialmente
	
	if(random_graph)
		generatesGraph();
}


void Graph::generatesGraph()
{
	vector<int> vec;
	
	// creamos el vector
	for(int i = 0; i < V; i++)
		vec.push_back(i);
	
	// generamos una permutacion aleatoria 
	random_shuffle(vec.begin(), vec.end());
	
	initial_vertex = vec[0]; // vertice inicial 
	
	int i, weight;
	for(i = 0; i <= V; i++)
	{
		weight = rand() % (10*V) + 1; // random weight (distancia) en [1,10*V]
    //se pude cambiar para otros valores deseados, como para distancias mas grandes o mas pequeñas
		
		if(i + 1 < V)
			addEdge(vec[i], vec[i + 1], weight);
		else
		{
			// agregamos el vertice final
			addEdge(vec[i], vec[0], weight);
			break;
		}
	}
	
	int limit_edges = V * (V - 1); // calculamis el limite de uniones
	int size_edges = rand() % (2 * limit_edges) + limit_edges;
	
	// agregamos uniones aleatorias
	for(int i = 0; i < size_edges; i++)
	{
		int src = rand() % V; // random inicio
		int dest = rand() % V; // random destino
		weight = rand() % V + 1; // random distancia en  [1,10*V]
		if(src != dest)
		{
			addEdge(vec[src], vec[dest], weight);
			addEdge(vec[dest], vec[src], weight);
		}
	}
}


void Graph::showInfoGraph()
{
	cout << "Informacion del grafico:\n\n";
	cout << "Numero de vertices: " << V;
	cout << "\nNumero de uniones: " << map_edges.size() << "\n";
}


void Graph::addEdge(int src, int dest, int weight) // agregar una nueva union
{
	map_edges[make_pair(src, dest)] = weight; // agregamos la union en el mapita
}


void Graph::showGraph() // muestra todas las uniones del grafo
{	
	map<pair<int, int>, int>::iterator it;
	for(it = map_edges.begin(); it != map_edges.end(); ++it)
		cout << it->first.first << " unido vertice  " << it->first.second << " con peso (distancia) " << it->second << endl;
}


int Graph::existsEdge(int src, int dest) // chequear si estan unidos
{
	map<pair<int, int>,int>::iterator it = map_edges.find(make_pair(src, dest));
	
	if(it != map_edges.end())
		return it->second; // retorna el costo 
	return -1;
}


// construye el algoritmo genetico
Genetic::Genetic(Graph* graph, int size_population, int generations, int mutation_rate, int children, bool show_population)
{
	if(size_population < 1) //ver si el tamaño de la  generacion es mayor que 1 
	{
		cout << "Error: poblacion < 1\n";
		exit(1);
	}
	else if(mutation_rate < 0 || mutation_rate > 100) // chequeamos que el radio de mutaciones no sea estupido 
	{
		cout << "Error:  radio de mutacion debe estar en [0,100] \n";
		exit(1);
	}
	this->graph = graph;
	this->size_population = size_population;
	this->real_size_population = 0;
	this->generations = generations;
	this->mutation_rate = mutation_rate;
	this->show_population = show_population;
  this->children=children;
}



// chequeamos si es una solucion valida (que los vertices esten unidos de lo contrario devuelve -1
int Genetic::isValidSolution(vector<int>& solution)
{
	int total_cost = 0;
	set<int> set_solution;
	
	//chequeamos si no contiene elementos repetidos (por si acaso)
	for(int i = 0; i < graph->V; i++)
		set_solution.insert(solution[i]);
	
	if(set_solution.size() != (unsigned)graph->V)
		return -1;

	// chequeo si las conecciones estan hechas
	for(int i = 0; i < graph->V; i++)
	{
		if(i + 1 <  graph->V)
		{
			int cost = graph->existsEdge(solution[i], solution[i+1]);
			
			// chequeamos si hay solucion
			if(cost == -1)
				return -1;
			else
				total_cost += cost;
		}
		else
		{
			int cost = graph->existsEdge(solution[i], solution[0]);
			
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


bool Genetic::existsChromosome(const vector<int> & v)
{
	// chequeamos si existe una poblacion
	for(vector<pair<vector<int>, int> >::iterator it=population.begin(); it!=population.end(); ++it)
	{
		const vector<int>& vec = (*it).first; // obtenemos el vector
		if(equal(v.begin(), v.end(), vec.begin())) // comparamos el vector
			return true;
	}
	return false;
}


void Genetic::initialPopulation() // generamos la poblacion inicial
{
	vector<int> parent;
	
	// insertamos el indice inicial ()
	parent.push_back(graph->initial_vertex);
	
	// creamos el padre 
	for(int i = 0; i < graph->V; i++)
	{
		if(i != graph->initial_vertex)
			parent.push_back(i);
	}
		
	int total_cost = isValidSolution(parent);
	
	if(total_cost != -1) // chequeamos si el padre es valido
	{
		population.push_back(make_pair(parent, total_cost)); // insertamos la poblacion 
		real_size_population++; // incrementamos el tamaño de la poblacion
	}
	
	// cramos permutaciones alaeatirio "generation" veces
	for(int i = 0; i < generations; i++)
	{
		// permutacion aleatorio, funcion shuffle es de ayuda
		random_shuffle(parent.begin() + 1, parent.begin() + (rand() % (graph->V - 1) + 1));
		
		int total_cost = isValidSolution(parent); // chequeamos si la solucion es valida
		
		// chequeamos si la permutacion es valida y si la solucion no esta
		if(total_cost != -1 && !existsChromosome(parent))
		{
			population.push_back(make_pair(parent, total_cost)); // agrega en la solucion iniciaul
			real_size_population++; // incrementamos el real size population
		}
		if(real_size_population == size_population) // chequamos el tamaño de la poblacion, si tenemos los necesarios breakeamos
			break;
	}
	
	//chequea si la poblacion inicial es  0
	if(real_size_population == 0)
		cout << "\nPoblacion inicial vacia :( intente  correr denuevo el algoritmo ...";
	else
		sort(population.begin(), population.end(), sort_pred()); // ordena la poblacion
}


void Genetic::showPopulation()
{
	cout << "\nMuestra las soluciones...\n\n";
	for(vector<pair<vector<int>, int> >::iterator it=population.begin(); it!=population.end(); ++it)
	{
		const vector<int>& vec = (*it).first; // obtenemos el vector
		
		for(int i = 0; i < graph->V; i++)
			cout << vec[i] << " ";
		cout << graph->initial_vertex;
		cout << "\n Costo: " << (*it).second << "\n\n";
	}
	cout << "\nTamaño Poblacion: " << real_size_population << endl;
}


// Insetamos el vector usando binary search
void Genetic::insertBinarySearch(vector<int>& child, int total_cost)
{
	int imin = 0;
	int imax = real_size_population - 1;
	
	while(imax >= imin)
	{
		int imid = imin + (imax - imin) / 2;
		
		if(total_cost == population[imid].second)
		{
			population.insert(population.begin() + imid, make_pair(child, total_cost));
			return;
		}
		else if(total_cost > population[imid].second)
			imin = imid + 1;
		else
			imax = imid - 1;
	}
	population.insert(population.begin() + imin, make_pair(child, total_cost));
}


/*
	crearemos la cruza
	este crossover genera dos puntos aleatorias
	Estos puntos generan subcadenas en ambos padres.
La subcadena invertida de parent1 se coloca en parent2 y
la subcadena invertida de parent2 se coloca en parent1
	
	ejemplo:
		parent1: 1 2 3 4 5
		parent2: 1 2 4 5 3
		
		substring en parent1: 2 3 4
		substring en parent2: 2 4 5
		
		substring invertido en parent1: 4 3 2
		substring invertido en parent2: 5 4 2
		
		child1: 1 5 4 2 5
		child2: 1 4 3 2 3
		
		Los hijos son invalidos : 5 aparece 2 veces en child1 2x y 3 aparace  2 veces en el hijo child2
		Solucion: mapear para ver si todos los genes estan en la solucion 
*/
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
	if(mutation <= mutation_rate) // chequeamos si el numero aleatorio <= radio de mutacion 
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
	
	int total_cost_child1 = isValidSolution(child1);
	int total_cost_child2 = isValidSolution(child2);
	
    // comprueba si es una solución válida y no existe en la población
	if(total_cost_child1 != -1 && !existsChromosome(child1))
	{
		// agrega un niño en la población
		insertBinarySearch(child1, total_cost_child1); // binary search para insertar
		real_size_population++; // incrementar real size population
	}
	
	// chequeamos otra vez.
	if(total_cost_child2 != -1 && !existsChromosome(child2))
	{
		// agregamos un child en la poblacion
		insertBinarySearch(child2, total_cost_child2); // binary search para incertar
		real_size_population++; // incrementar el real size population 
	}
}


// corre el algoritmo genetico 
void Genetic::run()
{  
  
	initialPopulation(); // crea la poblacion inicial
	if(show_population == true)
		showPopulation(); // muestra la poblacion
	if(real_size_population == 0)
		return;
  string nombreArchivo = "datos.txt";
  ofstream archivo;
// Abrimos el archivo
  archivo.open(nombreArchivo.c_str(), fstream::out);
	for(int i = 0; i < generations; i++)
	{
		int  old_size_population = real_size_population;
		
		/* selecciona a dos padres (si existen) que participarán
del proceso de reproducción */
    for (int j=1; j<children;j++){
		if(real_size_population >= 2)
		{	
			if(real_size_population == 2)
			{
				// aplicando crossover en los padres
				crossOver(population[0].first, population[1].first);
			}
			else
			{
				// real_size_population > 2
				
				int parent1, parent2;
			
				do
				{
					// seleccionar dos padres al azar
					parent1 = rand() % real_size_population;
					parent2 = rand() % real_size_population;
				}while(parent1 == parent2);
				
				// Aplicar cruce en los dos padres(m
				crossOver(population[parent1].first, population[parent2].first);
			}}
      	else // poblacion contiene solamente 1 padre 
		{
			// aplicar crossover con los dos padres 
			crossOver(population[0].first, population[0].first);
			
			if(real_size_population > size_population)
			{
				population.pop_back();// elimina al peor padre de la población(
				real_size_population--; // disminuya la real_size_population
			}
			
			
// obtiene la diferencia para comprobar si la población creció 
			int diff_population = real_size_population - size_population;
			
      while(diff_population>0){
			if(diff_population == 2)
			{
				if(real_size_population > size_population)
				{
					
					population.pop_back();
					population.pop_back();// elimina a los dos peores padres de la población);
          diff_population=diff_population-2;
					
					
// disminuye la población de tamaño real en 2 unidades
					real_size_population -= 2;
				}
			}
			else if(diff_population == 1)
			{
				if(real_size_population > size_population)
				{
					population.pop_back(); // elimina al peor padre de la población(
					real_size_population--; //disminuye el real_size poblacion
				}
        diff_population=diff_population-1;
			}
    
      }
		} 
		}
    if (i==generations-1){
       archivo << population[0].second;}
    else{
      archivo << population[0].second<<endl;
    }
    cout << "\nIteración "<< i <<" Costo: " << population[0].second; 
	}
	archivo.close();
	if(show_population == true)
		showPopulation(); // muestra la poblacion
	
	cout << "\nMejor solucion: ";
	const vector<int>& vec = population[0].first;
	for(int i = 0; i < graph->V; i++)
		cout << vec[i] << " ";
	cout << graph->initial_vertex;
	cout << "\n Costo: " << population[0].second;
}


int Genetic::getCostBestSolution()
{
	if(real_size_population > 0)
		return population[0].second;
	return -1;
}
