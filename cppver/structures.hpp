#ifndef STRUCTURES_H
#define STRUCTURES_H
// #include "utils.h"
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
    No *proximo;  // Ponteiro para próximo nó
    No *anterior; // Ponteiro para o nó anterior
    int soma_demandas_d1;
    int soma_demandas_d2;
    int custo_d1, custo_d2; // Custo d1: Daquele nó para o proximo  ---- Custo d2: Daquele nó para o anterior

    No(int e = 0) : estacao(e), proximo(nullptr), anterior(nullptr),
                    soma_demandas_d1(0), soma_demandas_d2(0),
                    custo_d1(0), custo_d2(0) {}
};

typedef enum
{
    INICIO_FIM,
    FIM_INICIO
} DirecaoRota;

class Rota
{
public:
    No *rota_i; // Início da lista encadeada
    No *rota_f; // Fim da lista encadeada
    int custo_total_d1, custo_total_d2;
    int rotaTam;
    DirecaoRota direcao_atual;

    // Construtor padrão
    Rota() : rota_i(nullptr), rota_f(nullptr),
             custo_total_d1(0), custo_total_d2(0),
             rotaTam(0), direcao_atual(INICIO_FIM) {}

    // Construtor de cópia (deep copy)
    Rota(const Rota &other)
    {
        custo_total_d1 = other.custo_total_d1;
        custo_total_d2 = other.custo_total_d2;
        rotaTam = other.rotaTam;
        direcao_atual = other.direcao_atual;
        rota_i = copiaLista(other.rota_i, other.rota_f, &rota_f);
    }

    // Operador de atribuição (deep copy)
    Rota &operator=(const Rota &other)
    {
        if (this == &other)
            return *this;

        limpa(); // libera a lista atual

        custo_total_d1 = other.custo_total_d1;
        custo_total_d2 = other.custo_total_d2;
        rotaTam = other.rotaTam;
        direcao_atual = other.direcao_atual;
        rota_i = copiaLista(other.rota_i, other.rota_f, &rota_f);

        return *this;
    }

    void PushBack(int estacao)
    {
        No *no_estacao = new No();
        no_estacao->estacao = estacao;

        if (this->rota_i == nullptr)
        {
            this->rota_i = new No(0);
            this->rota_f = new No(0);
            this->rotaTam = 0;
            this->direcao_atual = DirecaoRota::INICIO_FIM;
        }

        if (this->rota_f->anterior == nullptr)
        {
            this->rota_i->proximo = no_estacao;
            no_estacao->anterior = this->rota_i;
            this->rota_f->anterior = no_estacao;
            no_estacao->proximo = this->rota_f;

            no_estacao->custo_d1 = p.matriz_custo[estacao][0];
            no_estacao->custo_d2 = p.matriz_custo[0][estacao];

            no_estacao->soma_demandas_d1 =
                no_estacao->soma_demandas_d2 = p.demandas[estacao - 1];

            this->custo_total_d1 = no_estacao->custo_d2 + no_estacao->custo_d1;
            this->custo_total_d2 = no_estacao->custo_d1 + no_estacao->custo_d2;

            no_estacao->anterior->custo_d2 += p.matriz_custo[0][no_estacao->estacao] + p.matriz_custo[no_estacao->estacao][0];
            no_estacao->proximo->custo_d2 += p.matriz_custo[0][no_estacao->estacao];
        }
        else
        {

            No *aux = this->rota_f->anterior;
            aux->proximo = no_estacao;
            no_estacao->anterior = aux;
            this->rota_f->anterior = no_estacao;
            no_estacao->proximo = this->rota_f;

            // valores de custo e demanda do nó inserido
            no_estacao->custo_d1 = p.matriz_custo[estacao][no_estacao->proximo->estacao];
            no_estacao->custo_d2 = p.matriz_custo[no_estacao->anterior->estacao][estacao];

            no_estacao->soma_demandas_d1 = no_estacao->anterior->soma_demandas_d1 + p.demandas[estacao - 1];
            no_estacao->soma_demandas_d2 = p.demandas[estacao - 1];

            // valores de custo e demanda dos nós afetados (a terminar)
            no_estacao->anterior->custo_d2 += p.matriz_custo[0][no_estacao->estacao] + p.matriz_custo[no_estacao->estacao][no_estacao->anterior->estacao];
            no_estacao->proximo->custo_d2 += p.matriz_custo[0][no_estacao->estacao];

            // valores de custo da rota atualizados(a terminar)
            int custo_da_insercao;
            this->custo_total_d1;
        }

        this->rotaTam++;
    }

    // Destrutor
    ~Rota()
    {
        limpa();
    }

private:
    // Copia profunda da lista encadeada
    No *copiaLista(No *inicio, No *fim, No **novoFim)
    {
        if (!inicio)
        {
            *novoFim = nullptr;
            return nullptr;
        }

        No *novoInicio = new No(*inicio); // construtor de cópia de No
        novoInicio->anterior = nullptr;

        No *atualNovo = novoInicio;
        No *atualVelho = inicio->proximo;

        while (atualVelho)
        {
            No *novoNo = new No(*atualVelho); // copia todos os campos
            atualNovo->proximo = novoNo;
            novoNo->anterior = atualNovo;

            atualNovo = novoNo;

            if (atualVelho == fim)
            {
                *novoFim = atualNovo;
                break;
            }

            atualVelho = atualVelho->proximo;
        }

        return novoInicio;
    }

    // Libera memória da lista
    void limpa()
    {
        No *atual = rota_i;
        while (atual)
        {
            No *prox = atual->proximo;
            delete atual;
            atual = prox;
        }
        rota_i = rota_f = nullptr;
        rotaTam = 0;
        custo_total_d1 = custo_total_d2 = 0;
    }
};

class Solucao
{
public:
    int custo_total;
    int veiculos_usados;
    int veiculos_disponiveis;
    vector<Rota> rotas;
};

#endif