#include <iostream>
#include "structures.hpp"
#include "metodos.hpp"
#include "utils.hpp"
#include <time.h>

using namespace std;

Problema p("../exemplos/instancia3.txt", true);

int main(){
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

    s.SalvarSolucao("solucao_so_guloso3.txt");

    // cout << "====== Melhorando Solucao ======" << endl;
    // MelhorarSolucao(s.rotas);
    // s.custo_total = CustoTotal(s.rotas);

    // if(VerificaSolucao(s.rotas, true)){
    //     cout << "Solucao VALIDA" << endl;
    //     cout << "Custo: " << s.custo_total << endl;
    //     cout << "Veiculos usados: " << s.rotas.size() << endl;
    //     for(const vector<int>& r : s.rotas){
    //         for(const int& e : r){
    //             cout << e << " ";
    //         }
    //         cout << endl;
    //     }
    // }else{
    //     cout << "Solucao INVALIDA" << endl;
    // }

    cout << "====== VND Intra e Inter-Rotas ======" << endl;
    VNDIntraInter(s.rotas);
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
        s.SalvarSolucao("solucao_teste_VND3.txt");
    }else{
        cout << "Solucao INVALIDA" << endl;
    }
    
    cout << "====== ILS ======" << endl;
    ILS(s.rotas);
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
        s.SalvarSolucao("solucao_teste_ILS3.txt");
    }else{
        cout << "Solucao INVALIDA" << endl;
    }

    return 0;
}