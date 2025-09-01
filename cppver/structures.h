#ifndef STRUCTURES_H
#define STRUCTURES_H
#include <vector>

using namespace std;

class Problema
{
public:
    int qnt_veiculos;
    int capacidade_max;
    vector<int> demandas; // Array de inteiros
    int qnt_estacoes;
    int veiculos_disponiveis;
    vector<vector<int>> matriz_custo;
};

class No
{
public:
    int estacao;
    No *proximo; // Ponteiro para próximo nó
    int soma_demandas_d1;
    int soma_demandas_d2;
    int custo_d1, custo_d2;
};

class Rota
{
public:
    No *rota_i; // Início da lista encadeada
    No *rota_f; // Fim da lista encadeada
    int custo_total_d1, custo_total_d2;
    int direcao_atual;
};

class Solucao
{
public:
    int custo_total;
    int veiculos_usados;
    int veiculos_disponiveis;
    Rota* rotas;
};

#endif