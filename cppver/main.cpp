#include <iostream>
#include "structures.h"
#include "utils.h"
#include <utility>

int main(){
    SetProblema(ProblemaAleatorio(5, 3, 8, 20));
    PrintProblema("Problema p");
    pair<int, int> trianguloInicial = MelhoresVertices();

    cout << "Triangulo inicial: " << endl;
    cout << 0 << " -> ";
    cout << trianguloInicial.first << " -> ";
    cout << trianguloInicial.second << " -> ";
    cout << 0 << endl;

    return 0;
}