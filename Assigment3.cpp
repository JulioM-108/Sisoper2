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

  
