#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <thread>
using namespace std;
const int window_width = 600;
const int window_height = 600;
const int grid_size = 8;
const float square_size = 2.0f / grid_size;


bool esTurnoBlancas = true;  // Inicializa en true porque el juego comienza con las blancas
bool movimientoRealizado = false;  // Inicializar en false
// Variable global para saber si la IA ya jugó en este ciclo
bool IAHaJugado = false;




struct Piece {
    bool isKing;
    bool isWhite;
    int x, y;
};

std::vector<Piece> pieces;
Piece* selectedPiece = nullptr;  // Puntero a la pieza seleccionada

void initializePieces() {
    for (int i = 0; i < grid_size; ++i) {
        for (int j = 0; j < 3; ++j) {
            if ((i + j) % 2 != 0) {
                pieces.push_back({false, true, i, j});
            }
        }
    }

    for (int i = 0; i < grid_size; ++i) {
        for (int j = 5; j < grid_size; ++j) {
            if ((i + j) % 2 != 0) {
                pieces.push_back({false, false, i, j});
            }
        }
    }
}

void drawBoard() {
    for (int row = 0; row < grid_size; ++row) {
        for (int col = 0; col < grid_size; ++col) {
            if ((row + col) % 2 == 0)
                glColor3f(0.8f, 0.8f, 0.8f);
            else
                glColor3f(0.2f, 0.2f, 0.2f);

            float x = col * square_size - 1.0f;
            float y = row * square_size - 1.0f;

            glBegin(GL_QUADS);
            glVertex2f(x, y);
            glVertex2f(x + square_size, y);
            glVertex2f(x + square_size, y + square_size);
            glVertex2f(x, y + square_size);
            glEnd();
        }
    }
}

void drawPieces() {
    for (const auto& piece : pieces) {
        if (piece.isWhite)
            glColor3f(1.0f, 1.0f, 1.0f);
        else
            glColor3f(1.0f, 0.0f, 0.0f);

        float x = piece.x * square_size - 1.0f + square_size / 2;
        float y = piece.y * square_size - 1.0f + square_size / 2;
        float radius = square_size / 2.5f;

        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for (int i = 0; i <= 20; i++) {
            float angle = i * 2.0f * 3.14159f / 20;
            glVertex2f(x + (cos(angle) * radius), y + (sin(angle) * radius));
        }
        glEnd();
    }
}

// Convertir las coordenadas de la ventana a coordenadas del tablero
std::pair<int, int> getBoardCoords(double xpos, double ypos) {
    int col = static_cast<int>((xpos / window_width) * grid_size);
    int row = static_cast<int>(((window_height - ypos) / window_height) * grid_size);
    return {col, row};
}

// Eliminar una pieza de la posición dada
void removePiece(int col, int row) {
    for (auto it = pieces.begin(); it != pieces.end(); ++it) {
        if (it->x == col && it->y == row) {
            pieces.erase(it);
            break;
        }
    }
}

// Manejar la selección y el movimiento del jugador
// Manejar la selección y el movimiento del jugador
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (!esTurnoBlancas) {
        std::cout << "Es el turno de la IA. No se puede realizar un movimiento ahora." << std::endl;
        return;  // Desactiva los movimientos del jugador mientras es el turno de la IA
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        auto [col, row] = getBoardCoords(xpos, ypos);

        // Si no hay ninguna pieza seleccionada, selecciona una pieza
        if (!selectedPiece) {
            for (auto& piece : pieces) {
                if (piece.x == col && piece.y == row && piece.isWhite) {
                    selectedPiece = &piece;
                    break;
                }
            }
        }
        // Si ya hay una pieza seleccionada, intenta moverla
        else {
            int dx = std::abs(selectedPiece->x - col);
            int dy = std::abs(selectedPiece->y - row);

            // Validar que el movimiento sea diagonal y a una casilla vacía
            if (dx == 1 && dy == 1) {
                selectedPiece->x = col;
                selectedPiece->y = row;
                selectedPiece = nullptr;  // Deseleccionar después del movimiento
                movimientoRealizado = true; // Marcar que el jugador ha hecho un movimiento
            }
            // Captura: movimiento en diagonal de 2 espacios con una pieza enemiga en el medio
            else if (dx == 2 && dy == 2) {
                int middle_x = (selectedPiece->x + col) / 2;
                int middle_y = (selectedPiece->y + row) / 2;

                // Verificar si hay una pieza enemiga en el medio
                for (const auto& piece : pieces) {
                    if (piece.x == middle_x && piece.y == middle_y && !piece.isWhite) {
                        // Captura válida
                        removePiece(middle_x, middle_y);  // Eliminar la pieza enemiga
                        selectedPiece->x = col;
                        selectedPiece->y = row;
                        selectedPiece = nullptr;
                        movimientoRealizado = true; // Marcar que el jugador ha hecho un movimiento
                        break;
                    }
                }
            }
        }
    }
}



