#include <iostream>
#include <iomanip>
#include <string>
#include <ctime>
#include "structures.hpp"
#include "metodos.hpp"
#include "utils.hpp" 

using namespace std;

Problema p("../exemplos/instancia1.txt", true);

typedef struct
{
    string tipo;
    int valor_solucao;
    float tempo;
    float gap;

} dadosTipoSolucao;


int main()
{

    dadosTipoSolucao instancias[3];

    dadosTipoSolucao dados_VMP;
    dados_VMP.tipo = "Vizinho Mais Proximo";

    dadosTipoSolucao dados_MP;
    dados_MP.tipo = "Melhorar Solucao";

    dadosTipoSolucao dados_VND;
    dados_VND.tipo = "VND";

    dadosTipoSolucao dados_ILS;
    dados_ILS.tipo = "ILS";

    cout << "====== Teste de Metodos ======" << endl;

    clock_t antes_VMP = clock();
    Solucao s = VizinhoMaisProximo();
    clock_t depois_VMP = clock() - antes_VMP;
    dados_VMP.tempo = (float)(depois_VMP) / CLOCKS_PER_SEC;

    s.custo_total = CustoTotal(s.rotas);

    if (VerificaSolucao(s.rotas, true))
    {
        cout << "Solucao VALIDA" << endl;
        cout << "Custo: " << s.custo_total << endl;
        cout << "Veiculos usados: " << s.rotas.size() << endl;
        dados_VMP.valor_solucao = s.custo_total;

        for (const vector<int> &r : s.rotas)
        {
            for (const int &e : r)
            {
                cout << e << " ";
            }
            cout << endl;
        }
    }
    else
    {
        cout << "Solucao INVALIDA" << endl;
    }

    s.SalvarSolucao("solucao_so_guloso1.txt");

    cout << "====== Melhorando Solucao ======" << endl;
    Solucao solucao_melhorarSolucao = s;
    clock_t antes_MelhorarSolucao = clock();
    MelhorarSolucao(solucao_melhorarSolucao.rotas, solucao_melhorarSolucao.custo_total);
    clock_t depois_MelhorarSolucao = clock() - antes_MelhorarSolucao;
    dados_MP.tempo = (float)(depois_MelhorarSolucao) / CLOCKS_PER_SEC;

    s.custo_total = CustoTotal(s.rotas);

    if (VerificaSolucao(s.rotas, true))
    {
        cout << "Solucao VALIDA" << endl;
        cout << "Custo: " << s.custo_total << endl;
        cout << "Veiculos usados: " << s.rotas.size() << endl;
        dados_MP.valor_solucao = solucao_melhorarSolucao.custo_total;
        for (const vector<int> &r : s.rotas)
        {
            for (const int &e : r)
            {
                cout << e << " ";
            }
            cout << endl;
        }
    }
    else
    {
        cout << "Solucao INVALIDA" << endl;
    }

    cout << "====== VND Intra e Inter-Rotas ======" << endl;
    clock_t pre_VND = clock();
    VNDIntraInter(s.rotas);
    clock_t pos_VND = clock() - pre_VND;
    dados_VND.tempo = (float)pos_VND / CLOCKS_PER_SEC;

    s.custo_total = CustoTotal(s.rotas);
    dados_VND.valor_solucao = s.custo_total;

    if (VerificaSolucao(s.rotas, true))
    {
        cout << "Solucao VALIDA" << endl;
        cout << "Custo: " << s.custo_total << endl;
        cout << "Veiculos usados: " << s.rotas.size() << endl;
        for (const vector<int> &r : s.rotas)
        {
            for (const int &e : r)
            {
                cout << e << " ";
            }
            cout << endl;
        }
        s.SalvarSolucao("solucao_teste_VND1.txt");
    }
    else
    {
        cout << "Solucao INVALIDA" << endl;
    }

    cout << "====== ILS ======" << endl;
    clock_t pre_ILS = clock();
    ILS(s.rotas);
    clock_t pos_ILS = clock() - pre_ILS;
    dados_ILS.tempo = (float)pos_ILS / CLOCKS_PER_SEC;

    s.custo_total = CustoTotal(s.rotas);
    dados_ILS.valor_solucao = s.custo_total;

    if (VerificaSolucao(s.rotas, true))
    {
        cout << "Solucao VALIDA" << endl;
        cout << "Custo: " << s.custo_total << endl;
        cout << "Veiculos usados: " << s.rotas.size() << endl;
        for (const vector<int> &r : s.rotas)
        {
            for (const int &e : r)
            {
                cout << e << " ";
            }
            cout << endl;
        }
        s.SalvarSolucao("solucao_teste_ILS1.txt");
    }
    else
    {
        cout << "Solucao INVALIDA" << endl;
    }

    // Em tese, o "valor ótimo é o valor encontrado pelo ILS"
    float valor_final = (float)s.custo_total;

    // Calculo dos gaps
    dados_ILS.gap = 100;
    dados_VMP.gap = (((float)dados_VMP.valor_solucao - valor_final) / valor_final) * 100;
    dados_VND.gap = (((float)dados_VND.valor_solucao - valor_final) / valor_final) * 100;
    dados_MP.gap = (((float)dados_MP.valor_solucao - valor_final) / valor_final) * 100;

    // Printando resultados desta instancia
    // Larguras das colunas
    int w_inst = 12;
    int w_col = 10;

    // Linha de cima
    cout << string(w_inst + w_col * 7 + 8, '-') << "\n";

    // Cabeçalho 2

    std::cout
        << setw(w_inst) << "Tipo";

    for (int i = 0; i < 4; i++)
    {
        cout
            << setw(w_col) << "valor" << setw(w_col) << "tempo" << setw(w_col) << " gap ";
    }
    cout
        << "\n";

    dadosTipoSolucao dados_todos[4] = {dados_VMP, dados_MP, dados_VND, dados_ILS};
    for (dadosTipoSolucao dados : dados_todos)
    {
        cout << setw(w_col) << dados.valor_solucao
             << setw(w_col) << dados.tempo
             << setw(w_col)
             << dados.gap
             << "\n";
    }

    // Linha final
    cout << std::string(w_inst + w_col * 7 + 8, '-') << "\n";
    cout << "Valor da solução final: " << valor_final;

    return 0;
}