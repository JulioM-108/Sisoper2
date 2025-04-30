#include <iostream>

int main(){
  int numero = 0;

  std::cout << "Valor de la variable: " << numero << std::endl;
  std::cout << "Dirección de la variable: " << &numero << std::endl;

  int *modificarnumero = &numero;
  *modificarnumero = 25;

  std::cout << "Valor de la variable: " << numero << std::endl;
  std::cout << "Dirección de la variable: " << &numero << std::endl;
  return 0;
}
