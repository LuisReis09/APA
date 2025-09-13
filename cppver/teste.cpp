#include "structures.hpp"
#include "utils.hpp"
#include "metodos.hpp"

#include <iostream>

using namespace std;

int main()
{
    SetProblema(ProblemaAleatorio(10, 5, 12, 15));
    PrintProblema("Problema P");
    Solucao s = InsercaoMaisBarata(); // segmentation fault
    PrintSolucao(s, "Solução s");

    return 0;
}