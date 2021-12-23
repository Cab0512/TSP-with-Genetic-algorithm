#ifndef TSP_H
#define TSP_H

#include <vector>
#include <map>
#include <set>
#include <utility> // pair
#include <time.h> // time
#include <stdlib.h> // srand, rand 

// 
// clase para representar el grafo
class Graph
{
private:
	int V; // numero de vertices 
	int total_edges; // total de uniones
	int initial_vertex; // vertice inicial
	std::map<std::pair<int, int>, int> map_edges; // mapa de las uniones 
public:
	Graph(int V, int initial_vertex, bool random_graph = false); // constructor
	void addEdge(int v1, int v2, int weight); // añade una union 
	void showGraph(); // muestra las uniones de un grafo
	void generatesGraph(); // genera un grafo aleatorio
	void showInfoGraph(); // muestra informacion del grafo
	int existsEdge(int src, int dest); // chequea si existe una union 
	friend class Genetic; // para el algoritmo genetico 
};

typedef std::pair<std::vector<int>, int> my_pair;


// ordenamos el vector con la funcion par
struct sort_pred
{
	bool operator()(const my_pair& firstElem, const my_pair& secondElem)
	{
		return firstElem.second < secondElem.second;
	}
};

// clase para representar el algoritmo genetico
class Genetic
{
private:
	Graph* graph; // el grafo
	std::vector< my_pair > population; // cada elemento es un par: vector y costo total
	int size_population; // tamaño de poblacion
	int real_size_population; // valor real poblacion (eliminando las anomalias)
	int generations; // cantidad de generaciones
	int mutation_rate; // radio de mutacion
  int children;  //childrens
	bool show_population; // bandera para mostrar poblacion 
private:
	void initialPopulation(); // genera la poblacion inicial 
public:
	Genetic(Graph* graph, int amount_population, int generations, int mutation_rate,int children, bool show_population = true); // constructor
	int isValidSolution(std::vector<int>& solution); // chequea si la solucion es valida 
	void showPopulation(); // muestra la poblacion
	void crossOver(std::vector<int>& parent1, std::vector<int>& parent2); // hace el crossOver o cruza ademas de la mutación
	void insertBinarySearch(std::vector<int>& child, int total_cost); // usa busqueda binary para insertar
	void run(); // corre el algoritmo genetico 
	int getCostBestSolution(); // retorna la mejor solucion 
	bool existsChromosome(const std::vector<int> & v); // chequea si existe el cromosoma 
};

#endif