#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>

typedef struct nodo
{
	int fila;
	int columna;
	int hilo;
	struct nodo *sig;
}Prioridad;

static Prioridad * cabeza= NULL;
static Prioridad * fondo= NULL;

static int *a= NULL;
static int *b= NULL;
static int *r= NULL;

static int f1, f2, c1, c2;
void insertar(int fila, int columna, int hilo);
void extraer();
void inicializarMatriz(int f, int c, int m[f][c]);
void imprimirMatriz(int f, int c, int m[f][c], int n);
void multiplicarMatrices(Prioridad * p);

sem_t semilla;

int main(int argc, char const *argv[])
{
	srand (time(NULL));
	f1= atoi(argv[1]); c1= atoi(argv[2]); 
	f2= atoi(argv[3]); c2= atoi(argv[4]);
	int i,j,k=0;
	int mat1[f1][c1];
	int mat2[f2][c2];
	int matR[f1][c2];
	pthread_t hilos[f1*c2];
	sem_init(&semilla,0,f1*c2-1);

	for (i = 0; i < f1; ++i)
	{
		for (j = 0; j < c2; ++j)
		{
			matR[i][j]= 0;
		}
		
	}

	inicializarMatriz(f1,c1,mat1);
	inicializarMatriz(f2,c2,mat2);
	imprimirMatriz(f1,c1,mat1,1);
	imprimirMatriz(f2,c2,mat2,2);

	a= &mat1[0][0];
	b= &mat2[0][0];
	r= &matR[0][0];
	Prioridad arr[f1*c2];
	
	k=0;
	for (i = 0; i < f1; ++i)
	{
		for (j = 0; j < c2; ++j)
		{
			arr[k].fila= i; 
			arr[k].columna= j; 
			arr[k].hilo= k+1;
			insertar(i,j,k);
			//pthread_create(&hilos[k],NULL,(void *) &multiplicarMatrices, (void *)&arr[k]);
			k++;
		}
	}
	k=0;
	do{
		pthread_create(&hilos[k],NULL,(void *) &multiplicarMatrices, (void *)&arr[k]);
		//pthread_join(hilos[k],NULL);
		k++;
	}while(k<(f1*c2));
	k=0;
	do{
		pthread_join(hilos[k],NULL);
		k++;
	}while(k<(f1*c2));
	printf("\n");
	imprimirMatriz(f1,c2,matR,3);
	return 0;
}


void multiplicarMatrices(Prioridad * p){
	int f= p->fila, c= p->columna, n= p->hilo;
	int i;
	for (i = 0; i < c1; ++i)
	{
		//(*(r+f*c2+c)) = (*(r+f*c2+c)) + ((*(a+f*c1+i)) * (*(b+i*c2+c)));
		r[f*c2+c]= r[f*c2+c] + (a[f*c1+i]*b[i*c2+c]);
	}
	//printf("\n");
	do{
		sem_wait(&semilla);
		if (cabeza == NULL)
		{
			sem_post(&semilla);
			break;
		}
		if (cabeza!= NULL && cabeza->fila == f && cabeza->columna == c)
		{
			printf("Hilo %d: ", n);
			for (i = 0; i < c1; ++i)
			{
				if (i == c1-1)
					//printf("%d * %d = %d", (*(a+f*c1+i)), (*(b+i*c2+c)), (*(r+f*c2+c)));
					printf("(%d x %d) = %d", a[f*c1+i], b[i*c2+c], r[f*c2+c]);
				else
					//printf("%d * %d + ", (*(a+f*c1+i)), (*(b+i*c2+c)));
					printf("(%d x %d) + ", a[f*c1+i], b[i*c2+c]);

			}
			printf("\n");
			extraer();
			sem_post(&semilla);
		}else{
			sem_post(&semilla);
		}
	}while(cabeza!= NULL);
}
void insertar(int fila, int columna, int hilo){	
    Prioridad *nuevo;
    nuevo=malloc(sizeof(Prioridad));
    nuevo->fila=fila;
    nuevo->columna= columna;
    nuevo->hilo= hilo;
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
void extraer(){
    Prioridad *borrar = cabeza;
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
void inicializarMatriz(int f, int c, int m[f][c]){
	int i,j;
	for (i = 0; i < f; ++i)
	{
		for (j = 0; j < c; ++j)
		{
			m[i][j]= -10 + rand() % 21;
		}
		
	}
	printf("\n");
}
void imprimirMatriz(int f, int c, int m[f][c], int n){
	int i,j;
	printf("matriz %d =>", n);
	for (i = 0; i < f; ++i)
	{
		if (i==0)
			printf("\t");
		else
			printf("\t\t");
		for (j = 0; j < c; ++j)
		{
			printf("%d\t", m[i][j]);
		}
		printf("\n");
		
	}
	printf("\n");
}