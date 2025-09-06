#include <iostream>
#include "structures.h"
#include "utils.h"
#include <utility>

int main(){
    Problema p = ProblemaAleatorio(5, 3, 8, 20);

    PrintProblema(p, "Problema p");

    pair<int, int> trianguloInicial = MelhoresVertices(p);

    cout << "Triangulo inicial: " << endl;
    cout << 0 << " -> ";
    cout << trianguloInicial.first << " -> ";
    cout << trianguloInicial.second << " -> ";
    cout << 0 << endl;

    return 0;
}