int evaluarTablero(const std::vector<Piece>& pieces) {
    int score = 0;

    for (const auto& piece : pieces) {
        if (piece.isWhite) {
            score -= 10;  // Las piezas blancas (oponente) restan valor
            if (piece.isKing) score -= 5;  // Las damas blancas restan más valor
        } else {
            score += 10;  // Las piezas rojas (IA) suman valor
            if (piece.isKing) score += 5;  // Las damas rojas suman más valor
        }
    }

    std::cout << "Evaluación del tablero: " << score << std::endl;  // Depuración de evaluación
    return score;
}

std::vector<std::vector<Piece>> generarMovimientosPosibles(const std::vector<Piece>& pieces, bool IA_turno) {
    std::vector<std::vector<Piece>> movimientos;
    bool capturaEncontrada = false;

    std::cout << "Generando movimientos para la IA..." << std::endl;

    for (size_t i = 0; i < pieces.size(); ++i) {
        Piece piece = pieces[i];

        // Solo generar movimientos para piezas rojas si es el turno de la IA
        if (IA_turno && !piece.isWhite) {
            std::cout << "Evaluando pieza roja en posición (" << piece.x << ", " << piece.y << ")" << std::endl;

            // Movimientos normales (solo una casilla diagonal)
            if (!capturaEncontrada && piece.x + 1 < grid_size && piece.y + 1 < grid_size) {
                std::vector<Piece> nuevoEstado = pieces;
                nuevoEstado[i].x += 1;
                nuevoEstado[i].y += 1;
                movimientos.push_back(nuevoEstado);

                std::cout << "Movimiento normal generado: Pieza se mueve a (" << nuevoEstado[i].x << ", " << nuevoEstado[i].y << ")" << std::endl;
            }

            // Movimientos de captura (prioritarios)
            if (piece.x + 2 < grid_size && piece.y + 2 < grid_size) {
                int middle_x = (piece.x + (piece.x + 2)) / 2;
                int middle_y = (piece.y + (piece.y + 2)) / 2;

                std::cout << "Posible captura en (" << middle_x << ", " << middle_y << ")" << std::endl;

                // Verificar si hay una pieza enemiga en el medio
                for (const auto& enemyPiece : pieces) {
                    if (enemyPiece.x == middle_x && enemyPiece.y == middle_y && enemyPiece.isWhite) {
                        std::vector<Piece> nuevoEstado = pieces;
                        removePiece(middle_x, middle_y);  // Eliminar la pieza capturada
                        nuevoEstado[i].x += 2;
                        nuevoEstado[i].y += 2;
                        movimientos.push_back(nuevoEstado);

                        capturaEncontrada = true;  // Priorizar la captura
                        std::cout << "Captura válida: Pieza enemiga eliminada en (" << middle_x << ", " << middle_y << ")" << std::endl;
                        std::cout << "La pieza se mueve a (" << nuevoEstado[i].x << ", " << nuevoEstado[i].y << ")" << std::endl;
                    }
                }
            }
        }
    }

    if (capturaEncontrada) {
        std::cout << "Movimientos de captura generados." << std::endl;
    }

    return movimientos;
}



