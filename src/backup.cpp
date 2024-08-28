#include <iostream>
#include <iostream>
#include <vector>
#include <utility>
#include <random>
#include <set>
#include <algorithm>
#include <cmath>
#include <queue>

using namespace std;

struct Punto {
    int x, y;
    vector<pair<double,int>> knn_vecinos; //indices de los knn mas cercanos 
    double g;  // Costo acumulado hasta llegar a este nodo
    double h;  // Heurística (estimación del costo al objetivo)
    int padre; // Índice del nodo padre
};

// Función para calcular la distancia euclidiana entre dos puntos
double distancia_euclidiana(const Punto& p1, const Punto& p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

// Función para encontrar los k vecinos más cercanos de un punto dado
void knn(const vector<Punto>& puntos, Punto& query_point, int k) {
    vector<pair<double, int>> distancias;
    const double tolerancia = 1e-9;


    for (int i = 0; i < puntos.size(); ++i) {
    // Verificar si los puntos son prácticamente iguales
        if (abs(puntos[i].x - query_point.x) < tolerancia && abs(puntos[i].y - query_point.y) < tolerancia) {
            continue;
        }
    
        double distancia = distancia_euclidiana(puntos[i], query_point);
        distancias.push_back({distancia, i});
    }
    
    sort(distancias.begin(), distancias.end());


    vector<Punto> indices;

    for (int i = 0; i < k; ++i) {

       query_point.knn_vecinos.push_back({distancias[i].first,distancias[i].second});
       
        
    }

}

void knn_for_all_points( vector<Punto>& puntos, int k) {
    vector<vector<Punto>> vecinos_todos;

     //cout<<"llego hasta antes del knn"<<endl;

    for (int i = 0; i < puntos.size(); ++i) {
        knn(puntos, puntos[i], k);
        //vecinos_todos.push_back(vecinos);
    }
    //cout<<"llego hasta aqi"<<endl;
    //return vecinos_todos;
}

void imprimir_vecinos(const vector<Punto>& puntos,  int k) {
    for (int i = 0; i < puntos.size(); ++i) {
        cout << "Punto " << i + 1 << ": (" << puntos[i].x << ", " << puntos[i].y << ") - K vecinos más cercanos: ";
        for (int j = 0; j < k; ++j) {
            //cout << "(" << puntos[puntos[i].knn_vecinos[j].second].x << ", " << puntos[puntos[i].knn_vecinos[j].second].y << ") distancia -> "<< puntos[i].knn_vecinos[j].first;
            cout << "  (" << puntos[puntos[i].knn_vecinos[j].second].x << ", " << puntos[puntos[i].knn_vecinos[j].second].y << ") distancia -> "<< puntos[i].knn_vecinos[j].first;
        }
        cout << endl;
    }
}

// Función para seleccionar aleatoriamente m puntos de una matriz de puntos
vector<Punto> seleccionarPuntosAleatorios(const vector<vector<Punto>>& matriz, int m) {
    int n = matriz.size();
    vector<Punto> puntosAleatorios;

    // Verifica que m sea menor o igual al número total de puntos
    if (m > n * n) {
        cerr << "El valor de m es demasiado grande." << endl;
        return puntosAleatorios;
    }

    // Generador de números aleatorios
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, n * n - 1);

    // Conjunto para evitar seleccionar el mismo punto dos veces
    set<int> indicesSeleccionados;

    while (puntosAleatorios.size() < m) {
        int indice = dis(gen);
        if (indicesSeleccionados.count(indice) == 0) {
            int fila = indice / n;
            int columna = indice % n;
            puntosAleatorios.push_back(matriz[fila][columna]);
            indicesSeleccionados.insert(indice);
        }
    }

    return puntosAleatorios;
}

vector<vector<Punto>> crearMatrizPuntos(int n) {
    vector<vector<Punto>> matriz(n, vector<Punto>(n));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            matriz[i][j] = {i, j};
        }
    }

    return matriz;
}

/*
void printMatriz(const vector<vector<pair<int, int>>>& matriz) {
    int n = matriz.size();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << "(" << matriz[i][j].first << ", " << matriz[i][j].second << ") ";
        }
        cout << endl;
    }
}

void printPuntos(const vector<pair<int, int>>& matriz) {
    int n = matriz.size();
    for (int i = 0; i < n; ++i) {
            cout << "(" << matriz[i].first << ", " << matriz[i].second << ") ";
            cout <<endl;
    }
}
*/

void a_star_search(vector<Punto>& grafo, int inicio, int objetivo) {
    // Inicializar la cola de prioridad y el conjunto cerrado
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
    pq.push({0, inicio}); // (f(n), índice del nodo)
    set<int> closed_set;

    // ... (resto de la implementación del algoritmo A*)
}

double f(const Punto& nodo) {
    return nodo.g + nodo.h;
}






int main() {
    int n;
    int m;
    int k;

    cout << "Ingrese el tamaño de la matriz: ";
    cin >> n;

    vector<vector<Punto>> matrizPuntos = crearMatrizPuntos(n);
    cout << "Ingrese el numero de puntos aleatorios: ";
    cin >> m;

    vector<Punto> puntos_seleccionados = seleccionarPuntosAleatorios(matrizPuntos, m);

    cout << "Ingrese k para encontrar los vecinos mas cercanos: ";
    cin >> k;


    knn_for_all_points(puntos_seleccionados, k); 

    imprimir_vecinos(puntos_seleccionados, k);

    return 0;
}