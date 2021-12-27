#include <iostream>
#include "ga.h"
using namespace std;

int main()
{
	srand(time(NULL)); // para los numeros aleatorios
  
  // creamos grafo de prueba  aleatorio
	Graph * g4 = new Graph(20, 0, true);
	g4->informacionG();
	// parameters: grafo, tamaño generacion, cantidad de generaciones , radio de mutacion y hijos
	// parametro opcional: true para ver generacion 
	//si el algoritmo se demora mucho bajar la cantidad de hijos. 
  Genetic genetic(g4, 100, 1000, 10, 10, true,false);
 //True si quiere ver la ejecucion del algoritmo y la poblacion final

	const clock_t begin_time = clock(); // medir el tiempo
	genetic.run(); // ejecutar el algoritmo
	cout << "\n\nTiempo que demoro en correr el algoritmo: " << float(clock () - begin_time) /  CLOCKS_PER_SEC << " segundos."; // muestra el tiempo en segundos
	
	return 0;
}