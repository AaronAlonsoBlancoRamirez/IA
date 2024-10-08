#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <utility>
#include <random>
#include <set>
#include <algorithm>
#include <cmath>
#include <queue>
#include <stack>

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

    for (int i = 0; i < puntos.size(); ++i) {
        knn(puntos, puntos[i], k);
        //vecinos_todos.push_back(vecinos);
    }
   
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

double f(const Punto& nodo) {
    return nodo.g + nodo.h;
}

double calcular_heuristica(const Punto& nodo, const Punto& objetivo) {
    return distancia_euclidiana(nodo, objetivo);
}

vector<int> reconstruct_path(const vector<int>& came_from, int current) {
    vector<int> path;
    while (current != -1) {
        path.push_back(current);
        current = came_from[current];
    }
    reverse(path.begin(), path.end());
    return path;
}

vector<int> a_star_search(vector<Punto>& grafo, int inicio, int objetivo, vector<int>& visitados) {
    // Inicializar g y h de todos los nodos
    for (auto& punto : grafo) {
        punto.g = numeric_limits<double>::infinity(); // Inicializa g a infinito
        punto.h = 0;  // Inicializa h a 0
        punto.padre = -1;
    }
    grafo[inicio].g = 0; // El nodo inicial tiene g = 0
    grafo[inicio].h = calcular_heuristica(grafo[inicio], grafo[objetivo]);

    // Cola de prioridad para explorar nodos en orden de menor f(n)
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
    pq.push({f(grafo[inicio]), inicio}); // (f(n), índice del nodo)

    set<int> closed_set; // Conjunto de nodos ya explorados
    vector<int> came_from(grafo.size(), -1); // Para reconstruir el camino final

    while (!pq.empty()) {
        auto current = pq.top();
        pq.pop();
        int current_index = current.second;

        // Añadir el nodo actual a los visitados
        visitados.push_back(current_index);

        // Si alcanzamos el objetivo, reconstruir el camino
        if (current_index == objetivo) {
            return reconstruct_path(came_from, current_index); // Camino final encontrado
        }

        closed_set.insert(current_index); // Marcar el nodo como explorado

        // Explorar los vecinos del nodo actual
        for (auto& vecino : grafo[current_index].knn_vecinos) {
            int vecino_index = vecino.second;
            double tentative_g_score = grafo[current_index].g + vecino.first;

            // Si el vecino ya ha sido explorado y no tiene mejor g, saltar
            if (closed_set.count(vecino_index) > 0 && tentative_g_score >= grafo[vecino_index].g) {
                continue;
            }

            // Si encontramos un camino mejor a este vecino, lo actualizamos
            if (tentative_g_score < grafo[vecino_index].g) {
                came_from[vecino_index] = current_index; // Guardar de dónde vino
                grafo[vecino_index].g = tentative_g_score; // Actualizar el costo g
                grafo[vecino_index].h = calcular_heuristica(grafo[vecino_index], grafo[objetivo]); // Heurística
                pq.push({f(grafo[vecino_index]), vecino_index}); // Añadir a la cola de prioridad
            }
        }
    }

    // Si no encontramos camino, devolver una lista vacía
    return {};
}

