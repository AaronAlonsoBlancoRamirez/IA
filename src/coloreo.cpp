#include <iostream>
#include <vector>
#include <list>
#include <cstdlib>
#include <ctime>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Grafo {
private:
    int nodos;
    std::vector<std::list<int>> adj;

public:
    Grafo(int nodos) : nodos(nodos) {
        adj.resize(nodos);
    }

    void agregarArista(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    const std::list<int>& obtenerAdyacentes(int u) const {
        return adj[u];
    }

    int obtenerNumeroDeNodos() const {
        return nodos;
    }
};

// 10 colores en formato RGB
const float colores[][3] = {
    {1.0f, 0.0f, 0.0f},  // Rojo
    {0.0f, 1.0f, 0.0f},  // Verde
    {0.0f, 0.0f, 1.0f},  // Azul
    {1.0f, 1.0f, 0.0f},  // Amarillo
    {1.0f, 0.0f, 1.0f},  // Magenta
    {0.0f, 1.0f, 1.0f},  // Cian
    {0.5f, 0.5f, 0.5f},  // Gris
    {0.0f, 0.5f, 0.0f},  // Verde oscuro
    {0.5f, 0.0f, 0.5f},  // Púrpura
    {0.0f, 0.0f, 0.5f}   // Azul oscuro
};

Grafo* g;
std::vector<int> coloresAsignadosGlobal;
std::vector<float> posicionesX, posicionesY;

int seleccionarNodoMasRestrictivo(const Grafo& grafo) {
    int nodoSeleccionado = -1, maxRestricciones = -1;

    for (int i = 0; i < grafo.obtenerNumeroDeNodos(); i++) {
        if (coloresAsignadosGlobal[i] == -1) {
            int restricciones = 0;
            for (int vecino : grafo.obtenerAdyacentes(i)) {
                if (coloresAsignadosGlobal[vecino] != -1) restricciones++;
            }
            if (restricciones > maxRestricciones) {
                maxRestricciones = restricciones;
                nodoSeleccionado = i;
            }
        }
    }
    return nodoSeleccionado;
}


int seleccionarNodoMasRestringido(const Grafo& grafo) {
    int nodoSeleccionado = -1, minValoresDisponibles = 11; 

    for (int i = 0; i < grafo.obtenerNumeroDeNodos(); i++) {
        if (coloresAsignadosGlobal[i] == -1) {
            std::vector<bool> coloresDisponibles(10, true); 
            for (int vecino : grafo.obtenerAdyacentes(i)) {
                if (coloresAsignadosGlobal[vecino] != -1) {
                    coloresDisponibles[coloresAsignadosGlobal[vecino]] = false;
                }
            }

            int valoresDisponibles = 0;
            for (bool disponible : coloresDisponibles) {
                if (disponible) valoresDisponibles++;
            }

            if (valoresDisponibles < minValoresDisponibles) {
                minValoresDisponibles = valoresDisponibles;
                nodoSeleccionado = i;
            }
        }
    }
    return nodoSeleccionado;
}


bool colorearGrafoMasRestringido(Grafo& grafo) {
    int maximoColor = 0;  // Para rastrear el color máximo utilizado

    while (true) {
        int nodo = seleccionarNodoMasRestringido(grafo);
        if (nodo == -1) break;  // No quedan nodos sin color, termina

        std::cout << "Nodo seleccionado (MasRestringido): " << nodo << std::endl;

        for (int color = 0; color < 10; color++) {
            bool seguro = true;
            for (int vecino : grafo.obtenerAdyacentes(nodo)) {
                if (coloresAsignadosGlobal[vecino] == color) {
                    seguro = false;
                    break;
                }
            }

            if (seguro) {
                coloresAsignadosGlobal[nodo] = color;
                maximoColor = std::max(maximoColor, color);  // Actualiza el color máximo utilizado
                break;
            }
        }
    }

    // Imprimir los colores asignados
    std::cout << "Colores asignados (MasRestringido):" << std::endl;
    for (int i = 0; i < grafo.obtenerNumeroDeNodos(); i++) {
        std::cout << "Nodo " << i << ": Color " << coloresAsignadosGlobal[i] + 1 << std::endl;
    }

    std::cout << "Número máximo de colores utilizados: " << maximoColor + 1 << std::endl;

    return true;
}

void colorearGrafoMasRestrictivo(Grafo& grafo) {
    int maximoColor = 0;  // Para rastrear el color máximo utilizado
    int nodos = grafo.obtenerNumeroDeNodos();
    
    for (int i = 0; i < nodos; i++) {
        // Selecciona el nodo más restrictivo
        int nodo = seleccionarNodoMasRestrictivo(grafo);
        
        if (nodo == -1) break;  // Si no queda ningún nodo sin color, termina
        
        bool coloresDisponibles[10] = { true, true, true, true, true, true, true, true, true, true };
        
        // Marca los colores usados por los nodos adyacentes
        for (int vecino : grafo.obtenerAdyacentes(nodo)) {
            if (coloresAsignadosGlobal[vecino] != -1) {
                coloresDisponibles[coloresAsignadosGlobal[vecino]] = false;
            }
        }
        
        // Asigna el primer color disponible
        for (int color = 0; color < 10; color++) {
            if (coloresDisponibles[color]) {
                coloresAsignadosGlobal[nodo] = color;
                maximoColor = std::max(maximoColor, color);  // Actualiza el color máximo utilizado
                break;
            }
        }
    }

    // Imprimir los colores asignados
    std::cout << "Colores asignados (MasRestrictivo):" << std::endl;
    for (int i = 0; i < nodos; i++) {
        std::cout << "Nodo " << i << ": Color " << coloresAsignadosGlobal[i] + 1 << std::endl;
    }

    std::cout << "Número máximo de colores utilizados: " << maximoColor + 1 << std::endl;
}


