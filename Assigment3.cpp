#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

const int NUMERO_CILINDROS = 5000;
const int NUMERO_SOLICITUDES = 1000;

long long fcfs(int posicionInicial, const std::vector<int>& solicitudes) {
    long long movimiento = 0;
    int cabeza = posicionInicial;
    for (int i = 0; i < NUMERO_SOLICITUDES; i++) {
        movimiento += std::abs(solicitudes[i] - cabeza);
        cabeza = solicitudes[i];
    }
    return movimiento;
}

long long scan(int posicionInicial, const std::vector<int>& solicitudes) {
    long long movimiento = 0;
    int cabeza = posicionInicial;
    std::vector<int> izquierda;
    std::vector<int> derecha;

    for (int i = 0; i < NUMERO_SOLICITUDES; i++) {
        if (solicitudes[i] < posicionInicial)
            izquierda.push_back(solicitudes[i]);
        else
            derecha.push_back(solicitudes[i]);
    }

    std::sort(izquierda.begin(), izquierda.end());
    std::sort(derecha.begin(), derecha.end());

    // Primero hacia el extremo superior.
    for (int i = 0; i < (int)derecha.size(); i++) {
        movimiento += std::abs(derecha[i] - cabeza);
        cabeza = derecha[i];
    }
    // Hasta el cilindro más alto.
    movimiento += std::abs((NUMERO_CILINDROS - 1) - cabeza);
    cabeza = NUMERO_CILINDROS - 1;

    // Luego bajamos atendiendo la parte izquierda en orden inverso.
    for (int i = (int)izquierda.size() - 1; i >= 0; i--) {
        movimiento += std::abs(izquierda[i] - cabeza);
        cabeza = izquierda[i];
    }
    // Finalmente al cilindro 0.
    movimiento += std::abs(cabeza - 0);

    return movimiento;
}

long long cscan(int posicionInicial, const std::vector<int>& solicitudes) {
    long long movimiento = 0;
    int cabeza = posicionInicial;
    std::vector<int> izquierda;
    std::vector<int> derecha;

    for (int i = 0; i < NUMERO_SOLICITUDES; i++) {
        if (solicitudes[i] < posicionInicial)
            izquierda.push_back(solicitudes[i]);
        else
            derecha.push_back(solicitudes[i]);
    }

    std::sort(izquierda.begin(), izquierda.end());
    std::sort(derecha.begin(), derecha.end());

    // Atiendo hacia arriba.
    for (int i = 0; i < (int)derecha.size(); i++) {
        movimiento += std::abs(derecha[i] - cabeza);
        cabeza = derecha[i];
    }
    // Hasta el extremo.
    movimiento += std::abs((NUMERO_CILINDROS - 1) - cabeza);
    // Salto al inicio sin contar movimiento.
    cabeza = 0;

    // Se atiende desde el inicio.
    for (int i = 0; i < (int)izquierda.size(); i++) {
        movimiento += std::abs(izquierda[i] - cabeza);
        cabeza = izquierda[i];
    }

    return movimiento;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Uso: " << argv[0] << " <posicion_inicial>\n";
        return 1;
    }

    int posicionInicial = std::atoi(argv[1]);
    if (posicionInicial < 0 || posicionInicial >= NUMERO_CILINDROS) {
        std::cout << "La posicion inicial debe estar entre 0 y " << NUMERO_CILINDROS - 1 << "\n";
        return 1;
    }

    std::srand(std::time(nullptr));
    std::vector<int> solicitudes(NUMERO_SOLICITUDES);
    for (int i = 0; i < NUMERO_SOLICITUDES; i++) {
        solicitudes[i] = std::rand() % NUMERO_CILINDROS;
    }

    long long movFCFS  = fcfs(posicionInicial, solicitudes);
    long long movSCAN  = scan(posicionInicial, solicitudes);
    long long movCSCAN = cscan(posicionInicial, solicitudes);

    std::cout << "Movimiento total FCFS: "  << movFCFS  << "\n";
    std::cout << "Movimiento total SCAN: "  << movSCAN  << "\n";
    std::cout << "Movimiento total C-SCAN: "  << movCSCAN << "\n";

    return 0;
}

  