// Declaraciones de funciones
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// Función para graficar puntos y líneas
void graficar_puntos(const std::vector<Punto>& puntos, int windowWidth, int windowHeight,
                     int indiceInicio, int indiceFinal, const std::vector<int>& caminoIndices) {
    if (!glfwInit())
        return;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Grafico de Puntos", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create the window!" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to load OpenGL function pointers!" << std::endl;
        glfwTerminate();
        return;
    }

    // Configuración de OpenGL
    glPointSize(10.0f);  // Tamaño de los puntos

    // Configurar Shaders
    const char* vertexShaderSrc =
        "#version 330 core\n"
        "layout (location = 0) in vec2 aPos;\n"
        "out vec2 vertexPos;\n"
        "void main() {\n"
        "    gl_Position = vec4(aPos, 0.0f, 1.0f);\n"
        "    vertexPos = aPos;\n"
        "}\0";

    const char* fragmentShaderSrc =
        "#version 330 core\n"
        "in vec2 vertexPos;\n"
        "out vec4 fragColor;\n"
        "uniform vec4 color;\n"
        "void main() {\n"
        "    fragColor = color;\n"
        "}\0";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSrc, 0);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, 0, infoLog);
        std::cout << "Failed to compile the vertex shader! ERR: " << infoLog << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSrc, 0);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, 0, infoLog);
        std::cout << "Failed to compile the fragment shader! ERR: " << infoLog << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, 0, infoLog);
        std::cout << "Failed to link the shader program! ERR: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Crear el VBO para los puntos
    std::vector<float> vertices;
    std::vector<float> colores;
    for (size_t i = 0; i < puntos.size(); ++i) {
        float normX = (float)puntos[i].x / 999.0f * 2.0f - 1.0f;
        float normY = (float)puntos[i].y / 999.0f * 2.0f - 1.0f;
        vertices.push_back(normX);
        vertices.push_back(normY);

        // Colorear puntos especiales
        if (i == indiceInicio) {
            colores.push_back(1.0f); colores.push_back(0.0f); colores.push_back(0.0f); colores.push_back(1.0f); // Rojo
        } else if (i == indiceFinal) {
            colores.push_back(0.0f); colores.push_back(0.0f); colores.push_back(1.0f); colores.push_back(1.0f); // Azul
        } else {
            colores.push_back(1.0f); colores.push_back(1.0f); colores.push_back(1.0f); colores.push_back(1.0f); // Blanco
        }
    }

    // Crear el VBO para las líneas
    std::vector<float> lineVertices;
    std::vector<float> lineColores;
    for (const auto& p : puntos) {
        for (const auto& vecino : p.knn_vecinos) {
            int indiceVecino = vecino.second;
            const Punto& vecinoPunto = puntos[indiceVecino];

            float normX1 = (float)p.x / 999.0f * 2.0f - 1.0f;
            float normY1 = (float)p.y / 999.0f * 2.0f - 1.0f;
            float normX2 = (float)vecinoPunto.x / 999.0f * 2.0f - 1.0f;
            float normY2 = (float)vecinoPunto.y / 999.0f * 2.0f - 1.0f;

            lineVertices.push_back(normX1);
            lineVertices.push_back(normY1);
            lineVertices.push_back(normX2);
            lineVertices.push_back(normY2);

            // Color de línea
            lineColores.push_back(0.5f); lineColores.push_back(0.5f); lineColores.push_back(0.5f); lineColores.push_back(1.0f); // Gris
        }
    }

    // Añadir líneas para el camino
    for (size_t i = 0; i < caminoIndices.size() - 1; ++i) {
        int index1 = caminoIndices[i];
        int index2 = caminoIndices[i + 1];

        float normX1 = (float)puntos[index1].x / 999.0f * 2.0f - 1.0f;
        float normY1 = (float)puntos[index1].y / 999.0f * 2.0f - 1.0f;
        float normX2 = (float)puntos[index2].x / 999.0f * 2.0f - 1.0f;
        float normY2 = (float)puntos[index2].y / 999.0f * 2.0f - 1.0f;

        lineVertices.push_back(normX1);
        lineVertices.push_back(normY1);
        lineVertices.push_back(normX2);
        lineVertices.push_back(normY2);

        // Color para el camino
        lineColores.push_back(0.0f); lineColores.push_back(1.0f); lineColores.push_back(0.0f); lineColores.push_back(1.0f); // Verde
    }

    unsigned int VAO, VBO, colorVBO, lineVAO, lineVBO, lineColorVBO;

    // Configurar VAO y VBO para puntos
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &colorVBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, colores.size() * sizeof(float), colores.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Configurar VAO y VBO para líneas
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);
    glGenBuffers(1, &lineColorVBO);
    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, lineVertices.size() * sizeof(float), lineVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, lineColorVBO);
    glBufferData(GL_ARRAY_BUFFER, lineColores.size() * sizeof(float), lineColores.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Establece el fondo en negro
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Dibujar puntos con colores específicos
        glBindVertexArray(VAO);
        for (size_t i = 0; i < puntos.size(); ++i) {
            // Configurar el color dependiendo si es inicio, fin, o normal
            if (i == indiceInicio) {
                glUniform4f(glGetUniformLocation(shaderProgram, "color"), 1.0f, 0.0f, 0.0f, 1.0f); // Rojo
            } else if (i == indiceFinal) {
                glUniform4f(glGetUniformLocation(shaderProgram, "color"), 0.0f, 0.0f, 1.0f, 1.0f); // Azul
            } else {
                glUniform4f(glGetUniformLocation(shaderProgram, "color"), 1.0f, 1.0f, 1.0f, 1.0f); // Blanco
            }
            glDrawArrays(GL_POINTS, i, 1);
        }

        // Dibujar todas las aristas en color gris
        glBindVertexArray(lineVAO);
        glUniform4f(glGetUniformLocation(shaderProgram, "color"), 1.0f, 1.0f, 1.0f, 1.0f); // Blanco para las aristas que no forman el camino
        glDrawArrays(GL_LINES, 0, (lineVertices.size() / 2) - (caminoIndices.size() - 1) * 2);

        // Dibujar el camino con color verde
        glUniform4f(glGetUniformLocation(shaderProgram, "color"), 0.0f, 1.0f, 0.0f, 1.0f); // Verde
        glDrawArrays(GL_LINES, (lineVertices.size() / 2) - (caminoIndices.size() - 1) * 2, (caminoIndices.size() - 1) * 2);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &colorVBO);
    glDeleteVertexArrays(1, &lineVAO);
    glDeleteBuffers(1, &lineVBO);
    glDeleteBuffers(1, &lineColorVBO);

    glfwTerminate();
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


