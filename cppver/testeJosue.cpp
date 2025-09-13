#include "metodos.hpp"
#include "utils.hpp"
#include "threadpool.hpp"
#include "structures.hpp"
#include <iostream>
using namespace std;

int main(){
    Problema p = LerDados("../exemplos/exemplo1.txt");
    SetProblema(p);

    Solucao sol = VizinhoMaisProximo();
}