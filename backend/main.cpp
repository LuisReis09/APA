#include <iostream>
#include <fstream>
#include "structures.hpp"
#include "utils.hpp"
#include "metodos.hpp"
#include <ctime>
#include <chrono>

#define MAX 1000000
#define N_TESTES 15

using namespace std;
using namespace std::chrono;
Problema p;

int main()
{
    const string nome_arquivo   = "n12_q20";    // deve ler de ../exemplos/ + nome_arquivo + ".txt"
    const int valor_otimo       = 15700;        // -1 se não houver valor ótimo conhecido
    const int iteracoes_teste   = 10;           // quantas vezes eu devo executar esse teste de verificação 

    srand(time(NULL));
    int menor_vmp, menor_imb, menor_vnd1, menor_vnd2, menor_ils;
    int soma_vmp, soma_imb, soma_vnd1, soma_vnd2, soma_ils;
    int media_vmp, media_imb, media_vnd1, media_vnd2, media_ils;
    float media_tempExec_vmp, media_tempExec_imb, media_tempExec_vnd1, media_tempExec_vnd2, media_tempoExec_ils;
    float soma_tempo_vmp, soma_tempo_imb, soma_tempo_vnd1, soma_tempo_vnd2, soma_tempo_ils;
    
    string path = "./exemplos/" + nome_arquivo + ".txt";
    p = Problema(path, true);
    soma_vmp = soma_imb = soma_vnd1 = soma_vnd2 = soma_ils = 0;
    soma_tempo_imb = soma_tempo_vmp = soma_tempo_vnd1 = soma_tempo_vnd2 = soma_tempo_ils = 0;
    menor_vmp = menor_imb = menor_vnd1 = menor_vnd2 = menor_ils = MAX;

    for (int i = 1; i <= iteracoes_teste; i++)
    {
        auto antes = high_resolution_clock::now();
        Solucao sol = VizinhoMaisProximo();
        auto depois = high_resolution_clock::now();

        sol.custo_total = CustoTotal(sol.rotas);
        soma_vmp += sol.custo_total;
        soma_tempo_vmp += (float)duration_cast<milliseconds>(depois - antes).count() / 1000.0f;
        if (sol.custo_total < menor_vmp)
        {
            menor_vmp = sol.custo_total;
        }

        sol.SalvarSolucao("./solucoes/" + nome_arquivo + "/vmp/" + to_string(i) + ".txt");

        antes = high_resolution_clock::now();
        Solucao sol_imb = IMB();
        depois = high_resolution_clock::now();
        sol_imb.custo_total = CustoTotal(sol_imb.rotas);
        soma_imb += sol_imb.custo_total;
        soma_tempo_imb += (float)duration_cast<milliseconds>(depois - antes).count() / 1000.0f;

        if (sol_imb.custo_total < menor_imb)
        {
            menor_imb = sol_imb.custo_total;
        }

        sol_imb.SalvarSolucao("./solucoes/" + nome_arquivo + "/imb/" + to_string(i) + ".txt");

        antes = high_resolution_clock::now();
        VNDIntraInter(sol.rotas);
        depois = high_resolution_clock::now();
        sol.custo_total = CustoTotal(sol.rotas);
        soma_vnd1 += sol.custo_total;
        soma_tempo_vnd1 += (float)duration_cast<milliseconds>(depois - antes).count() / 1000.0f;
        if (sol.custo_total < menor_vnd1)
        {
            menor_vnd1 = sol.custo_total;
        }
        sol.SalvarSolucao("./solucoes/" + nome_arquivo + "/vnd1/" + to_string(i) + ".txt");

        antes = high_resolution_clock::now();
        VND2(sol.rotas);
        depois = high_resolution_clock::now();
        sol.custo_total = CustoTotal(sol.rotas);
        soma_vnd2 += sol.custo_total;
        soma_tempo_vnd2 += (float)duration_cast<milliseconds>(depois - antes).count() / 1000.0f;
        if (sol.custo_total < menor_vnd2)
        {
            menor_vnd2 = sol.custo_total;
        }

        sol.SalvarSolucao("./solucoes/" + nome_arquivo + "/vnd2/" + to_string(i) + ".txt");

        antes = high_resolution_clock::now();
        ILS(sol.rotas, 2000, 150);
        depois = high_resolution_clock::now();
        sol.custo_total = CustoTotal(sol.rotas);
        soma_ils += sol.custo_total;
        soma_tempo_ils += (float)duration_cast<milliseconds>(depois - antes).count() / 1000.0f;
        if (sol.custo_total < menor_ils)
        {
            menor_ils = sol.custo_total;
        }
        sol.SalvarSolucao("./solucoes/" + nome_arquivo + "/ils/" + to_string(i) + ".txt");

    }
    
    media_vmp = soma_vmp / iteracoes_teste;
    media_imb = soma_imb / iteracoes_teste;
    media_vnd1 = soma_vnd1 / iteracoes_teste;
    media_vnd2 = soma_vnd2 / iteracoes_teste;
    media_ils = soma_ils / iteracoes_teste;

    media_tempExec_imb = soma_tempo_imb / iteracoes_teste;
    media_tempExec_vmp = soma_tempo_vmp / iteracoes_teste;
    media_tempExec_vnd1 = soma_tempo_vnd1 / iteracoes_teste;
    media_tempExec_vnd2 = soma_tempo_vnd2 / iteracoes_teste;
    media_tempoExec_ils = soma_tempo_ils / iteracoes_teste;

    cout << "\n=== ARQUIVO: " << nome_arquivo << " ===" << endl;
    cout << "Valor otimo: " << valor_otimo << endl << endl;

    cout << "Media Vizinho Mais Proximo: " << media_vmp << endl;
    cout << "Menor Valor Vizinho Mais Proximo: " << menor_vmp << endl;
    cout << "Tempo medio de execucao para Vizinho Mais proximo (em ms): " << media_tempExec_vmp * 1000.0f << endl;
    cout << "GAP para a solucao otima: " << GAP(menor_vmp, valor_otimo) << "%" << endl;
    cout << "- - - - -\n";

    cout << "Media Insercao Mais Barata: " << media_imb << endl;
    cout << "Menor Valor Insercao Mais Barata: " << menor_imb << endl;
    cout << "Tempo medio de execucao para Insercao Mais Barata (em ms): " << media_tempExec_imb * 1000.0f << endl;
    cout << "GAP para a solucao otima: " << GAP(menor_imb, valor_otimo) << "%" << endl;
    cout << "- - - - -\n";

    cout << "Media VND1: " << media_vnd1 << endl;
    cout << "Menor Valor VND1: " << menor_vnd1 << endl;
    cout << "Tempo medio de execucao para VNDInterIntra (em ms): " << media_tempExec_vnd1 * 1000.0f << endl;
    cout << "GAP para a solucao otima: " << GAP(menor_vnd1, valor_otimo) << "%" << endl;
    cout << "- - - - -\n";

    cout << "Media VND2: " << media_vnd2 << endl;
    cout << "Menor Valor VND2: " << menor_vnd2 << endl;
    cout << "Tempo medio de execucao para VND2 (em ms): " << media_tempExec_vnd2 * 1000.0f << endl;
    cout << "GAP para a solucao otima: " << GAP(menor_vnd2, valor_otimo) << "%" << endl;
    cout << "- - - - -\n";

    cout << "Media ILS: " << media_ils << endl;
    cout << "Menor Valor ILS: " << menor_ils << endl;
    cout << "Tempo medio de execucao para ILS (em ms): " << media_tempoExec_ils * 1000.0f << endl;
    cout << "GAP para a solucao otima: " << GAP(menor_ils, valor_otimo) << "%" << endl;
    cout << "- - - - -\n";

    return 0;
}