vector<int> dfs(const vector<Punto>& grafo, int inicio, int objetivo, vector<int>& visitados) {

    stack<int> pila;
    vector<int> camino;
    vector<bool> visitado(grafo.size(), false);
    
    pila.push(inicio);
    visitado[inicio] = true;
    visitados.push_back(inicio);  // Añadir el inicio a los puntos visitados
    
    while (!pila.empty()) {
        int nodo_actual = pila.top();
        pila.pop();
        camino.push_back(nodo_actual);
        
        // Si encontramos el objetivo
        if (nodo_actual == objetivo) {
            return camino;  // Retornar el camino final
        }

        // Explorar los vecinos
        for (const auto& vecino : grafo[nodo_actual].knn_vecinos) {
            int vecino_index = vecino.second;
            if (!visitado[vecino_index]) {
                visitado[vecino_index] = true;
                visitados.push_back(vecino_index);  // Añadir el vecino a los puntos visitados
                pila.push(vecino_index);
            }
        }
    }
    
    return {};  // Si no se encuentra un camino
}


vector<int> bfs(const vector<Punto>& grafo, int inicio, int objetivo, vector<int>& visitados) {
    queue<int> cola;
    vector<int> camino;
    vector<bool> visitado(grafo.size(), false);
    vector<int> padre(grafo.size(), -1);  // Para almacenar el camino de vuelta

    cola.push(inicio);
    visitado[inicio] = true;
    visitados.push_back(inicio);  // Añadir el nodo de inicio a los puntos visitados

    while (!cola.empty()) {
        int nodo_actual = cola.front();
        cola.pop();

        // Si encontramos el objetivo, reconstruimos el camino
        if (nodo_actual == objetivo) {
            int nodo = objetivo;
            while (nodo != -1) {
                camino.push_back(nodo);
                nodo = padre[nodo];
            }
            reverse(camino.begin(), camino.end());
            return camino;
        }

        // Explorar los vecinos del nodo actual
        for (const auto& vecino : grafo[nodo_actual].knn_vecinos) {
            int vecino_index = vecino.second;
            if (!visitado[vecino_index]) {
                visitado[vecino_index] = true;
                visitados.push_back(vecino_index);  // Añadir los vecinos a los puntos visitados
                padre[vecino_index] = nodo_actual;  // Registrar de dónde viene el nodo
                cola.push(vecino_index);
            }
        }
    }

    return {};  // Si no se encuentra un camino
}

