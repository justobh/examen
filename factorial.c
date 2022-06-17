#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>

//----COLA------

typedef struct nodo
{
	int valor;
	int pos;
	struct nodo *sig;
}nodoCola;

static nodoCola * cabeza= NULL;
static nodoCola * fondo= NULL;

//La cola se utiliza para poder saber qué valor debe imprimirse antes que otro.
//De esta forma la impresión queda por orden en que se ingresó y no por quien termine de calcular primero.

//----VECTOR-----

static char const **vectorOrden;

//El puntero apunta al vector argv[] para poder utilizarse de manera global.

//----SEMAFORO---

sem_t semilla;

//Se utiliza para la sincronización.



void proceso(int *orden);
void insertar(int valor, int pos);
void extraer();


int main(int argc, char const *argv[])
{
	int k,j,i;
	pthread_t hilo[argc];
	vectorOrden = argv;
	i=1;
	sem_init(&semilla,0,argc);

	int arr[argc]; //Almacena la posición de cada valor.
					//De ésta forma se protegen los valores de dichas posiciones y que no sufran cambios.

	do{
		arr[i]= i;
		pthread_create(&hilo[i],NULL,(void *) &proceso, (void *)&arr[i]);
		insertar(atoi(argv[i]),i);
		i++;
	}while(i<argc);
	i=1;
	do{
		pthread_join(hilo[i],NULL); 
		i++;
	}while(i<argc);
	return 0;
}


void proceso(int *orden){ //Funcion que realiza el factorial. 

	//Recibe la posición en la que se encuentra el valor en el vector argv[]

	int numero= atoi(vectorOrden[*orden]); //Utiliza la posición para transformar el string a int.

	int i;
	int fact = 1;
	for(i=1;i<=numero;i++){

		fact = fact * i;

	}
do{
	
	sem_wait(&semilla); //Bloquea a los demás de proseguir a la impresión.

	//Se revisa en la cola si al valor de "numero" le corresponde imprimir su factorial o no.

	if (cabeza->valor != numero && cabeza->pos != *orden)	//No es su turno. Debe dejar que los otros prosigan.
		
		sem_post(&semilla);

	else if(cabeza->valor == numero && cabeza->pos == *orden){	//Es su turno para imprimir su factorial. 
																//El valor sale de la cola y deja que los otros prosigan.
		printf("El Factorial de %d es %d\n",numero,fact);
		extraer();
		sem_post(&semilla);
		break;
	}
}while(1);

}

void insertar(int valor, int pos){	//Se inserta en la cola: 
									//El valor en int "valor" del string y la posicion "pos" en la esta en argv[]
    nodoCola *nuevo;
    nuevo=malloc(sizeof(nodoCola));
    nuevo->valor=valor;
    nuevo->pos= pos;
    nuevo->sig=NULL;

    if (cabeza == NULL)
    {
        cabeza = nuevo;
        fondo = nuevo;
    }
    else
    {
        fondo->sig = nuevo;
        fondo = nuevo;
    }
}
void extraer(){					//Se extrae al que se encuentra primero en la cola.
								//Dandole así paso al siguiente valor que debe imprimir.
    nodoCola *borrar = cabeza;
    if (cabeza == fondo)
    {
        cabeza = NULL;
        fondo = NULL;
    }
    else
    {
       	cabeza = cabeza->sig;
    }
    free(borrar);
}
