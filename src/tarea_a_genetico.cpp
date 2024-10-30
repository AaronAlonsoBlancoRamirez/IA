#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>

using namespace std;

// parámetros del algoritmo genético

int tam_genes = 30; //cantidad de genes
int tam_pop = 100; //cantidad de individuos de la población
int tam_torneo = 50; //tamaño del torneo
int generaciones = 200; //cantidad de generaciones
double prob_mut = 0.2; // probabilidad de mutación
double prob_cruz = 0.3; //probabilidad de cruces

//población
vector<vector<int> > poblacion;

void inicializarPoblacion()
{
    for (int i = 0; i<tam_pop; i++)
    {
        vector<int>individuo;
        
        for (int j = 0; j<tam_genes;j++)
        {
            int num = rand() % 2; // elige un número en el intervalo [0,1]
            individuo.push_back(num); //inserta en el vector de la población
        }
        poblacion.push_back(individuo); //inserta en el vector de la población
    }
}

void mostrarPoblacion()
{
    for (int i=0; i<tam_pop;i++)
    {
        for (int j=0; j<tam_genes; j++)
        cout<< poblacion[i][j] << " ";
        cout<< endl; 
    }
}

//retorna la puntuación del individuo
int obtenerPuntuacion(vector<int>individuo)
{
    
    //la puntuación es la suma de los valores de los genes
    int suma = 0;
    
    for (int i = 0; i < tam_genes; i++)
    suma += individuo[i];
    
    return suma;
    
}

//realiza la mutación
void mutacion(vector<int>& individuo)
{
    //elige un gen aleatoriamente en el intervalo [0,tam_genes - 1]
    int gen = rand() % tam_genes;
    
    //modifica el valor del gen elegido
    if (individuo[gen] == 0)
        individuo[gen] =1;
    else
        individuo[gen] = 0;
    
}

//realiza el cruce
void cruce(int indice_padre1, int indice_padre2, vector<int>& hijo)
{
    //elige un punto aleatoriamente en el intervalo [0, tam_genes - 1]
    int punto = rand() % tam_genes;
    
    for (int i = 0; i <= punto; i++)
        hijo.push_back(poblacion[indice_padre1][i]);
        
    for (int i = 0; i <= tam_genes; i++)
        hijo.push_back(poblacion[indice_padre2][i]);
    
}

//retorna el índice del mejor individuo de la población
int obtenerMejor()
{
    int indice_mejor = 0;
    int puntuacion_mejor = obtenerPuntuacion(poblacion[0]);
    
    for (int i = 1; i < tam_pop; i++)
    {
        int puntuacion = obtenerPuntuacion(poblacion[i]);
        if (puntuacion > puntuacion_mejor)
        {
            indice_mejor = i;
            puntuacion_mejor = puntuacion;
        }
    }
    
    return indice_mejor;
}

int main(int argc, char** argv) 
{
    srand(time(NULL));
    
    inicializarPoblacion();
    //mostrarPoblacion();
    
    for (int i = 0; i <generaciones; i++)
    {
        
        for (int j = 0; j < tam_torneo; j++)
        {
            //calcula la probabilidad de cruce
            double prob = ((double)rand() / (double)RAND_MAX+1);
            
            if (prob < prob_cruz)
            {
                //elige dos padres
                int indice_padre1 = rand() % tam_pop;
                int indice_padre2;
                
                //garantiza que los índices de los padres sean diferentes
                do
                {
                    indice_padre2 = rand() % tam_pop;
                } while (indice_padre1 == indice_padre2);
                
                vector<int> hijo;
                
                //aplica el cruce de 1 punto
                cruce(indice_padre1, indice_padre2, hijo);
                
                //calcula la probabilidad de mutación
                prob = ((double)rand() / (double)RAND_MAX + 1);
                
                if (prob < prob_mut)
                    mutacion(hijo);
                
                int puntuacion_padre = obtenerPuntuacion(poblacion[indice_padre1]);
                
                int puntuacion_hijo = obtenerPuntuacion(hijo);
                
                /*
                    si la puntuación del hijo es mejor que la del padre1,
                    entonces el padre1 es reemplazado por el hijo.
                */
                
                if (puntuacion_hijo > puntuacion_padre)
                {
                    //copia los genes del hijo al padre
                    for (int k = 0; k <tam_genes; k++)
                        poblacion[indice_padre1][k] = hijo[k];
                }
            }
        }
        
        cout<< "Generacion:  " << i + 1 << endl;
        cout<< "Mejor:  ";
        
        int indice_mejor = obtenerMejor();
        int puntuacion_mejor = obtenerPuntuacion(poblacion[indice_mejor]);
        
        for (int j = 0; j < tam_genes; j++)
            cout<< poblacion[indice_mejor][j]<< "  ";
            cout<< "\nPuntuacion:  " << puntuacion_mejor << "\n\n";
            
            //verifica si se encontró la solución óptima global
        if (puntuacion_mejor == tam_genes)
            break;
    }
    
    system("pause");
    return 0;
}