vector<int> hill_climbing(const vector<Punto>& grafo, int inicio, int objetivo, vector<int>& visitados) {
    int nodo_actual = inicio;
    visitados.push_back(nodo_actual);  // Añadir el nodo inicial a los visitados
    vector<int> camino;
    camino.push_back(nodo_actual);  // Iniciar el camino con el nodo actual

    while (nodo_actual != objetivo) {
        double mejor_heuristica = calcular_heuristica(grafo[nodo_actual], grafo[objetivo]);
        int siguiente_nodo = -1;

        // Explorar los vecinos
        for (const auto& vecino : grafo[nodo_actual].knn_vecinos) {
            int vecino_index = vecino.second;
            double heuristica_vecino = calcular_heuristica(grafo[vecino_index], grafo[objetivo]);

            // Si el vecino tiene una mejor heurística, actualizar el siguiente nodo
            if (heuristica_vecino < mejor_heuristica) {
                mejor_heuristica = heuristica_vecino;
                siguiente_nodo = vecino_index;
            }
        }

        // Si no encontramos un vecino mejor, estamos en un máximo local
        if (siguiente_nodo == -1) {
            cout << "Hill Climbing: Se llegó a un máximo local en el nodo " << nodo_actual << endl;
            break;
        }

        // Moverse al siguiente nodo
        nodo_actual = siguiente_nodo;
        camino.push_back(nodo_actual);  // Añadir el siguiente nodo al camino
        visitados.push_back(nodo_actual);  // Añadir el nodo visitado
    }

    // Si alcanzamos el objetivo
    if (nodo_actual == objetivo) {
        cout << "Hill Climbing: Camino encontrado." << endl;
    } else {
        cout << "Hill Climbing: No se encontró un camino óptimo (máximo local)." << endl;
    }

    return camino;
}


