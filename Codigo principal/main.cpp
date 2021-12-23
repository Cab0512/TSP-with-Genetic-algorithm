#include <iostream>
#include "tsp.h"
using namespace std;

int main()
{
	srand(time(NULL)); // para los numeros aleatorios

	Graph * graph1 = new Graph(5, 0);
	// grafo de prueba
	graph1->addEdge(0, 1, 1);
	graph1->addEdge(1, 0, 1);
	graph1->addEdge(0, 2, 3);
	graph1->addEdge(2, 0, 3);
	graph1->addEdge(0, 3, 4);
	graph1->addEdge(3, 0, 4);
	graph1->addEdge(0, 4, 5);
	graph1->addEdge(4, 0, 5);
	graph1->addEdge(1, 2, 1);
	graph1->addEdge(2, 1, 1);
	graph1->addEdge(1, 3, 4);
	graph1->addEdge(3, 1, 4);
	graph1->addEdge(1, 4, 8);
	graph1->addEdge(4, 1, 8);
	graph1->addEdge(2, 3, 5);
	graph1->addEdge(3, 2, 5);
	graph1->addEdge(2, 4, 1);
	graph1->addEdge(4, 2, 1);
	graph1->addEdge(3, 4, 2);
	graph1->addEdge(4, 3, 2);
	
	
	

	// creamos otro grafo de prueba pero esta vez aleatorio
	Graph * graph4 = new Graph(20, 0, true);
	graph4->showInfoGraph();
	graph4->showGraph();
	// parameters: grafo, tamaño generacion, cantidad de generaciones , radio de mutacion y hijos
	// parametro opcional: true para ver generacion 
	Genetic genetic(graph4, 100, 1000, 10, 10, false);

	const clock_t begin_time = clock(); // medir el tiempo
	genetic.run(); // ejecutar el algoritmo
	cout << "\n\nTiempo que demoro en correr el algoritmo: " << float(clock () - begin_time) /  CLOCKS_PER_SEC << " segundos."; // muestra el tiempo en segundos
	
	return 0;
}