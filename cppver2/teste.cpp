#include <iostream>
#include "structures.hpp"
#include "metodos.hpp"
#include "utils.hpp"
// #include "threadpool.hpp"

using namespace std;

Problema p("../exemplos/instancia3.txt", true);

int main(){
    // ThreadPool pool(5);

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

    
    
    s.SalvarSolucao("solucao_so_guloso.txt");

    cout << "======= Melhora Solucao =======" << endl;
    MelhorarSolucao(s.rotas, s.custo_total);
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


    cout << "====== VND ======" << endl;
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
    }else{
        cout << "Solucao INVALIDA" << endl;
    }

    // for(int i = 0; i < 5; i++){
    //     ThreadData *data = new ThreadData(i, s.rotas, 10000, 1000, s.custo_total, pool);
        
    //     cout << "Iniciando thread " << i << endl;
    //     pool.EnqueueTask(ThreadILS, (void *) data);
    //     cout << "Thread " << i << " iniciada." << endl;
    // }

    // pool.WaitAllThreads();

    // cout << "====== ILS ======" << endl;
    // ILS(s.rotas);
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
    //     s.SalvarSolucao("solucao_teste_ils.txt");
    // }else{
    //     cout << "Solucao INVALIDA" << endl;
    // }

    cout << "====== VND ======" << endl;
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
        s.SalvarSolucao("solucao_teste.txt");
    }else{
        cout << "Solucao INVALIDA" << endl;
    }


    return 0;
}