int minimax(std::vector<Piece>& pieces, int profundidad, bool maximizador) {
    // Evaluar el tablero si llegamos a la profundidad deseada o el juego ha terminado
    if (profundidad == 0) {
        return evaluarTablero(pieces);
    }

    if (maximizador) {
        int mejorValor = -10000;
        std::vector<std::vector<Piece>> movimientos = generarMovimientosPosibles(pieces, true);
        
        for (auto& nuevoEstado : movimientos) {  // Quitar const
            int valor = minimax(nuevoEstado, profundidad - 1, false);
            mejorValor = std::max(mejorValor, valor);
        }
        return mejorValor;
    } else {
        int peorValor = 10000;
        std::vector<std::vector<Piece>> movimientos = generarMovimientosPosibles(pieces, false);
        
        for (auto& nuevoEstado : movimientos) {  // Quitar const
            int valor = minimax(nuevoEstado, profundidad - 1, true);
            peorValor = std::min(peorValor, valor);
        }
        return peorValor;
    }
}

void turnoIA() {
    std::cout << "Turno de la IA" << std::endl;

    int mejorValor = -10000;
    std::vector<Piece> mejorMovimiento;
    std::vector<std::vector<Piece>> movimientos = generarMovimientosPosibles(pieces, true);

    if (movimientos.empty()) {
        std::cout << "No se encontraron movimientos válidos para la IA. Termina el turno." << std::endl;
        return;
    }

    std::cout << "Número de movimientos posibles para la IA: " << movimientos.size() << std::endl;

    for (auto& nuevoEstado : movimientos) {
        int valor = minimax(nuevoEstado, 3, false);
        std::cout << "Valor del movimiento evaluado por Minimax: " << valor << std::endl;
        if (valor > mejorValor) {
            mejorValor = valor;
            mejorMovimiento = nuevoEstado;
        }
    }

    if (!mejorMovimiento.empty()) {
        pieces = mejorMovimiento;  // Actualizar el tablero con el mejor movimiento encontrado
    } else {
        std::cout << "La IA no pudo encontrar un movimiento válido." << std::endl;
    }
}



bool jugadorHaCompletadoMovimiento() {
    // Aquí deberías verificar si el jugador ha realizado un movimiento válido
    // Por ejemplo, después de hacer clic en la ficha blanca y moverla a una posición válida
    // Si el jugador ya hizo su movimiento, retornar true
    return movimientoRealizado;  // Esto es solo un ejemplo, ajusta según tu lógica
}

void actualizarJuego() {
    if (esTurnoBlancas) {
        // Si es el turno de las blancas, verifica si el jugador ha completado su movimiento
        if (jugadorHaCompletadoMovimiento()) {
            esTurnoBlancas = false;  // Cambia el turno a la IA
            std::cout << "Turno del jugador blanco completado. Ahora es turno de la IA." << std::endl;
        }
    } else {
        // Turno de la IA
        turnoIA();
        esTurnoBlancas = true;  // Cambia el turno de vuelta al jugador
        std::cout << "Turno de la IA completado. Ahora es turno del jugador blanco." << std::endl;
    }
}





int main() {
    // Inicializar GLFW
    if (!glfwInit()) {
        std::cerr << "Error al inicializar GLFW" << std::endl;
        return -1;
    }

    // Crear la ventana de OpenGL
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Juego de Damas", NULL, NULL);
    if (!window) {
        std::cerr << "Error al crear la ventana" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Inicializar las piezas en el tablero
    initializePieces();

    // Configurar el callback para manejar el clic del mouse
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Ciclo principal del juego
    while (!glfwWindowShouldClose(window)) {
        // Limpiar el buffer
        glClear(GL_COLOR_BUFFER_BIT);

        // Dibujar el tablero y las piezas
        drawBoard();
        drawPieces();

        // Actualizar el estado del juego
        actualizarJuego();  // Verifica y cambia los turnos

        // Procesar eventos y actualizar gráficos
        glfwSwapBuffers(window);
        glfwPollEvents();  // Asegura que se procesen los eventos del mouse

        // Pausa para evitar ciclos infinitos
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
    }

    // Destruir la ventana y terminar
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
