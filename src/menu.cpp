#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

// Constantes de la plancha (en cm)
const float PLANCHAL_WIDTH = 220.0f;
const float PLANCHAL_HEIGHT = 190.0f;

// Estructura para representar los vidrios
struct Vidrio {
    float ancho;
    float alto;
};

// Estructura para representar una plancha
struct Plancha {
    std::vector<Vidrio> vidrios;
    float espacio_disponible_alto;
};

// Función para inicializar OpenGL
void initOpenGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Fondo negro
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, PLANCHAL_WIDTH, 0.0f, PLANCHAL_HEIGHT, -1.0f, 1.0f); // Dimensiones de la plancha
}

// Función para redimensionar la ventana y mantener proporciones
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // Mantener la relación de aspecto de la plancha (220x190)
    float aspect_ratio = PLANCHAL_WIDTH / PLANCHAL_HEIGHT;
    int new_width = width;
    int new_height = height;

    if (width / static_cast<float>(height) > aspect_ratio) {
        new_width = height * aspect_ratio;
    } else {
        new_height = width / aspect_ratio;
    }

    int viewport_x = (width - new_width) / 2;
    int viewport_y = (height - new_height) / 2;
    glViewport(viewport_x, viewport_y, new_width, new_height);
}

// Añadimos un valor de padding
const float PADDING = 10.0f;  // Puedes ajustar este valor para aumentar o reducir el espacio

// Función para dibujar las planchas con padding
void drawPlanchas(const std::vector<Plancha>& planchas) {
    glClear(GL_COLOR_BUFFER_BIT);

    for (size_t i = 0; i < planchas.size(); ++i) {
        float yOffset = i * (PLANCHAL_HEIGHT + 10);  // Espacio entre planchas

        // Dibuja la plancha (borde) con padding
        glColor3f(1.0f, 1.0f, 1.0f);  // Borde blanco
        glBegin(GL_LINE_LOOP);
        glVertex2f(PADDING, PADDING);  // Inferior izquierda
        glVertex2f(PLANCHAL_WIDTH - PADDING, PADDING);  // Inferior derecha
        glVertex2f(PLANCHAL_WIDTH - PADDING, PLANCHAL_HEIGHT - PADDING);  // Superior derecha
        glVertex2f(PADDING, PLANCHAL_HEIGHT - PADDING);  // Superior izquierda
        glEnd();

        // Dibuja los cortes de lado a lado (vidrios)
        float cortePosY = PADDING;  // Empezar desde el fondo con padding
        for (const auto& vidrio : planchas[i].vidrios) {
            glColor3f(0.0f, 0.0f, 1.0f);  // Color azul para los vidrios
            glBegin(GL_QUADS);
            glVertex2f(PADDING, cortePosY);  // Esquina inferior izquierda del vidrio
            glVertex2f(vidrio.ancho - PADDING, cortePosY);  // Esquina inferior derecha del vidrio
            glVertex2f(vidrio.ancho - PADDING, cortePosY + vidrio.alto);  // Esquina superior derecha del vidrio
            glVertex2f(PADDING, cortePosY + vidrio.alto);  // Esquina superior izquierda del vidrio
            glEnd();
            cortePosY += vidrio.alto;  // Mover la siguiente posición hacia arriba
        }
    }

    glFlush();
}

// Función para dibujar etiquetas que identifiquen los ejes
void drawLabels() {
    // Dibujar una línea en el eje X
    glColor3f(1.0f, 0.0f, 0.0f);  // Rojo para el eje X
    glBegin(GL_LINES);
    glVertex2f(PADDING, PADDING);  // Inicio del eje X
    glVertex2f(PLANCHAL_WIDTH - PADDING, PADDING);  // Fin del eje X
    glEnd();

    // Dibujar una línea en el eje Y
    glColor3f(0.0f, 1.0f, 0.0f);  // Verde para el eje Y
    glBegin(GL_LINES);
    glVertex2f(PADDING, PADDING);  // Inicio del eje Y
    glVertex2f(PADDING, PLANCHAL_HEIGHT - PADDING);  // Fin del eje Y
    glEnd();
}

// Algoritmo First Fit para acomodar los vidrios
void firstFit(std::vector<Vidrio>& vidrios, std::vector<Plancha>& planchas) {
    for (const auto& vidrio : vidrios) {
        bool colocado = false;
        for (auto& plancha : planchas) {
            // Verificar si el vidrio cabe en la plancha
            if (plancha.espacio_disponible_alto >= vidrio.alto) {
                plancha.vidrios.push_back(vidrio);
                plancha.espacio_disponible_alto -= vidrio.alto;
                colocado = true;
                break;
            }
        }

        // Si no cabe en ninguna plancha, creamos una nueva
        if (!colocado) {
            Plancha nueva_plancha = {std::vector<Vidrio>{vidrio}, PLANCHAL_HEIGHT - vidrio.alto};
            planchas.push_back(nueva_plancha);
        }
    }
}

// Función principal
int main() {
    // Entrada de datos
    int numero_vidrios;
    std::cout << "Ingrese el número de vidrios: ";
    std::cin >> numero_vidrios;

    std::vector<Vidrio> vidrios;
    for (int i = 0; i < numero_vidrios; ++i) {
        Vidrio vidrio;
        std::cout << "Ingrese el ancho del vidrio " << i + 1 << " (de 0 a 220 cm): ";
        std::cin >> vidrio.ancho;
        std::cout << "Ingrese la altura del vidrio " << i + 1 << " (de 0 a 190 cm): ";
        std::cin >> vidrio.alto;
        vidrios.push_back(vidrio);
    }

    // Aplicar algoritmo First Fit
    std::vector<Plancha> planchas;
    firstFit(vidrios, planchas);

    // Inicializar ventana con GLFW después de ingresar los datos
    if (!glfwInit()) {
        std::cerr << "No se pudo inicializar GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Trazado de Cortes - Plancha de Vidrio", NULL, NULL);
    if (!window) {
        std::cerr << "No se pudo crear la ventana" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "No se pudo inicializar GLAD" << std::endl;
        return -1;
    }

    initOpenGL();

    // Configurar la función de redimensionado
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Bucle principal
    while (!glfwWindowShouldClose(window)) {
        // Dibujar las planchas y los vidrios
        drawPlanchas(planchas);
        // Dibujar las etiquetas para confirmar el ancho y largo
        drawLabels();
        // Procesar eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
