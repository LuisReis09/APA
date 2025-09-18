#include <iostream>
#include "structures.hpp"
#include "metodos.hpp"
#include "utils.hpp"
#include <time.h>
// #include "threadpool.hpp"

using namespace std;

Problema p("../exemplos/instancia3.txt", true);

int main(){
    // ThreadPool pool(5);
    srand(time(NULL)); // seed para aleatorizar o tamanho do segmento

    cout << "====== Teste de Metodos ======" << endl;
    Solucao s = VizinhoMaisProximo();
    s.custo_total = CustoTotal(s.rotas);

    if(VerificaSolucao(s.rotas, true)){
        cout << "Solucao VALIDA" << endl;
        cout << "Custo: " << s.custo_total << endl;
        cout << "Veiculos usados: " << s.rotas.size() << endl;
        for(const vector<int>& r : s.rotas){
            for(const int& e : r){
                cout << e << " ";
            }
            cout << endl;
        }
    }else{
        cout << "Solucao INVALIDA" << endl;
    }

    int iter = 500;
    while(iter--){
        cout << "Iteracao " << (500 - iter) << endl;
        MelhorarSolucao(s.rotas, s.custo_total);
        s.custo_total = CustoTotal(s.rotas);
        VNDIntraInter(s.rotas);
        s.custo_total = CustoTotal(s.rotas);
    }

    cout << "====== Solucao Final ======" << endl;
    s.SalvarSolucao("solucao_teste_final.sol");
    
    
    return 0;
}