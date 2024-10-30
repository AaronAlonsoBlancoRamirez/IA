#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <numeric>
#include <cmath>
#include <limits>
#include <set>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;

struct Punto {
    int x, y;
    vector<pair<double, int>> knn_vecinos;
};

// Función para calcular la distancia euclidiana entre dos puntos
double distancia_euclidiana(const Punto& p1, const Punto& p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

// Función para calcular la longitud total de un recorrido
double calcular_longitud_recorrido(const vector<int>& recorrido, const vector<Punto>& puntos) {
    double longitud_total = 0;
    for (size_t i = 0; i < recorrido.size() - 1; ++i) {
        longitud_total += distancia_euclidiana(puntos[recorrido[i]], puntos[recorrido[i + 1]]);
    }
    // Agregar la distancia de vuelta al punto inicial
    longitud_total += distancia_euclidiana(puntos[recorrido.back()], puntos[recorrido.front()]);
    return longitud_total;
}

// Generación de una población inicial de rutas aleatorias
vector<vector<int>> generar_poblacion_inicial(int tam_poblacion, int num_puntos) {
    vector<vector<int>> poblacion;
    vector<int> ruta_base(num_puntos);
    iota(ruta_base.begin(), ruta_base.end(), 0); // Inicializar con 0, 1, ..., num_puntos - 1

    random_device rd;
    mt19937 gen(rd());

    for (int i = 0; i < tam_poblacion; ++i) {
        shuffle(ruta_base.begin(), ruta_base.end(), gen);
        poblacion.push_back(ruta_base);
    }
    return poblacion;
}

// Función de selección por torneo
vector<int> seleccion_por_torneo(const vector<vector<int>>& poblacion, const vector<double>& fitness, int tam_torneo) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, poblacion.size() - 1);

    int mejor = dis(gen);
    for (int i = 1; i < tam_torneo; ++i) {
        int candidato = dis(gen);
        if (fitness[candidato] < fitness[mejor]) {
            mejor = candidato;
        }
    }
    return poblacion[mejor];
}

// Cruce de dos rutas (Order Crossover - OX)
vector<int> crossover(const vector<int>& padre1, const vector<int>& padre2) {
    int n = padre1.size();
    vector<int> hijo(n, -1);

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, n - 1);

    int inicio = dis(gen);
    int fin = dis(gen);
    if (inicio > fin) swap(inicio, fin);

    // Copiar segmento del primer padre
    for (int i = inicio; i <= fin; ++i) {
        hijo[i] = padre1[i];
    }

    // Completar con genes del segundo padre
    int indice_hijo = (fin + 1) % n;
    for (int i = 0; i < n; ++i) {
        int indice_padre = (fin + 1 + i) % n;
        if (find(hijo.begin(), hijo.end(), padre2[indice_padre]) == hijo.end()) {
            hijo[indice_hijo] = padre2[indice_padre];
            indice_hijo = (indice_hijo + 1) % n;
        }
    }

    return hijo;
}

// Mutación por intercambio de dos ciudades
void mutacion(vector<int>& ruta) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, ruta.size() - 1);

    int i = dis(gen);
    int j = dis(gen);
    swap(ruta[i], ruta[j]);
}

// Algoritmo genético para el TSP
vector<int> algoritmo_genetico_tsp(const vector<Punto>& puntos, int tam_poblacion, int num_generaciones, double prob_mutacion) {
    int num_puntos = puntos.size();
    vector<vector<int>> poblacion = generar_poblacion_inicial(tam_poblacion, num_puntos);

    for (int generacion = 0; generacion < num_generaciones; ++generacion) {
        // Calcular fitness para cada individuo
        vector<double> fitness;
        for (const auto& individuo : poblacion) {
            fitness.push_back(calcular_longitud_recorrido(individuo, puntos));
        }

        // Mostrar información de la generación actual
        double mejor_fitness = *min_element(fitness.begin(), fitness.end());
        double promedio_fitness = accumulate(fitness.begin(), fitness.end(), 0.0) / fitness.size();
        cout << "Generación " << generacion + 1 << ": Mejor fitness = " << mejor_fitness << ", Promedio fitness = " << promedio_fitness << endl;

        // Nueva generación
        vector<vector<int>> nueva_poblacion;
        while (nueva_poblacion.size() < poblacion.size()) {
            // Selección
            vector<int> padre1 = seleccion_por_torneo(poblacion, fitness, 3);
            vector<int> padre2 = seleccion_por_torneo(poblacion, fitness, 3);

            // Cruce
            vector<int> hijo = crossover(padre1, padre2);

            // Mutación
            if ((double)rand() / RAND_MAX < prob_mutacion) {
                mutacion(hijo);
            }

            nueva_poblacion.push_back(hijo);
        }

        poblacion = nueva_poblacion;
    }

    // Devolver el mejor individuo de la última generación
    vector<double> fitness_final;
    for (const auto& individuo : poblacion) {
        fitness_final.push_back(calcular_longitud_recorrido(individuo, puntos));
    }
    auto mejor_it = min_element(fitness_final.begin(), fitness_final.end());
    cout << "Mejor fitness final: " << *mejor_it << endl;
    return poblacion[distance(fitness_final.begin(), mejor_it)];
}

// Declaraciones de funciones
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);


void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
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
    // Ejemplo de uso con puntos aleatorios
    int n = 20; // Número de puntos
    vector<Punto> puntos(n);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 100);

    for (int i = 0; i < n; ++i) {
        puntos[i] = {dis(gen), dis(gen)};
    }

    // Parámetros del algoritmo genético
    int tam_poblacion = 50;
    int num_generaciones = 100;
    double prob_mutacion = 0.1;

    int inicio = 1;
    // Ejecutar el algoritmo genético
    vector<int> mejor_ruta = algoritmo_genetico_tsp(puntos, tam_poblacion, num_generaciones, prob_mutacion);

    graficar_dfs_bfs(puntos,mejor_ruta,mejor_ruta,800,600,inicio,inicio);

    // Imprimir la mejor ruta encontrada
    cout << "Mejor ruta encontrada:" << endl;
    for (int i : mejor_ruta) {
        cout << i << " -> ";
    }
    cout << mejor_ruta.front() << " (regreso al inicio)" << endl;

    return 0;
}