void graficar_dfs_bfs(const std::vector<Punto>& puntos, const std::vector<int>& visitados,
                  const std::vector<int>& camino, int windowWidth, int windowHeight,
                  int indiceInicio, int indiceFinal) {
    if (!glfwInit())
        return;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "DFS - Graficado de Puntos", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create the window!" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to load OpenGL function pointers!" << std::endl;
        glfwTerminate();
        return;
    }

    // Configuración de OpenGL
    glPointSize(10.0f);  // Tamaño de los puntos

    // Configurar Shaders
    const char* vertexShaderSrc =
        "#version 330 core\n"
        "layout (location = 0) in vec2 aPos;\n"
        "out vec2 vertexPos;\n"
        "void main() {\n"
        "    gl_Position = vec4(aPos, 0.0f, 1.0f);\n"
        "    vertexPos = aPos;\n"
        "}\0";

    const char* fragmentShaderSrc =
        "#version 330 core\n"
        "in vec2 vertexPos;\n"
        "out vec4 fragColor;\n"
        "uniform vec4 color;\n"
        "void main() {\n"
        "    fragColor = color;\n"
        "}\0";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSrc, 0);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, 0, infoLog);
        std::cout << "Failed to compile the vertex shader! ERR: " << infoLog << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSrc, 0);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, 0, infoLog);
        std::cout << "Failed to compile the fragment shader! ERR: " << infoLog << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, 0, infoLog);
        std::cout << "Failed to link the shader program! ERR: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Crear el VBO para los puntos
    std::vector<float> vertices;
    std::vector<float> colores;
    for (size_t i = 0; i < puntos.size(); ++i) {
        float normX = (float)puntos[i].x / 999.0f * 2.0f - 1.0f;
        float normY = (float)puntos[i].y / 999.0f * 2.0f - 1.0f;
        vertices.push_back(normX);
        vertices.push_back(normY);

        // Colorear puntos especiales
        if (i == indiceInicio) {
            colores.push_back(1.0f); colores.push_back(0.0f); colores.push_back(0.0f); colores.push_back(1.0f); // Rojo
        } else if (i == indiceFinal) {
            colores.push_back(0.0f); colores.push_back(0.0f); colores.push_back(1.0f); colores.push_back(1.0f); // Azul
        } else if (std::find(camino.begin(), camino.end(), i) != camino.end()) {
            colores.push_back(0.0f); colores.push_back(1.0f); colores.push_back(0.0f); colores.push_back(1.0f); // Verde
        } else if (std::find(visitados.begin(), visitados.end(), i) != visitados.end()) {
            colores.push_back(1.0f); colores.push_back(1.0f); colores.push_back(0.0f); colores.push_back(1.0f); // Amarillo
        } else {
            colores.push_back(1.0f); colores.push_back(1.0f); colores.push_back(1.0f); colores.push_back(1.0f); // Blanco
        }
    }

    // Crear el VBO para las líneas
    std::vector<float> lineVertices;
    std::vector<float> lineColores;
    for (const auto& p : puntos) {
        for (const auto& vecino : p.knn_vecinos) {
            int indiceVecino = vecino.second;
            const Punto& vecinoPunto = puntos[indiceVecino];

            float normX1 = (float)p.x / 999.0f * 2.0f - 1.0f;
            float normY1 = (float)p.y / 999.0f * 2.0f - 1.0f;
            float normX2 = (float)vecinoPunto.x / 999.0f * 2.0f - 1.0f;
            float normY2 = (float)vecinoPunto.y / 999.0f * 2.0f - 1.0f;

            lineVertices.push_back(normX1);
            lineVertices.push_back(normY1);
            lineVertices.push_back(normX2);
            lineVertices.push_back(normY2);

            // Color de línea
            lineColores.push_back(0.5f); lineColores.push_back(0.5f); lineColores.push_back(0.5f); lineColores.push_back(1.0f); // Gris
        }
    }

    // Añadir líneas para el camino
    for (size_t i = 0; i < camino.size() - 1; ++i) {
        int index1 = camino[i];
        int index2 = camino[i + 1];

        float normX1 = (float)puntos[index1].x / 999.0f * 2.0f - 1.0f;
        float normY1 = (float)puntos[index1].y / 999.0f * 2.0f - 1.0f;
        float normX2 = (float)puntos[index2].x / 999.0f * 2.0f - 1.0f;
        float normY2 = (float)puntos[index2].y / 999.0f * 2.0f - 1.0f;

        lineVertices.push_back(normX1);
        lineVertices.push_back(normY1);
        lineVertices.push_back(normX2);
        lineVertices.push_back(normY2);

        // Color para el camino
        lineColores.push_back(0.0f); lineColores.push_back(1.0f); lineColores.push_back(0.0f); lineColores.push_back(1.0f); // Verde
    }

    unsigned int VAO, VBO, colorVBO, lineVAO, lineVBO, lineColorVBO;

    // Configurar VAO y VBO para puntos
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &colorVBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, colores.size() * sizeof(float), colores.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Configurar VAO y VBO para líneas
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);
    glGenBuffers(1, &lineColorVBO);
    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, lineVertices.size() * sizeof(float), lineVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, lineColorVBO);
    glBufferData(GL_ARRAY_BUFFER, lineColores.size() * sizeof(float), lineColores.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Establece el fondo en negro
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Dibujar puntos con colores específicos
        glBindVertexArray(VAO);
        for (size_t i = 0; i < puntos.size(); ++i) {
            // Configurar el color dependiendo si es inicio, fin, o normal
            if (i == indiceInicio) {
                glUniform4f(glGetUniformLocation(shaderProgram, "color"), 1.0f, 0.0f, 0.0f, 1.0f); // Rojo
            } else if (i == indiceFinal) {
                glUniform4f(glGetUniformLocation(shaderProgram, "color"), 0.0f, 0.0f, 1.0f, 1.0f); // Azul
            } else if (std::find(camino.begin(), camino.end(), i) != camino.end()) {
                glUniform4f(glGetUniformLocation(shaderProgram, "color"), 0.0f, 1.0f, 0.0f, 1.0f); // Verde
            } else if (std::find(visitados.begin(), visitados.end(), i) != visitados.end()) {
                glUniform4f(glGetUniformLocation(shaderProgram, "color"), 1.0f, 1.0f, 0.0f, 1.0f); // Amarillo
            } else {
                glUniform4f(glGetUniformLocation(shaderProgram, "color"), 1.0f, 1.0f, 1.0f, 1.0f); // Blanco
            }
            glDrawArrays(GL_POINTS, i, 1);
        }

        // Dibujar todas las aristas en color gris
        glBindVertexArray(lineVAO);
        glUniform4f(glGetUniformLocation(shaderProgram, "color"), 1.0f, 1.0f, 1.0f, 1.0f); // Blanco para las aristas que no forman el camino
        glDrawArrays(GL_LINES, 0, (lineVertices.size() / 2));

        // Dibujar el camino con color verde
        glUniform4f(glGetUniformLocation(shaderProgram, "color"), 0.0f, 1.0f, 0.0f, 1.0f); // Verde
        glDrawArrays(GL_LINES, (lineVertices.size() / 2) - (camino.size() - 1) * 2, (camino.size() - 1) * 2);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &colorVBO);
    glDeleteVertexArrays(1, &lineVAO);
    glDeleteBuffers(1, &lineVBO);
    glDeleteBuffers(1, &lineColorVBO);

    glfwTerminate();
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
    //imprimir_vecinos(puntos_seleccionados, k);
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int inicio = 2;
    int objetivo = 43;
    vector<int> visitados_a_star;  // Para almacenar todos los nodos que A* recorrió
    vector<int> camino_a_star = a_star_search(puntos_seleccionados, inicio, objetivo, visitados_a_star);

    // Graficar los puntos visitados y el camino encontrado con A*
    graficar_dfs_bfs(puntos_seleccionados, visitados_a_star, camino_a_star, 800, 600, inicio, objetivo);


    /*
    // Imprimir el camino si se encontró
    if (!camino_a_star.empty()) {
        cout << "Camino encontrado: ";
        for (int nodo : camino_a_star) {
            cout << nodo << " ";
        }
        cout << endl;
    } else {
        cout << "No se encontró un camino." << endl;
    }
    */
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //HILL_CLIMBING
    //int inicio = 2;
    //int objetivo = 43;
    vector<int> visitados_hill_climbing;  // Para almacenar todos los nodos que el Hill Climbing recorrió
    vector<int> camino_hill_climbing = hill_climbing(puntos_seleccionados, inicio, objetivo, visitados_hill_climbing);

    // Graficar los puntos visitados y el camino encontrado con Hill Climbing
    graficar_dfs_bfs(puntos_seleccionados, visitados_hill_climbing, camino_hill_climbing, 800, 600, inicio, objetivo);
    
    // Imprimir el camino si se encontró
    /*
    if (!camino_hill_climbing.empty()) {
        cout << "Camino encontrado: ";
        for (int nodo : camino_hill_climbing) {
            cout << nodo << " ";
        }
        cout << endl;
    } else {
        cout << "No se encontró un camino." << endl;
    }
    */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //DFS
    /*
    int inicio = 2;
    int objetivo = 43;

    vector<int> visitados;  // Para almacenar todos los nodos que el DFS recorrió
    vector<int> camino_dfs = dfs(puntos_seleccionados, inicio, objetivo, visitados);

    // Imprimir el camino si se encontró
    if (!camino_dfs.empty()) {
        cout << "Camino encontrado: ";
        for (int nodo : camino_dfs) {
            cout << nodo << " ";
        }
        cout << endl;
    } else {
        cout << "No se encontró un camino." << endl;
    }

    // Graficar los puntos visitados y el camino encontrado
    graficar_dfs_bfs(puntos_seleccionados, visitados, camino_dfs, 800, 600, inicio, objetivo);
    */

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //int inicio = 2;
    //int objetivo = 43;
    
    //BFS
    /*
    vector<int> visitados_bfs;  // Para almacenar todos los nodos que el BFS recorrió
    vector<int> camino_bfs = bfs(puntos_seleccionados, inicio, objetivo, visitados_bfs);

      if (!camino_bfs.empty()) {
        cout << "Camino encontrado: ";
        for (int nodo : camino_bfs) {
            cout << nodo << " ";
        }
        cout << endl;
    } else {
        cout << "No se encontró un camino." << endl;
    }

    // Graficar los puntos visitados y el camino encontrado
    graficar_dfs_bfs(puntos_seleccionados, visitados_bfs, camino_bfs, 800, 600, inicio, objetivo);
    */

    


    return 0;
}
