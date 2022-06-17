#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX 20

//----ESTRUCTURA-----

typedef struct argumentos
{
	int empezar;
	int termina;
	int numHilo;
}datos;

// Se utiliza para almacenar en donde debe empezar y terminar la busqueda.
// También para almacenar a qué hilo le corresponde dicha busqueda entre los ya mencionados valores.

//-----VECTOR--------

static int v[MAX];			//Vector en el que se encuentran los valores aleatorios de 0 a 9

//-----VARIABLES-----

static int posEncontrada; 	//Variable global que almacena la posicion del elemento encontrado.
static int hiloEncontrado; 	//Variable global que almacena el hilo que encontró el valor.
static int valorbusq; 		//Variable global en el que almacena el valor a buscarse.
static int band=0;			//Variable global que se utiliza como bandera.
static int repartir;		//Variable global que almacena el valor para definir el rango de busqueda.

//----------------

void inicializarVector();		//Función que inicializa el vector, dando valores aleatorios entre 0 a 9 para cada posicion.
void busquedaSeq(datos * emp);	//Función que realiza la busqueda secuencial.

int main(int argc, char const *argv[])
{
	srand (time(NULL));
	int sum=0;
	
	int mitad=(int)(MAX/2);	//Se calcula y almacena la mitad del vector v[MAX]

	if (MAX%2 > 0) 			//De ser impar se redondea al valor más grande.
		mitad++;

	inicializarVector();
	int i, j=0;
	valorbusq= atoi(argv[1]);
	int cantHilos= atoi(argv[2]);
	datos ver[cantHilos];
	pthread_t hilo[cantHilos];

	if (valorbusq>9 || valorbusq<0 || cantHilos > MAX || cantHilos < 1)
	{
		perror("Error en argumentos");
		exit(1);
	}

	datos emp[cantHilos];	//Vector que almacena para cada hilo desde donde empezar y terminar la busqueda. 

	/*
		Se define la cantidad de elementos que corresponde a cada Hilo
		mediante la división entre el tamaño de v[MAX] y la cantidad de Hilos.

		- En caso de que la división sea exacta; los hilos recorrerán la misma cantidad de elementos
		- En caso contrario; se redondea al mayor numero y el ultimo hilo toma todo lo que falte.

	*/
	if (MAX%cantHilos==0)
		repartir= (int) (MAX/cantHilos);
	else
		repartir= ((int) (MAX/cantHilos))+1;
	for (i = 0; repartir*i < MAX; ++i)
	{
		emp[i].empezar= repartir*i;
		
		if (emp[i].empezar + repartir > MAX-1)
			emp[i].termina= MAX;
		else
			emp[i].termina= emp[i].empezar + repartir;
		emp[i].numHilo= i;
	}
	/*if (cantHilos>mitad)
	{
		if (emp[cantHilos-1].termina!= MAX)
			emp[cantHilos-1].termina= MAX;
	}*/
	
	/*
		En caso de que la cantidad de hilos este entre: 
			- La mitad del tamaño del vector y el tamaño del vector (MAX/2 < X < MAX).
		Entonces se deberá separar el valor del fin de un hilo para colocarlo en otro.
		Es decir tener ahora algunos hilos que tienen como rango de busqueda solo un valor simplemente y otros más que uno.
		Así cumplir con utilizar los hilos que se pida.

		Quedando de ésta forma: Usando MAX=20 y Cantidad de hilos requeridos= 11

		H1	H2	 H3		 H4		 H5		 H6		 H7		 H8		 H9		 H10	 H11
				_____	_____	____	_____	_____	_____	_____	_____	_____
		|	|	|	|	|	|	|	|	|	|	|	|	|	|	|	|	|	|	|	|
		0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0

		Ésto es debido a que apartir del valor mitad+1 hasta MAX-1 siempre se distribuyen los rangos
		como si se pidiera la cantidad de hilos MAX/2.
		Ej. Si se pide 19 hilos de un vector de 20. Agrupará los recorridos como si se pidiesen 10 hilos y no 19.
		
		 H1		 H2		 H3		 H4		 H5		 H6		 H7		 H8		 H9		 H10	H11 ... H19
		_____	_____	_____	____	_____	_____	_____	_____	_____	_____
		|	|	|	|	|	|	|	|	|	|	|	|	|	|	|	|	|	|	|	|	 /	 /	 /
		0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0


		Por lo que 1 hilo no lo usa. Ya que el valor de "repartir" en los valores [MAX/2;MAX-1] siempre da el mismo valor.

	*/

	if (cantHilos>mitad && cantHilos<MAX)
	{
		i=0;
		while(i < (cantHilos-mitad))
		{
			emp[i+mitad].empezar= emp[i].termina-1;
			emp[i+mitad].termina= emp[i].termina;
			emp[i].termina= emp[i].termina-1;
			emp[i+mitad].numHilo= emp[i].numHilo+1;
			//Se actualizan los valores de los hilos para que se ejecuten en el orden que corresponde.
			j = i+1;
			while (j < mitad)
			{
				emp[j].numHilo= emp[j].numHilo+1;
				j++;
			}
			i++;
		}
	}

	i=0;
	do{
		pthread_create(&hilo[emp[i].numHilo],NULL,(void *) &busquedaSeq, (void *) &emp[i]);
		for (int i = 0; i < cantHilos; ++i)
		{
			ver[emp[i].numHilo].empezar = emp[i].empezar;
			ver[emp[i].numHilo].termina = emp[i].termina;
			ver[emp[i].numHilo].numHilo = emp[i].numHilo;
		}
		i++;
	}while(i<cantHilos);
	printf("\n");
	for (i = 0; i < cantHilos; ++i)
	{
		printf("[%d,%d] ",ver[i].empezar,ver[i].termina);
	}
	printf("\n");
	i=0;
	do{
		pthread_join(hilo[i],NULL);
		i++;
	}while(i<cantHilos);

	if (band){
		printf("Posicion: %d\n", posEncontrada);
		printf("En el hilo: %d\n", hiloEncontrado);
	}else
		printf("No encontrada\n");
	return 0;
}

void busquedaSeq(datos * emp){
	int i;

	for (i = emp->empezar; i < emp->termina && band==0; ++i) //Se termina cuando la bandera es 1 o se termino de recorrer.
	{
		if (i<MAX)
		{
			if (v[i]==valorbusq) //Si se encuentra el valor, se guarda el valor y el numero de hilo. La bandera cambia.
			{
				posEncontrada= i;
				hiloEncontrado= emp->numHilo;

				band=1; 		//La bandera le indica a los demás hilos que se encontró el valor y tienen que terminar 
			}
		}else
			break;
	}
}

void inicializarVector(){
	int i;
	for (i = 0; i < MAX; ++i)
	{
		v[i]= rand() % 10;
		printf("%d ", v[i]);
	}
	printf("\n");
}