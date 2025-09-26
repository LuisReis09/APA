#include <iostream>
#include "structures.hpp"
#include "utils.hpp"
#include "metodos.hpp"
#include "VND1.hpp"
// #include "VNDHerick.hpp"
#include <time.h>

using namespace std;

Problema p("../exemplos/instancia1.txt", true);

int main()
{
    srand(time(NULL)); // seed para aleatorizar o tamanho do segmento

    cout << "====== Teste de Metodos ======" << endl;
    Solucao s = VizinhoMaisProximo2();
    s.custo_total = CustoTotal(s.rotas);
    cout << "custo sem verificacoes: " << s.custo_total << endl;
    CorrigeSolucao(s.rotas);
    s.custo_total = RVND(s.rotas);
    cout << "custo depois de corrigir: " << s.custo_total << endl;

    if (VerificaSolucao(s.rotas, true))
    {
        cout << "Solucao VALIDA" << endl;
        cout << "Custo: " << s.custo_total << endl;
        cout << "Veiculos usados: " << s.rotas.size() << endl << endl;
        // for (const vector<int> &r : s.rotas)
        // {
        //     for (const int &e : r)
        //     {
        //         cout << e << " ";
        //     }
        //     cout << endl;
        // }
    }
    else
    {
        cout << "Solucao INVALIDA" << endl;
    }

    // s.SalvarSolucao("solucao_so_guloso1.txt");

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

    cout << "====== VND 2 ======" << endl;

    for(int i=0; i < 15; i++){
        cout << "ITER " << i+1 << endl;
        s.custo_total = RVND(s.rotas);
        cout << "Custo Pos-RVND: " << s.custo_total << endl;
        s.custo_total = VND1(s.rotas);
        cout << "Custo Pos-VND1: " << s.custo_total << endl;
        s.SalvarSolucao("iteracao_vnds.txt");
    }

    if (VerificaSolucao(s.rotas, true))
    {
        cout << "Solucao VALIDA" << endl;
        cout << "Custo: " << s.custo_total << endl;
        cout << "Veiculos usados: " << s.rotas.size() << endl;
        // for (const vector<int> &r : s.rotas)
        // {
        //     for (const int &e : r)
        //     {
        //         cout << e << " ";
        //     }
        //     cout << endl;
        // }
        s.SalvarSolucao("solucao_teste_VND1.txt");
    }
    else
    {
        cout << "Solucao INVALIDA" << endl;
    }

    cout << "====== ILS ======" << endl;
    ILS(s.rotas, 10000, 400);
    s.custo_total = CustoTotal(s.rotas);

    if (VerificaSolucao(s.rotas, true))
    {
        cout << "Solucao VALIDA" << endl;
        cout << "Custo: " << s.custo_total << endl;
        cout << "Veiculos usados: " << s.rotas.size() << endl;
        // for (const vector<int> &r : s.rotas)
        // {
        //     for (const int &e : r)
        //     {
        //         cout << e << " ";
        //     }
        //     cout << endl;
        // }
    }
    else
    {
        cout << "Solucao INVALIDA" << endl;
    }

    // VNDIntraInter(s.rotas);
    // s.custo_total = CustoTotal(s.rotas);

    // cout << "Custo Ultimate: " << s.custo_total << endl;
    // s.SalvarSolucao("solucao_ultimate.txt");

    return 0;
}