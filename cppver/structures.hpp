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

    No(int e = 0) : estacao(e), proximo(nullptr), anterior(nullptr) {}
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
    int rotaTam; // Número de estações na rota (excluindo os depósitos)
    DirecaoRota direcao_atual;

    // Construtor padrão
    Rota() : custo_total_d1(0), custo_total_d2(0),
             rotaTam(0), direcao_atual(INICIO_FIM) {
                rota_i = new No(0); // Nó inicial (depósito)
                rota_f = new No(0); // Nó final (depósito)
                rota_i->proximo = rota_f;
                rota_f->anterior = rota_i;
             }

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

        int stationAtIndex(int posicao)
    {
        if (posicao > (rotaTam + 1) || posicao < 0)
            return -1;

        No *atual = rota_i;
        int index_atual = 0;
        while (index_atual != posicao)
        {
            atual = atual->proximo;
            index_atual++;
        }
        return index_atual;
    }

    void InsertBegin(int estacao)
    {
        extern Problema p;
        No *novo = new No(estacao);
        novo->proximo = rota_i->proximo;
        novo->anterior = rota_i;
        rota_i->proximo->anterior = novo;
        rota_i->proximo = novo;
        rotaTam++;

        // Retira o custo matriz[0][rota_i->proximo->estacao] do custo_total_d1
        // E incrementa o custo do galpão até a nova estação + o custo da nova estação até a antiga primeira estação
        custo_total_d1 -= p.matriz_custo[0][novo->proximo->estacao];
        custo_total_d1 += p.matriz_custo[0][estacao] + p.matriz_custo[estacao][novo->proximo->estacao];

        // Retira o custo matriz[rota_i->proximo->estacao][0] do custo_total_d2
        // E incrementa o custo da antiga primeira estação até a nova estação + o custo da nova estação até o galpão
        custo_total_d2 -= p.matriz_custo[novo->proximo->estacao][0];
        custo_total_d2 += p.matriz_custo[novo->proximo->estacao][estacao] + p.matriz_custo[estacao][0];

        // Agora temos a estação inserida e o novo custo atualizado
    }

    void InsertEnd(int estacao)
    {
        extern Problema p;
        No *novo = new No(estacao);
        novo->anterior = rota_f->anterior;
        novo->proximo = rota_f;
        rota_f->anterior->proximo = novo;
        rota_f->anterior = novo;
        rotaTam++;

        // Retira o custo matriz[rota_f->anterior->estacao][0] do custo_total_d1
        // E incrementa o custo da antiga última estação até a nova estação + o custo da nova estação até o galpão
        custo_total_d1 -= p.matriz_custo[novo->anterior->estacao][0];
        custo_total_d1 += p.matriz_custo[novo->anterior->estacao][estacao] + p.matriz_custo[estacao][0];

        // Retira o custo matriz[0][rota_f->anterior->estacao] do custo_total_d2
        // E incrementa o custo do galpão até a nova estação + o custo da nova estação até a antiga última estação
        custo_total_d2 -= p.matriz_custo[0][novo->anterior->estacao];
        custo_total_d2 += p.matriz_custo[0][estacao] + p.matriz_custo[estacao][novo->anterior->estacao];

        // Agora temos a estação inserida e o novo custo atualizado
    }

    void InsertAt(int posicao, int estacao)
    {
        extern Problema p;
        if (posicao < 1 || posicao > (rotaTam+1))
            return; // Posição inválida

        // Se a posição for mais perto do início, percorre a partir do início
        // Caso contrário, percorre a partir do fim

        No *novo = new No(estacao);
        No *atual;

        if (posicao <= rotaTam / 2)
        {
            atual = rota_i->proximo;
            for (int i = 1; i < posicao; i++)
                atual = atual->proximo;

            // Quando eu chego na posição que eu quero, a estação vai ser o novo elemento daquela posição
            // Ou seja, o atual que está na posição posicao vai ser empurrado para frente, ficando na posição posicao + 1
            novo->proximo = atual;
            novo->anterior = atual->anterior;
            atual->anterior->proximo = novo;
            atual->anterior = novo;
            rotaTam++;
        }
        else
        {
            atual = rota_f->anterior;
            for (int i = rotaTam; i > posicao; i--)
                atual = atual->anterior;

            // Quando eu chego na posição que eu quero, a estação vai ser o novo elemento daquela posição
            // Ou seja, o atual que está na posição posicao vai ser empurrado para frente, ficando na posição posicao + 1
            novo->proximo = atual;
            novo->anterior = atual->anterior;
            atual->anterior->proximo = novo;
            atual->anterior = novo;
            rotaTam++;
        }

        // Atualiza custos da direcao INICIO_FIM
        custo_total_d1 -= p.matriz_custo[novo->anterior->estacao][atual->estacao];
        custo_total_d1 += p.matriz_custo[novo->anterior->estacao][estacao] + p.matriz_custo[estacao][atual->estacao];

        // Atualiza custos da direcao FIM_INICIO
        custo_total_d2 -= p.matriz_custo[atual->estacao][novo->anterior->estacao];
        custo_total_d2 += p.matriz_custo[atual->estacao][estacao] + p.matriz_custo[estacao][novo->anterior->estacao];
    }

    /*
        Recebe um vetor de estações e insere todas na posição indicada.
        Atualiza os custos da rota de acordo com as inserções.
    */
    void InsertArrayAt(int posicao, vector<int> estacoes)
    {
        extern Problema p;
        No *novos_nos[estacoes.size()];
        novos_nos[0] = new No(estacoes[0]);

        int custo_insercao = 0, custo_insercao_2 = 0;

        for (size_t i = 1; i < estacoes.size(); i++)
        {
            novos_nos[i] = new No(estacoes[i]);
            novos_nos[i - 1]->proximo = novos_nos[i];
            novos_nos[i]->anterior = novos_nos[i - 1];
            custo_insercao += p.matriz_custo[estacoes[i - 1]][estacoes[i]];
            custo_insercao_2 += p.matriz_custo[estacoes[i]][estacoes[i - 1]];
        }

        No *atual;
        if (posicao <= rotaTam / 2)
        {
            atual = rota_i->proximo;
            for (int i = 1; i < posicao; i++)
                atual = atual->proximo;

            custo_insercao -= p.matriz_custo[atual->anterior->estacao][atual->estacao];
            custo_insercao_2 -= p.matriz_custo[atual->estacao][atual->anterior->estacao];

            // Conecta o primeiro novo nó ao nó anterior ao atual
            novos_nos[0]->anterior = atual->anterior;
            atual->anterior->proximo = novos_nos[0];
            custo_insercao += p.matriz_custo[atual->anterior->estacao][novos_nos[0]->estacao];
            custo_insercao_2 += p.matriz_custo[novos_nos[0]->estacao][atual->anterior->estacao];

            // Conecta o último novo nó ao nó atual
            novos_nos[estacoes.size() - 1]->proximo = atual;
            atual->anterior = novos_nos[estacoes.size() - 1];
            custo_insercao += p.matriz_custo[atual->anterior->estacao][atual->estacao];
            custo_insercao_2 += p.matriz_custo[atual->estacao][atual->anterior->estacao];

            // Atualiza os custos da rota
            custo_total_d1 += custo_insercao;
            custo_total_d2 += custo_insercao_2;

            // Incrementa a quantidade de estações na rota
            rotaTam += estacoes.size();
        }
        else
        {
            atual = rota_f->anterior;
            for (int i = rotaTam; i > posicao; i--)
                atual = atual->anterior;

            custo_insercao -= p.matriz_custo[atual->anterior->estacao][atual->estacao];
            custo_insercao_2 -= p.matriz_custo[atual->estacao][atual->anterior->estacao];

            // Conecta o primeiro novo nó ao nó anterior ao atual
            novos_nos[0]->anterior = atual->anterior;
            atual->anterior->proximo = novos_nos[0];
            custo_insercao += p.matriz_custo[atual->anterior->estacao][novos_nos[0]->estacao];
            custo_insercao_2 += p.matriz_custo[novos_nos[0]->estacao][atual->anterior->estacao];

            // Conecta o último novo nó ao nó atual
            novos_nos[estacoes.size() - 1]->proximo = atual;
            atual->anterior = novos_nos[estacoes.size() - 1];
            custo_insercao += p.matriz_custo[atual->anterior->estacao][atual->estacao];
            custo_insercao_2 += p.matriz_custo[atual->estacao][atual->anterior->estacao];

            // Atualiza os custos da rota
            custo_total_d1 += custo_insercao;
            custo_total_d2 += custo_insercao_2;

            // Incrementa a quantidade de estações na rota
            rotaTam += estacoes.size();
        }
        
    }

    void RemoveAt(int posicao){
        extern Problema p;
        if (posicao < 1 || posicao > rotaTam)
            return; // Posição inválida

        No *atual;

        if (posicao <= rotaTam / 2)
        {
            atual = rota_i->proximo;
            for (int i = 1; i < posicao; i++)
                atual = atual->proximo;

            atual->anterior->proximo = atual->proximo;
            atual->proximo->anterior = atual->anterior;
            rotaTam--;

        }
        else
        {
            atual = rota_f->anterior;
            for (int i = rotaTam; i > posicao; i--)
                atual = atual->anterior;

            atual->anterior->proximo = atual->proximo;
            atual->proximo->anterior = atual->anterior;
            rotaTam--;
        }

        // Atualiza custos da direcao INICIO_FIM
        custo_total_d1 -= p.matriz_custo[atual->anterior->estacao][atual->estacao];
        custo_total_d1 -= p.matriz_custo[atual->estacao][atual->proximo->estacao];
        custo_total_d1 += p.matriz_custo[atual->anterior->estacao][atual->proximo->estacao];

        // Atualiza custos da direcao FIM_INICIO
        custo_total_d2 -= p.matriz_custo[atual->proximo->estacao][atual->estacao];
        custo_total_d2 -= p.matriz_custo[atual->estacao][atual->anterior->estacao];
        custo_total_d2 += p.matriz_custo[atual->proximo->estacao][atual->anterior->estacao];

        delete atual; // Libera memória do nó removido
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

typedef struct{
    Solucao solucao;
    int i_rota;
    Rota rota_retorno;
} VND_attr;

// Utilizado em combinação às threads do ILS
struct ParametrosILS {
    int id_execucao;          // só para identificar
    Solucao solucao;          // passa por valor (cópia)
    int max_iteracoes;
    int max_sem_melhora;
    Solucao* resultado;       // onde salvar a solução final
    int pool_count;           // Utilizado para determinar quantas threads serão chamadas 
};


#endif