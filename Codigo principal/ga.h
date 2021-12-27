#ifndef GA_H
#define GA_H

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
	int uniones; // total de uniones
	int vertice_inicial; // vertice inicial
	std::map<std::pair<int, int>, int> mapa_uniones; // mapa de las uniones 
public:
	Graph(int V, int vertice_inicial, bool randomg = false); // constructor
	void addunion(int v1, int v2, int distancia); // añade una union 
	void showG(); // muestra las uniones de un grafo
	void generaG(); // genera un grafo aleatorio
	void informacionG(); // muestra informacion del grafo
	int existsunion(int inicio, int fin); // chequea si existe una union 
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

//Para eso me base en el libro: Genetic Algorithms With Python, el cual explica como utilizar el algoritmo genetico 

class Genetic
{
private:
	Graph* graph; // el grafo
	std::vector< my_pair > poblacion; // cada elemento es un par: vector y costo total
	int tam_poblacion; // tamaño de poblacion
	int poblacion_actual; // valor real poblacion (eliminando las anomalias)
	int generaciones; // cantidad de generaciones
	int prob_mutacion; // radio de mutacion, probabilidad de mutacion
  int numcross;  //n°cross
	bool shows; // bandera para mostrar la distancia total por iteracion 
  bool show; //mostrar poblacion inicial y final
private:
	void poblacioninicial(); // genera la poblacion inicial 
public:
	Genetic(Graph* graph, int tam_poblacion, int generaciones, int prob_mutacion, int numcross, bool show = true,bool shows=true); // constructor
	int Solval(std::vector<int>& solution); // chequea si la solucion es valida 
	void showP(); // muestra la poblacion
	void crossOver(std::vector<int>& parent1, std::vector<int>& parent2); // hace el crossOver o cruza ademas de la mutación
	void Binary(std::vector<int>& child, int total_cost); // usa busqueda binary para insertar
	void run(); // corre el algoritmo genetico 
	int BestSol(); // retorna la mejor solucion 
	bool Cromosoma(const std::vector<int> & v); // chequea si existe el cromosoma 
};

#endif