void dibujarGrafo() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Dibuja los nodos del grafo
    for (int i = 0; i < g->obtenerNumeroDeNodos(); i++) {
        glColor3f(colores[coloresAsignadosGlobal[i]][0], colores[coloresAsignadosGlobal[i]][1], colores[coloresAsignadosGlobal[i]][2]);
        glBegin(GL_QUADS);
        glVertex2f(posicionesX[i] - 0.01f, posicionesY[i] - 0.01f);
        glVertex2f(posicionesX[i] + 0.01f, posicionesY[i] - 0.01f);
        glVertex2f(posicionesX[i] + 0.01f, posicionesY[i] + 0.01f);
        glVertex2f(posicionesX[i] - 0.01f, posicionesY[i] + 0.01f);

        glEnd();
    }
    
    // Dibuja las aristas
    glColor3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < g->obtenerNumeroDeNodos(); i++) {
        for (int vecino : g->obtenerAdyacentes(i)) {
            glBegin(GL_LINES);
            glVertex2f(posicionesX[i], posicionesY[i]);
            glVertex2f(posicionesX[vecino], posicionesY[vecino]);
            glEnd();
        }
    }
}

void inicializarOpenGL() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
}

void iniciarGrafo(int nodos, int aristas) {
    g = new Grafo(nodos);
    
    posicionesX.resize(nodos);
    posicionesY.resize(nodos);
    srand(static_cast<unsigned int>(time(0)));
    for (int i = 0; i < nodos; i++) {
        posicionesX[i] = (rand() % 200 - 100) / 100.0f;
        posicionesY[i] = (rand() % 200 - 100) / 100.0f;
    }

    for (int i = 0; i < nodos; i++) {
        for (int j = 0; j < aristas; j++) {
            int vecino = rand() % nodos;
            if (vecino != i) {
                g->agregarArista(i, vecino);
            }
        }
    }

    std::cout << "Estructura del grafo:\n";
    for (int i = 0; i < nodos; i++) {
        std::cout << "Nodo " << i << ": ";
        for (int vecino : g->obtenerAdyacentes(i)) {
            std::cout << vecino << " ";
        }
        std::cout << "\n";
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
}

int main() {
    std::cout << "Ingrese el número de nodos: ";
    int nodos;
    std::cin >> nodos;

    std::cout << "Ingrese el número de aristas desde cada nodo: ";
    int aristas;
    std::cin >> aristas;

    // Inicializa el grafo
    coloresAsignadosGlobal.resize(nodos, -1);
    iniciarGrafo(nodos, aristas);

    if (!glfwInit()) {
        std::cerr << "Error al inicializar GLFW" << std::endl;
        return -1;
    }

    // Primera ventana para el primer algoritmo
    GLFWwindow* window1 = glfwCreateWindow(800, 800, "Coloreado Restrictivo", nullptr, nullptr);
    if (!window1) {
        std::cerr << "Error al crear la primera ventana" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window1);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Error al inicializar GLAD en la primera ventana" << std::endl;
        return -1;
    }

    inicializarOpenGL();
    glfwSetFramebufferSizeCallback(window1, framebuffer_size_callback);

    // Colorear con el primer algoritmo (colorearGrafoMasRestrictivo)
    std::fill(coloresAsignadosGlobal.begin(), coloresAsignadosGlobal.end(), -1);  // Asegura que el vector esté reiniciado
    colorearGrafoMasRestrictivo(*g);

    // Mostrar el grafo coloreado con el primer algoritmo
    while (!glfwWindowShouldClose(window1)) {
        glClear(GL_COLOR_BUFFER_BIT);
        dibujarGrafo();
        glfwSwapBuffers(window1);
        glfwPollEvents();
    }

    // Cierra la primera ventana
    glfwDestroyWindow(window1);

    // Segunda ventana para el segundo algoritmo
    GLFWwindow* window2 = glfwCreateWindow(800, 800, "Coloreado Restringido", nullptr, nullptr);
    if (!window2) {
        std::cerr << "Error al crear la segunda ventana" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window2);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Error al inicializar GLAD en la segunda ventana" << std::endl;
        return -1;
    }

    inicializarOpenGL();
    glfwSetFramebufferSizeCallback(window2, framebuffer_size_callback);

    // Reiniciar colores antes del segundo algoritmo
    std::fill(coloresAsignadosGlobal.begin(), coloresAsignadosGlobal.end(), -1);  // Reinicia los colores para el segundo algoritmo

    // Colorear con el segundo algoritmo (colorearGrafoMasRestringido)
    colorearGrafoMasRestringido(*g);

    // Mostrar el grafo coloreado con el segundo algoritmo
    while (!glfwWindowShouldClose(window2)) {
        glClear(GL_COLOR_BUFFER_BIT);
        dibujarGrafo();
        glfwSwapBuffers(window2);
        glfwPollEvents();
    }

    // Cierra la segunda ventana
    glfwDestroyWindow(window2);
    glfwTerminate();

    delete g;
    return 0;
}
