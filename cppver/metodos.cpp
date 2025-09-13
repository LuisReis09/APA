#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <random>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cmath>
#include <utility>
#include <thread>
#include <map>
#include <algorithm>
#include <mutex>
#include <barrier>
#include "metodos.hpp"
#include "threadpool.hpp"
#include "utils.hpp"
#include <pthread.h>
using namespace std;

// ================================================================================== //
//                               Variáveis Globais                                    //
// ================================================================================== //
// Problema p; // p já é importado de utils.hpp
Solucao melhor_solucao;
pthread_mutex_t mutex_melhorSolucaoCheck_ILS = PTHREAD_MUTEX_INITIALIZER; // Mutex do ILS
pthread_barrier_t barreira_ILS;

void setSolucao(Solucao solucao_recebida)
{
    melhor_solucao = solucao_recebida;
}

Solucao VizinhoMaisProximo()
{
    /*
        Implementa o algoritmo de vizinho mais próximo para cada conjunto
        Retorna a lista de rotas (listas de índices) e o custo total
    */

    vector<vector<int>> rotas(p.qnt_veiculos, vector<int>(0));
    vector<int> necessidades_rotas(p.qnt_veiculos, 0);
    int custo_total = 0;

    // Inicialização das estações que precisam ser visitadas
    vector<int> restam_visitar;
    for (int i = 1; i < p.matriz_custo[0].size(); i++)
        restam_visitar.push_back(i);

    for (int i = 0; i < p.qnt_veiculos; i++)
    {
        necessidades_rotas.push_back(0);
        rotas[i].push_back(0);
    }

    // Monta fila de prioridade por rota
    // tipo {{1, 2}, {3, 4}}
    map<int, int> fila;

    // Para que uma estação seja visitavel, as somas das necessidades
    // Devem estar entre [-p.capacidade_max, p.capacidade_max]
    for (int estacao : restam_visitar)
        fila[estacao] = p.matriz_custo[0][estacao]; // {id_estacao : custo}

    // Cada rota guarda sua fila de prioridade de inserções possíveis
    // Caso haja conflito de interesses, a inserção mais barata global
    // Estrutura do tipo: {1 : {2 : 5}}
    map<int, map<int, int>> fila_prioridade; // Se trata de um mapa de mapas

    // Cada fila de prioridade recebe uma cópia de fila
    for (int r = 0; r < rotas.size(); r++)
        fila_prioridade[r] = fila; // Estrutura do tipo {id_rota : {{1 : 2}, id_rota : {estacao : custo} }}

    vector<int> rotas_a_atualizar;
    for (int id_rota = 0; id_rota < rotas.size(); id_rota++)
        rotas_a_atualizar.push_back(id_rota);

    for (int i : restam_visitar)
        printf("%d\t", i);
    cout << "\nTamanho de restam_visitar: " << restam_visitar.size() << "\n";

    // Enquanto houver estações para visitar
    while (restam_visitar.size())
    {
        // Melhor rota é definida com
        // 1. Menor custo de inserção
        // 2. Mais perto de carga 0
        // encontra a melhor rota em rotas_a_atualizar
        auto melhor_it = std::min_element(
            rotas_a_atualizar.begin(),
            rotas_a_atualizar.end(),
            [&](int a, int b)
            {
                // pega o primeiro par (estacao, custo) de cada fila de prioridade
                if (fila_prioridade.at(a).empty() || fila_prioridade.at(b).empty())
                {
                    printf("ERRO: FILA PRIORIDADE ESTÁ VAZIA\n");
                };
                auto it_a = fila_prioridade[a].begin();
                auto it_b = fila_prioridade[b].begin();

                int valor_a = it_a->second; // equivalente a next(iter(dict.values()))
                int valor_b = it_b->second;

                // critério 1: menor custo
                if (valor_a != valor_b)
                    return valor_a < valor_b;

                // critério 2: mais perto de 0
                int crit_a = std::abs(necessidades_rotas[a] + it_a->first); // equivalente a next(iter(dict.keys()))
                int crit_b = std::abs(necessidades_rotas[b] + it_b->first);
                return crit_a < crit_b;
            });

        int melhor_rota = (melhor_it != rotas_a_atualizar.end()) ? *melhor_it : rotas_a_atualizar.back();

        // Seleciona a estação de menor custo necessario achar o indice
        printf("checkpoint 1\n");
        auto it = min_element( // Retorna o par chave : valor
            fila_prioridade[melhor_rota].begin(),
            fila_prioridade[melhor_rota].end(),
            [](const auto &a, const auto &b)
            { return a.second < b.second; });
        printf("checkpoint 1.2\n");

        if (it == fila_prioridade[melhor_rota].end())
        {
            // mapa vazio — tratar: p.ex. break/continue/ignore this route
            continue;
        }
        int estacao_escolhida = it->first;
        int custo_escolhido = it->second;

        rotas[melhor_rota].push_back(estacao_escolhida);
        necessidades_rotas[melhor_rota] += p.demandas[estacao_escolhida];

        custo_total += custo_escolhido;

        // Removendo por valor
        printf("checkpoint 2\n");
        restam_visitar.erase(
            remove(restam_visitar.begin(),
                   restam_visitar.end(),
                   estacao_escolhida),
            restam_visitar.end());
        printf("checkpoint 2.1\n");

        // Para a rota que acabou de inserir, é necessário reajustar sua fila de prioridades, considerando a nova estação final
        printf("\tValor de melhor rota: %d\n\tValor de estacao_escolhida: %d\n", melhor_rota, estacao_escolhida);
        for (int i : restam_visitar)
            printf("%d\t", i);
        cout << "\nTamanho de restam_visitar: " << restam_visitar.size() << "\n";
        // Restam visitar:
        map<int, int> ajuste;
        if (restam_visitar.size())
        {
            for (int estacao : restam_visitar)
            {
                if (std::abs(necessidades_rotas[melhor_rota] + p.demandas[estacao]) <= p.capacidade_max)
                    ajuste[estacao] = p.matriz_custo[estacao_escolhida][estacao];
            }
            fila_prioridade[melhor_rota] = ajuste;
        }

        printf("checkpoint 3\n");

        // Remove a estação escolhida de todas as filas
        for (int i : rotas_a_atualizar)
            fila_prioridade[i].erase(estacao_escolhida);

        printf("checkpoint 3.1\n");
        rotas_a_atualizar.erase(
            std::remove_if(
                rotas_a_atualizar.begin(),
                rotas_a_atualizar.end(),
                [&](int i)
                {
                    return fila_prioridade[i].empty();
                    // remove se o mapa dessa rota estiver vazio
                }),
            rotas_a_atualizar.end());
    }

    // Finalmente, cada rota deve retornar ao galpão
    printf("CHECKPOINT 4\n");
    for (auto &rota : rotas)
    {
        if (rota.size() != 0)
        {
            int ultima_estacao = rota.back();
            custo_total += p.matriz_custo[ultima_estacao][0]; // Custo da ultima estação para o deposito
            rota.push_back(0);
        }
    }
    printf("CHECKPOINT 4.1\n");

    // Construção do Objeto Solução e do array de rotas
    Solucao solucao_encontrada;
    solucao_encontrada.custo_total = custo_total;
    solucao_encontrada.veiculos_usados = p.qnt_veiculos;
    solucao_encontrada.veiculos_disponiveis = 0;

    vector<int> rota_vazia = {0, 0};
    vector<Rota> rotas_solucao;
    for (vector<int> rota : rotas)
    {

        for (int estacao : rota)
        {
            printf("%d\t", estacao);
        }

        if (rota == rota_vazia)
        {
            solucao_encontrada.veiculos_usados--;
            solucao_encontrada.veiculos_disponiveis++;
            Rota route = Rota();
            rotas_solucao.push_back(route);
            continue;
        }

        // Construção do objeto Rota
        Rota route = Rota();

        printf("checkpoint 5\n");
        for (int index = 1; index < rota.size() - 1; index++)
        {
            printf("Inserindo a estacao %d na rota\n", rota[index]);
            route.InsertEnd(rota[index]);
        }
        printf("checkpoint 5.1\n");

        rotas_solucao.push_back(route);
        printf("checkpoint 5.2\n");
    }

    solucao_encontrada.rotas = rotas_solucao;
    printf("FIM VIZINHO MAIS PROXIMO\n");
    setSolucao(solucao_encontrada);
    return solucao_encontrada;
}

vector<Rota> MelhorarSolucao(vector<Rota> rotas)
{
    bool melhorou = true;
    int index_rota_origem;
    while (melhorou)
    {
        melhorou = false;
        index_rota_origem = 0;
        for (Rota rota : melhor_solucao.rotas)
        {
            // Itera por cada estação em rota

            No *no_atual = rota.rota_i->proximo;
            int index_rota_destino = 0;
            while (no_atual->estacao)
            {
                int index_rota_destino = 0;
                for (Rota rota_destino : rotas)
                {
                    for (int pos = 1; pos < rota_destino.rotaTam; pos++)
                    {
                        if (no_atual->estacao == 0) // Nunca mover depositos
                            continue;

                        // Cópia para teste
                        Rota nova_rota_origem = rota;
                        Rota nova_rota_destino;

                        // Remoção da estação atual pra nova_rota_origem
                        // Achando a posição do alvo

                        No *aux = nova_rota_origem.rota_i;
                        int posicao = 0;
                        while (aux->estacao != no_atual->estacao)
                        {
                            posicao++;
                            aux = aux->proximo;
                        }
                        nova_rota_origem.RemoveAt(posicao);

                        index_rota_origem == index_rota_destino
                            ? nova_rota_destino = nova_rota_origem
                            : nova_rota_destino = rota_destino;

                        // Inserção da estação na posição válida
                        nova_rota_destino.InsertAt(posicao, no_atual->estacao);

                        // Checa viabilidade
                        if (!ValidaDemanda(nova_rota_destino) || !ValidaDemanda(nova_rota_origem))
                            continue;

                        // Calculo de custos
                        int custo_atual;
                        int custo_novo;

                        if (index_rota_destino == index_rota_origem)
                        {
                            rota.direcao_atual >= INICIO_FIM ? custo_atual = rota.custo_total_d1 : custo_atual = rota.custo_total_d2;
                            custo_novo = SomaCustoRota(nova_rota_destino);
                        }
                        else
                        {
                            custo_atual = SomaCusto({rota, rota_destino});
                            custo_novo = SomaCusto({nova_rota_origem, nova_rota_destino});
                        }

                        // Aplica a mudança se melhorou
                        if (custo_novo < custo_atual)
                        {
                            rotas[index_rota_destino] = nova_rota_destino;
                            if (index_rota_destino != index_rota_origem)
                                rotas[index_rota_origem] = nova_rota_origem;
                            melhorou = true;
                        }
                    }
                    index_rota_destino++;
                }

                no_atual = no_atual->proximo;
            }

            index_rota_origem++;
        }
    }

    return rotas;
}

Solucao InsercaoMaisBarata()
{
    Solucao s;
    vector<Rota> rotas(p.qnt_veiculos, Rota());

    // rota inicial:
    // 0 -> triang_inicial.first -> triang_inicial.second -> 0
    pair<int, int> triang_inicial = MelhoresVertices();
    rotas[0].InsertEnd(triang_inicial.first);
    rotas[0].InsertEnd(triang_inicial.second);
    int caminhoes_restantes = p.qnt_veiculos - 1;

    // Faz uma lista de estações que não estão
    // no triângulo inicial da primeira rota
    vector<int> estacoes_restantes(p.qnt_estacoes - 2);
    for (int i = 0; i < p.qnt_estacoes; i++)
    {
        if (i == triang_inicial.first || i == triang_inicial.second)
            continue;
        estacoes_restantes.push_back(i);
    }

    // Verifica onde é melhor inserir essa estação.
    // Se a relação do novo custo for negativa (diminui o custo total da rota) ou 0, insere nesta mesma rota;
    // Se for positiva, tenta inserir em outra rota (se possível)

    typedef struct
    {
        int i_rota;
        Rota rota;
        int indice;
        DirecaoRota direcao;
    } Configuracao;

    // para todas as estacoes restantes (que não foram inseridas no triangulo inical)
    for (int estacao : estacoes_restantes)
    {
        int menor_custo = __INT_MAX__;
        Configuracao melhor_config = {
            -1,
            Rota(),
            -1,
            DirecaoRota::INICIO_FIM};

        // Ultima rota verificada
        Configuracao ultima_config = {
            (int)rotas.size() - 1,
            Rota(),
            -1,
            DirecaoRota::INICIO_FIM};
        // para cada rota
        for (int iRota = 0; iRota < rotas.size(); iRota++)
        {
            if (rotas[iRota].rota_i == nullptr)
            {
                ultima_config.i_rota = iRota;
                ultima_config.direcao = DirecaoRota::INICIO_FIM;
                break;
            }
            // PARA A ROTA NO SENTIDO INICIO-FIM
            Rota rota = rotas[iRota];

            No *aux = rotas[iRota].rota_i;
            int i = 0;
            while (aux && aux->estacao)
            {

                // calcula o custo de insercao neste nó;
                int custo_insercao = p.matriz_custo[aux->estacao][estacao] + p.matriz_custo[estacao][aux->proximo->estacao];
                int custo = custo_insercao - p.matriz_custo[aux->estacao][aux->proximo->estacao];

                if (custo < menor_custo)
                {
                    if (VerificaNovaDemanda(rotas[iRota], i, estacao))
                    {
                        menor_custo = custo;
                        melhor_config = {
                            iRota,
                            rotas[iRota],
                            i,
                            DirecaoRota::INICIO_FIM};
                    }
                }

                aux = aux->proximo;
                i++;
            }

            // percorrer ao contrario tambem
            aux = rotas[iRota].rota_f;
            i = 0;
            while (aux && aux->estacao)
            {

                // calcula o custo de insercao neste nó;
                int custo_insercao = p.matriz_custo[aux->estacao][estacao] + p.matriz_custo[estacao][aux->anterior->estacao];
                int custo = custo_insercao - p.matriz_custo[aux->estacao][aux->anterior->estacao];

                if (custo < menor_custo)
                {
                    if (VerificaNovaDemanda(rotas[iRota], i, estacao))
                    {
                        menor_custo = custo;
                        melhor_config = {
                            iRota,
                            rotas[iRota],
                            i,
                            DirecaoRota::FIM_INICIO};
                    }
                }

                aux = aux->anterior;
                i++;
            }
        }
        if (
            menor_custo <= (p.matriz_custo[0][estacao] + p.matriz_custo[estacao][0]) ||
            caminhoes_restantes == 0)
        {
            // inserção da melhor configuração na rota definida
            rotas[melhor_config.i_rota].direcao_atual = melhor_config.direcao;
            rotas[melhor_config.i_rota].InsertAt(melhor_config.indice, estacao);
        }
        else
        {
            // inserção no inicio, pois é uma nova rota
            rotas[ultima_config.i_rota].InsertBegin(estacao);
        }
    }

    for (Rota rota : rotas)
    {
        if (rota.direcao_atual == DirecaoRota::INICIO_FIM)
            s.custo_total += rota.custo_total_d1;
        else
            s.custo_total += rota.custo_total_d2;
    }

    s.rotas = rotas;
    s.veiculos_disponiveis = caminhoes_restantes;
    s.veiculos_usados = p.qnt_veiculos - caminhoes_restantes;

    return s;
}

bool VerificaDemandaSwap(const Rota &rota, const No *aux1, const No *aux2)
{
    int demanda_atual = 0;
    if (rota.direcao_atual == DirecaoRota::INICIO_FIM)
    {
        No *aux = rota.rota_i->proximo; // pula a garagem

        // considerando que o que está antes dos nós do Swap estão com a demanda
        while (aux != aux1)
        {
            demanda_atual += p.demandas[aux->estacao - 1];
            aux = aux->proximo;
        }

        // conta a demanda do aux2 no lugar do aux1
        demanda_atual += p.demandas[aux2->estacao - 1];
        if (abs(demanda_atual) > p.capacidade_max)
        {
            return false;
        }
        // pula o aux1
        aux = aux->proximo;

        while (aux != aux2)
        {
            demanda_atual += p.demandas[aux->estacao - 1];
            if (abs(demanda_atual) > p.capacidade_max)
            {
                return false;
            }

            aux = aux->proximo;
        }

        // ao encontrar aux2, calcula a demanda de aux1 no lugar
        demanda_atual += p.demandas[aux1->estacao - 1];
        if (abs(demanda_atual) > p.capacidade_max)
        {
            return false;
        }
        // pula aux2
        aux = aux->proximo;

        // calcula o restante da demanda
        while (aux && aux->estacao)
        {
            demanda_atual += p.demandas[aux->estacao - 1];
            if (abs(demanda_atual) > p.capacidade_max)
            {
                return false;
            }

            aux = aux->proximo;
        }

        // verifica na direcao opostam
    }
    else
    {
        No *aux = rota.rota_f->anterior; // pula a garagem

        // considerando que o que está antes dos nós do Swap estão com a demanda
        while (aux != aux1)
        {
            demanda_atual += p.demandas[aux->estacao - 1];
            aux = aux->anterior;
        }

        // conta a demanda do aux2 no lugar do aux1
        demanda_atual += p.demandas[aux2->estacao - 1];
        if (abs(demanda_atual) > p.capacidade_max)
        {
            return false;
        }
        // pula o aux1
        aux = aux->anterior;

        while (aux != aux2)
        {
            demanda_atual += p.demandas[aux->estacao - 1];
            if (abs(demanda_atual) > p.capacidade_max)
            {
                return false;
            }

            aux = aux->anterior;
        }

        // ao encontrar aux2, calcula a demanda de aux1 no lugar
        demanda_atual += p.demandas[aux1->estacao - 1];
        if (abs(demanda_atual) > p.capacidade_max)
        {
            return false;
        }
        // pula aux2
        aux = aux->anterior;

        // calcula o restante da demanda
        while (aux && aux->estacao)
        {
            demanda_atual += p.demandas[aux->estacao - 1];
            if (abs(demanda_atual) > p.capacidade_max)
            {
                return false;
            }

            aux = aux->anterior;
        }
    }

    return true;
}

/*
    Aplicar testes de swap na rota e retornar a melhor rota encontrada
    ou a mesma que recebeu, caso não haja melhora
 */
Rota VND_Swap(const Rota &r)
{
    Rota r_ret = Rota();
    int melhor_custo = __INT_MAX__;
    int custo_anterior, custo_swap, novo_custo;

    No *melhor_aux1 = nullptr, *melhor_aux2 = nullptr;

    // cria uma cópia da rota recebida, a qual será modificada e retornada,
    // de acordo com o algoritmo
    {
        No *aux = r.rota_i->proximo;
        while (aux && aux->estacao)
        {
            r_ret.InsertEnd(aux->estacao);
            aux = aux->proximo;
        }
        r_ret.direcao_atual = r.direcao_atual;
        r_ret.custo_total_d1 = r.custo_total_d1;
        r_ret.custo_total_d2 = r.custo_total_d2;
        r_ret.rotaTam = r.rotaTam;
    }

    if (r.direcao_atual == DirecaoRota::INICIO_FIM)
    {
        // começa do primeiro elemento, ignorando a garagem
        No *aux1 = r_ret.rota_i->proximo;

        if (!aux1)
            return r_ret;

        // termina quando chegar na garagem
        while (aux1 && aux1->estacao)
        {

            // mesma lógica que a anterior, porém começando do próximo de aux1
            No *aux2 = aux1->proximo;
            if (!aux2)
            {
                // só há um elemento
                if (aux2 == aux2->proximo && aux1 == r_ret.rota_i->proximo)
                    return r_ret;
                break;
            }

            while (aux2 && aux2->estacao)
            {

                custo_anterior = p.matriz_custo[aux1->anterior->estacao][aux1->estacao] +
                                 p.matriz_custo[aux1->estacao][aux1->proximo->estacao] +
                                 p.matriz_custo[aux2->anterior->estacao][aux2->estacao] +
                                 p.matriz_custo[aux2->estacao][aux2->proximo->estacao];

                custo_swap = p.matriz_custo[aux1->anterior->estacao][aux2->estacao] +
                             p.matriz_custo[aux2->estacao][aux1->proximo->estacao] +
                             p.matriz_custo[aux2->anterior->estacao][aux1->estacao] +
                             p.matriz_custo[aux1->estacao][aux2->proximo->estacao];

                novo_custo = custo_swap - custo_anterior;

                if (novo_custo < melhor_custo && VerificaDemandaSwap(r_ret, aux1, aux2))
                {
                    // atualiza melhor custo
                    melhor_custo = novo_custo;

                    // guarda as estações
                    melhor_aux1 = aux1;
                    melhor_aux2 = aux2;
                }
                aux2 = aux2->proximo;
            }
            aux1 = aux1->proximo;
        }

        if (melhor_custo < 0)
        {
            int temp = melhor_aux1->estacao;
            melhor_aux1->estacao = melhor_aux2->estacao;
            melhor_aux2->estacao = temp;

            r_ret.custo_total_d1 += melhor_custo;
            // atualizar o custo total d2
        }
    }
    else
    {

        // começa do primeiro elemento, ignorando a garagem
        No *aux1 = r_ret.rota_i->anterior;

        if (!aux1)
            return r_ret;

        // termina quando chegar na garagem
        while (aux1 && aux1->estacao)
        {

            // mesma lógica que a anterior, porém começando do próximo de aux1
            No *aux2 = aux1->anterior;
            if (!aux2)
            {
                // só há um elemento
                if (aux2 == aux1->anterior && aux1 == r_ret.rota_f->anterior)
                    return r_ret;
                break;
            }

            while (aux2 && aux2->estacao)
            {

                custo_anterior = p.matriz_custo[aux1->proximo->estacao][aux1->estacao] +
                                 p.matriz_custo[aux1->estacao][aux1->anterior->estacao];
                custo_anterior += p.matriz_custo[aux2->proximo->estacao][aux2->estacao] +
                                  p.matriz_custo[aux2->estacao][aux2->anterior->estacao];

                custo_swap = p.matriz_custo[aux1->proximo->estacao][aux2->estacao] +
                             p.matriz_custo[aux2->estacao][aux1->anterior->estacao];
                custo_swap += p.matriz_custo[aux2->proximo->estacao][aux1->estacao] +
                              p.matriz_custo[aux1->estacao][aux2->anterior->estacao];

                novo_custo = custo_swap - custo_anterior;

                if (novo_custo < melhor_custo && VerificaDemandaSwap(r_ret, aux1, aux2))
                {
                    // atualiza melhor custo
                    melhor_custo = novo_custo;

                    // guarda as estações
                    melhor_aux1 = aux1;
                    melhor_aux2 = aux2;
                }
                aux2 = aux2->anterior;
            }
            aux1 = aux1->anterior;
        }

        if (melhor_custo < 0)
        {
            int temp = melhor_aux1->estacao;
            melhor_aux1->estacao = melhor_aux2->estacao;
            melhor_aux2->estacao = temp;

            r_ret.custo_total_d2 += melhor_custo;
        }
    }

    return r_ret;
}

bool VerificaDemandaReInsertion(const Rota &rota, const No *aux1, const No *aux2)
{
    int demanda_atual = 0;
    if (rota.direcao_atual == DirecaoRota::INICIO_FIM)
    {
        No *aux = rota.rota_i->proximo; // pula a garagem

        // considerando que o que está antes dos nós do Swap estão com a demanda
        while (aux != aux1)
        {
            demanda_atual += p.demandas[aux->estacao - 1];
            aux = aux->proximo;
        }

        // pula o aux1
        aux = aux->proximo;

        while (aux != aux2)
        {
            demanda_atual += p.demandas[aux->estacao - 1];
            if (abs(demanda_atual) > p.capacidade_max)
            {
                return false;
            }

            aux = aux->proximo;
        }

        // ao encontrar aux2, calcula a demanda de aux1 em seguida, depois continua
        demanda_atual += p.demandas[aux2->estacao - 1];
        if (abs(demanda_atual) > p.capacidade_max)
        {
            return false;
        }
        demanda_atual += p.demandas[aux1->estacao - 1];
        if (abs(demanda_atual) > p.capacidade_max)
        {
            return false;
        }
        // os restantes
        aux = aux->proximo;

        // calcula o restante da demanda
        while (aux && aux->estacao)
        {
            demanda_atual += p.demandas[aux->estacao - 1];
            if (abs(demanda_atual) > p.capacidade_max)
            {
                return false;
            }

            aux = aux->proximo;
        }

        // verifica na direcao oposta
    }
    else
    {
        No *aux = rota.rota_f->anterior; // pula a garagem

        // considerando que o que está antes dos nós do Swap estão com a demanda
        while (aux != aux1)
        {
            demanda_atual += p.demandas[aux->estacao - 1];
            aux = aux->anterior;
        }

        // conta a demanda do aux2 no lugar do aux1
        demanda_atual += p.demandas[aux2->estacao - 1];
        if (abs(demanda_atual) > p.capacidade_max)
        {
            return false;
        }
        // pula o aux1
        aux = aux->anterior;

        while (aux != aux2)
        {
            demanda_atual += p.demandas[aux->estacao - 1];
            if (abs(demanda_atual) > p.capacidade_max)
            {
                return false;
            }

            aux = aux->anterior;
        }

        // ao encontrar aux2, calcula a demanda de aux1 em seguida, depois continua
        demanda_atual += p.demandas[aux2->estacao - 1];
        if (abs(demanda_atual) > p.capacidade_max)
        {
            return false;
        }
        demanda_atual += p.demandas[aux1->estacao - 1];
        if (abs(demanda_atual) > p.capacidade_max)
        {
            return false;
        }

        // os restantes
        aux = aux->anterior;

        // calcula o restante da demanda
        while (aux && aux->estacao)
        {
            demanda_atual += p.demandas[aux->estacao - 1];
            if (abs(demanda_atual) > p.capacidade_max)
            {
                return false;
            }

            aux = aux->anterior;
        }
    }

    return true;
}

/*
    Aplicar testes de re-insertion na rota e retornar a melhor rota encontrada
    ou a mesma que recebeu, caso não haja melhora
 */
Rota VND_Re_Insertion(const Rota &r)
{
    Rota r_ret = Rota();
    int melhor_custo = __INT_MAX__;
    int custo_anterior, custo_reinsertion, novo_custo;
    No *melhor_aux1 = nullptr, *melhor_aux2 = nullptr;

    // cópia da rota
    {
        No *aux = r.rota_i->proximo;
        while (aux && aux->estacao)
        {
            r_ret.InsertEnd(aux->estacao);
            aux = aux->proximo;
        }
        r_ret.direcao_atual = r.direcao_atual;
        r_ret.custo_total_d1 = r.custo_total_d1;
        r_ret.custo_total_d2 = r.custo_total_d2;
        r_ret.rotaTam = r.rotaTam;
    }

    if (r_ret.direcao_atual == DirecaoRota::INICIO_FIM)
    {
        No *aux1 = r_ret.rota_i->proximo;
        if (!aux1)
            return r_ret;

        while (aux1 && aux1->estacao)
        {
            No *aux2 = aux1->proximo;

            if (!aux2)
            {
                // se for um único elemento na rota
                if (aux2 == aux1->proximo && aux1 == r_ret.rota_i->proximo)
                    return r_ret;
                break;
            }
            while (aux2 && aux2->estacao)
            {

                /*
                    Lógica de Re-Insertion
                    - custo anterior: custo do nó em sua posicao inicial + custo do lugar onde vai ser inserido
                    - novo custo: custo do lugar de onde o nó foi retirado + de onde foi inserido
                */

                custo_anterior = p.matriz_custo[aux1->anterior->estacao][aux1->estacao] +
                                 p.matriz_custo[aux1->estacao][aux1->proximo->estacao] +
                                 p.matriz_custo[aux2->estacao][aux2->proximo->estacao];

                custo_reinsertion = p.matriz_custo[aux1->anterior->estacao][aux1->proximo->estacao] +
                                    p.matriz_custo[aux2->estacao][aux1->estacao] +
                                    p.matriz_custo[aux1->estacao][aux2->proximo->estacao];

                novo_custo = custo_reinsertion - custo_anterior;

                if (novo_custo < melhor_custo && VerificaDemandaReInsertion(r_ret, aux1, aux2))
                {

                    melhor_custo = novo_custo;
                    melhor_aux1 = aux1;
                    melhor_aux2 = aux2;
                }

                aux2 = aux2->proximo;
            }
            aux1 = aux1->proximo;
        }

        // reinserção:
        if (melhor_custo < 0)
        {
            // remover o aux1
            melhor_aux1->anterior->proximo = melhor_aux1->proximo;
            melhor_aux1->proximo->anterior = melhor_aux1->anterior;

            // inserir aux1 depois de aux2
            melhor_aux1->proximo = melhor_aux2->proximo;
            melhor_aux1->anterior = melhor_aux2;
            melhor_aux2->proximo->anterior = melhor_aux1;
            melhor_aux2->proximo = melhor_aux1;

            r_ret.custo_total_d1 += melhor_custo;

            // recalcular custo total d2
        }
    }
    else
    {

        No *aux1 = r_ret.rota_i->anterior;
        if (!aux1)
            return r_ret;

        while (aux1 && aux1->estacao)
        {
            No *aux2 = aux1->anterior;

            if (!aux2)
            {
                // se for um único elemento na rota
                if (aux2 == aux1->anterior && aux1 == r_ret.rota_i->anterior)
                    return r_ret;
                break;
            }
            while (aux2 && aux2->estacao)
            {

                /*
                    Lógica de Re-Insertion
                    - custo anterior: custo do nó em sua posicao inicial + custo do lugar onde vai ser inserido
                    - novo custo: custo do lugar de onde o nó foi retirado + de onde foi inserido
                */

                custo_anterior = p.matriz_custo[aux1->proximo->estacao][aux1->estacao] +
                                 p.matriz_custo[aux1->estacao][aux1->anterior->estacao] +
                                 p.matriz_custo[aux2->estacao][aux2->anterior->estacao];

                custo_reinsertion = p.matriz_custo[aux1->proximo->estacao][aux1->anterior->estacao] +
                                    p.matriz_custo[aux2->estacao][aux1->estacao] +
                                    p.matriz_custo[aux1->estacao][aux2->anterior->estacao];

                novo_custo = custo_reinsertion - custo_anterior;

                if (novo_custo < melhor_custo && VerificaDemandaReInsertion(r_ret, aux1, aux2))
                {
                    melhor_custo = novo_custo;
                    melhor_aux1 = aux1;
                    melhor_aux2 = aux2;
                }

                aux2 = aux2->anterior;
            }
            aux1 = aux1->anterior;
        }

        // reinserção
        if (melhor_custo < 0)
        {
            // remover o aux1
            melhor_aux1->proximo->anterior = melhor_aux1->anterior;
            melhor_aux1->anterior->proximo = melhor_aux1->proximo;

            // inserir aux1 depois de aux2
            melhor_aux1->anterior = melhor_aux2->anterior;
            melhor_aux1->proximo = melhor_aux2;
            melhor_aux2->anterior->proximo = melhor_aux1;
            melhor_aux2->anterior = melhor_aux1;

            r_ret.custo_total_d2 += melhor_custo;
        }
    }

    return r_ret;
}

bool VerificaDemandaInversion(const Rota &r, No *auxA, No *auxB)
{
    int demanda = 0;
    if (r.direcao_atual == DirecaoRota::INICIO_FIM)
    {

        No *aux = r.rota_i->proximo;
        while (aux && aux != auxA)
        {
            demanda += p.demandas[aux->estacao - 1];
            // desconsidera verificar a demanda nessa parte inalterada
            aux = aux->proximo;
        }
        aux = auxB;
        while (aux && aux != auxA->anterior)
        {
            demanda += p.demandas[aux->estacao - 1];
            if (abs(demanda) > p.capacidade_max)
                return false;

            aux = aux->anterior;
        }
        aux = auxB->proximo;
        while (aux && aux->estacao)
        {
            demanda += p.demandas[aux->estacao - 1];
            if (abs(demanda) > p.capacidade_max)
                return false;
            aux = aux->proximo;
        }
    }
    else
    {

        No *aux = r.rota_f->anterior;
        while (aux && aux != auxA)
        {
            demanda += p.demandas[aux->estacao - 1];
            // desconsidera verificar a demanda nessa parte inalterada
            aux = aux->anterior;
        }
        aux = auxB;
        while (aux && aux != auxA->proximo)
        {
            demanda += p.demandas[aux->estacao - 1];
            if (abs(demanda) > p.capacidade_max)
                return false;

            aux = aux->proximo;
        }
        aux = auxB->anterior;
        while (aux && aux->estacao)
        {
            demanda += p.demandas[aux->estacao - 1];
            if (abs(demanda) > p.capacidade_max)
                return false;
            aux = aux->anterior;
        }
    }

    return true;
}

/*
    Inverte uma sub-rota na tentativa de "desatar nós"
 */
Rota VND_Inversion(const Rota &r)
{
    Rota r_ret = Rota();
    int melhor_custo = __INT_MAX__;
    int custo_anterior, custo_inversion, novo_custo;
    No *melhor_auxA = nullptr, *melhor_auxB = nullptr;

    // cópia da rota
    {
        No *aux = r.rota_i->proximo;
        while (aux && aux->estacao)
        {
            r_ret.InsertEnd(aux->estacao);
            aux = aux->proximo;
        }
        r_ret.direcao_atual = r.direcao_atual;
        r_ret.custo_total_d1 = r.custo_total_d1;
        r_ret.custo_total_d2 = r.custo_total_d2;
        r_ret.rotaTam = r.rotaTam;
    }

    if (r.direcao_atual == DirecaoRota::INICIO_FIM)
    {
        No *auxA = r_ret.rota_i->proximo;
        if (!auxA)
            return r_ret;

        while (auxA && auxA->estacao)
        {
            No *auxB = auxA->proximo;
            if (!auxB)
            {
                if (auxB == auxA->proximo && auxA == r_ret.rota_i->proximo)
                    return r_ret;
                break;
            }

            while (auxB && auxB->estacao)
            {

                custo_anterior = p.matriz_custo[auxA->anterior->estacao][auxA->estacao] +
                                 p.matriz_custo[auxB->estacao][auxB->proximo->estacao];

                custo_inversion = p.matriz_custo[auxA->anterior->estacao][auxB->estacao] +
                                  p.matriz_custo[auxA->estacao][auxB->proximo->estacao];

                // custo no miolo invertido
                No *aux1 = auxA;
                No *aux2 = auxB;
                while (aux1 != auxB)
                {
                    custo_anterior += p.matriz_custo[aux1->estacao][aux1->proximo->estacao];
                    custo_inversion += p.matriz_custo[aux2->estacao][aux2->anterior->estacao];
                    aux1 = aux1->proximo;
                    aux2 = aux2->anterior;
                }

                novo_custo = custo_inversion - custo_anterior;

                if (novo_custo < melhor_custo && VerificaDemandaInversion(r_ret, auxA, auxB))
                {
                    melhor_custo = novo_custo;

                    melhor_auxA = auxA;
                    melhor_auxB = auxB;
                }

                auxB = auxB->proximo;
            }
            auxA = auxA->proximo;
        }

        if (melhor_custo < 0)
        {
            No *anterior = melhor_auxA->anterior;
            No *proximo = melhor_auxB->proximo;

            No *atual = melhor_auxA;
            No *temp = nullptr;
            while (atual != proximo)
            {
                temp = atual->proximo;
                atual->proximo = atual->anterior;
                atual->anterior = temp;
                atual = temp;
            }

            if (anterior)
                anterior->proximo = melhor_auxB;
            melhor_auxB->anterior = anterior;

            if (proximo)
                proximo->anterior = melhor_auxA;
            melhor_auxA->proximo = proximo;

            r_ret.custo_total_d1 += melhor_custo;

            // calcular custo d2
            r_ret.custo_total_d2 = 0;
            atual = r_ret.rota_f;
            while (atual)
            {
                r_ret.custo_total_d2 += p.matriz_custo[atual->anterior->estacao][atual->estacao];
                atual = atual->anterior;
            }
        }
    }
    else
    {
        No *auxA = r_ret.rota_f->anterior;
        if (!auxA)
            return r_ret;

        while (auxA && auxA->estacao)
        {
            No *auxB = auxA->anterior;
            if (!auxB)
            {
                if (auxB == auxA->anterior && auxA == r_ret.rota_f->anterior)
                    return r_ret;
                break;
            }

            while (auxB && auxB->estacao)
            {

                custo_anterior = p.matriz_custo[auxA->proximo->estacao][auxA->estacao] +
                                 p.matriz_custo[auxB->estacao][auxB->anterior->estacao];

                custo_inversion = p.matriz_custo[auxA->proximo->estacao][auxB->estacao] +
                                  p.matriz_custo[auxA->estacao][auxB->anterior->estacao];

                // custo no miolo invertido
                No *aux1 = auxA;
                No *aux2 = auxB;
                while (aux1 != auxB)
                {
                    custo_anterior += p.matriz_custo[aux1->estacao][aux1->anterior->estacao];
                    custo_inversion += p.matriz_custo[aux2->estacao][aux2->proximo->estacao];
                    aux1 = aux1->anterior;
                    aux2 = aux2->proximo;
                }

                novo_custo = custo_inversion - custo_anterior;

                if (novo_custo < melhor_custo && VerificaDemandaInversion(r_ret, auxA, auxB))
                {
                    melhor_custo = novo_custo;

                    melhor_auxA = auxA;
                    melhor_auxB = auxB;
                }

                auxB = auxB->anterior;
            }
            auxA = auxA->anterior;
        }

        if (melhor_custo < 0)
        {
            No *proximo = melhor_auxA->proximo;
            No *anterior = melhor_auxB->anterior;

            No *atual = melhor_auxA;
            No *temp = nullptr;
            while (atual != anterior)
            {
                temp = atual->anterior;
                atual->anterior = atual->proximo;
                atual->proximo = temp;
                atual = temp;
            }

            if (proximo)
                proximo->anterior = melhor_auxB;
            melhor_auxB->proximo = proximo;

            if (anterior)
                anterior->proximo = melhor_auxA;
            melhor_auxA->anterior = anterior;

            r_ret.custo_total_d2 += melhor_custo;

            // calcular o custo de d1
            r_ret.custo_total_d1 = 0;
            atual = r_ret.rota_i;
            while (atual)
            {
                r_ret.custo_total_d1 += p.matriz_custo[atual->estacao][atual->proximo->estacao];
                atual = atual->proximo;
            }
        }
    }

    return r_ret;
}

Solucao VND(Solucao solucao)
{
    int n_rotas = solucao.rotas.size();
    VND_attr attr[n_rotas];

    // inicializa os atributos de cada rota
    for (int i = 0; i < n_rotas; i++)
    {
        attr[i].solucao = solucao;
        attr[i].i_rota = i;
        attr[i].rota_retorno = solucao.rotas[i];
    }

    // Para cada rota, cria uma thread de execução do VND
    for (int i = 0; i < n_rotas;)
    {
        // uma thread para cada attr[i]
    }

    // espera as rotas de cada thread
}

void *VND(void *atributos)
{
    /**
     * Movimentos de vizinhança escolhidos (constante NUM_VND_STRUCTURES define quantas são)
     * 1. Swap (Troca de dois elementos)
     * 2. Re-insertion (Mover um elemento para outra posição)
     * 3. Inversion (Pegar uma fatia da solução e inverter)
     */

    VND_attr *attr = (VND_attr *)atributos;

    // criando um vetor de estruturas:
    vector<function<Rota(Rota)>> estruturas_VND = {
        VND_Swap,
        VND_Re_Insertion,
        VND_Inversion};

    // inicia percorrendo as estruturas da primeira (k = 0) até a ultima, while k <= NUM_VND_STRUCTURES
    int k = 0;

    // dependendo da direção, a condicional dentro do loop é diferente
    // para fins de otimização, faz-se a verificação uma única vez, fora do loop
    if (attr->solucao.rotas[attr->i_rota].direcao_atual == DirecaoRota::INICIO_FIM)
    {
        // usa o custo_total na direcao d1
        while (k < NUM_VND_STRUCTURES)
        {
            // chama uma estrutura de vizinhança k e retorna o valor para nova_rota
            Rota nova_rota = estruturas_VND[k](attr->rota_retorno);

            if (nova_rota.custo_total_d1 < attr->rota_retorno.custo_total_d1)
            {
                k = 0;
                // essa atribuição ainda tem problemas. criar operador move na classe rota
                attr->rota_retorno = nova_rota; // move evita fazer a cópia da rota
            }
            else
                k++;
        }
    }
    else
    {
        // usa o custo_total na direcao d2
        while (k < NUM_VND_STRUCTURES)
        {
            // chama uma estrutura de vizinhança k e retorna o valor para nova_rota
            Rota nova_rota = estruturas_VND[k](attr->rota_retorno);

            if (nova_rota.custo_total_d2 < attr->rota_retorno.custo_total_d2)
            {
                k = 0;
                attr->rota_retorno = nova_rota;
            }
            else
                k++;
        }
    }

    return nullptr;
}

vector<Rota> ILS_switch(vector<Rota> rotas, int trocas_a_realizar)
{
    int qtd_rotas = rotas.size();
    if (!qtd_rotas)
        return rotas;

    while (trocas_a_realizar > 0)
    {
        int rota_origem_index, rota_destino_index;

        if (qtd_rotas > 1)
        {
            rota_origem_index = rand() % qtd_rotas;
            rota_destino_index = rand() % qtd_rotas;
            while (rota_origem_index == rota_destino_index)
                rota_destino_index = rand() % qtd_rotas;
        }
        else
        {
            rota_origem_index = 0;
            rota_destino_index = 0;
        }

        if (rotas[rota_origem_index].rotaTam < 3 || rotas[rota_destino_index].rotaTam < 3)
        {
            trocas_a_realizar--;
            continue;
        }

        int posicao_1 = (rand() % (rotas[rota_origem_index].rotaTam - 2)) + 1;
        int posicao_2 = (rand() % (rotas[rota_destino_index].rotaTam - 2)) + 1;

        rotas[rota_origem_index].RemoveAt(posicao_1);
        rotas[rota_origem_index].InsertAt(posicao_1, rotas[rota_destino_index].stationAtIndex(posicao_2));

        rotas[rota_destino_index].RemoveAt(posicao_2);
        rotas[rota_destino_index].InsertAt(posicao_2, rotas[rota_origem_index].stationAtIndex(posicao_1));

        trocas_a_realizar--;
    }
    return rotas;
}

vector<Rota> ILS_relocate(vector<Rota> rotas, int trocas_a_realizar, int qtd_elementos_trocados, bool reverso = false)
{
    while ((trocas_a_realizar > 0) && (rotas.size()))
    {
        int rota_origem_index = rand() % (rotas.size() - 1);
        int rota_destino_index = rand() % (rotas.size() - 1);

        if ((rotas[rota_origem_index].rotaTam) <= (qtd_elementos_trocados + 1))
        {
            trocas_a_realizar--;
            continue;
        }

        int max_index = rotas[rota_origem_index].rotaTam - 1 - qtd_elementos_trocados;
        int index_retirada = (rand() % max_index) + 1; // Ponto inicial da fatia a ser retirada

        // Retirando a fatia de rota_origem
        Rota fatia;
        No *aux = rotas[rota_origem_index].rota_i;
        int index_aux = 0;
        while (index_aux < index_retirada && aux != NULL)
        {
            aux = aux->proximo;
            index_aux++;
        }

        // Aux agora é o primeiro termo da fatia
        // Tenho que fazer "á força bruta" pq eu não posso fazer deepcopy sem quebrar a lógica de ponteiros

        No *atual;
        atual->estacao = aux->estacao;
        fatia.rota_i = atual;

        int fatia_size = 1;
        aux = aux->proximo;

        while (fatia_size != qtd_elementos_trocados)
        {
            No *novo_no;
            novo_no->anterior = atual;
            atual->proximo = novo_no;
            novo_no->estacao = aux->estacao;

            aux = aux->proximo;
            atual = novo_no;

            fatia_size++;
        }
        fatia.rota_f = atual;

        // Removendo a fatia
        for (int i = 0; i < qtd_elementos_trocados; i++)
            rotas[rota_origem_index].RemoveAt(index_retirada);

        int index_insercao = (rand() % (rotas[rota_destino_index].rotaTam - 1)) + 1;
        // Acha o nó que deve ser inserido
        No *aux_insercao = rotas[rota_destino_index].rota_i;
        index_aux = 0;
        while (index_aux != index_insercao)
        {
            aux_insercao = aux_insercao->proximo;
            index_aux++;
        }

        // aux_insercao está no nó que fará fronteira com os nós da fatia
        // Antes: NO_NATIVO --> NO_NATIVO
        // Depois: NO_ATIVO <--> {FATIA} <--> NO_NATIVO
        fatia.rota_i->anterior = aux_insercao;
        fatia.rota_f->proximo = aux_insercao->proximo;
        aux_insercao->proximo->anterior = fatia.rota_f;
        aux_insercao->proximo = fatia.rota_i;

        if (rotas[rota_origem_index].rotaTam <= 2)
            rotas.erase(rotas.begin() + rota_origem_index);

        trocas_a_realizar--;
    }

    return rotas;
}

vector<Rota> ILS_newroute(vector<Rota> rotas, int qtd_elementos)
{
    // Array de estações que montarão uma nova rota ao fim da função
    vector<int> elementos_retirados;
    while (qtd_elementos > 0)
    {
        // Construção de rotas válidas:
        vector<int> rotas_validas;
        int index_aux = 0;

        for (Rota rota : rotas)
        {
            if (rota.rotaTam > 2)
                rotas_validas.push_back(index_aux);

            index_aux++;
        }

        int aux_rota_origem = rand() % rotas_validas.size();
        int rota_origem = rotas_validas[aux_rota_origem];
        int indice_retirada = (rand() % (rotas[rota_origem].rotaTam - 2)) + 1;

        // Adiciona o termo retirado ao array de elementos retirados
        int estacao = rotas[rota_origem].stationAtIndex(indice_retirada);
        rotas[rota_origem].RemoveAt(indice_retirada);
        elementos_retirados.push_back(estacao);

        if (rotas[rota_origem].rotaTam <= 2)
            rotas.erase(rotas.begin() + rota_origem);

        qtd_elementos--;
    }

    // Construção da rota achada
    if (elementos_retirados.size() != 0)
    {
        // Construção da rota
        Rota nova_rota;
        No *inicio;
        inicio->estacao = 0;
        No *fim;
        fim->estacao = 0;
        nova_rota.rota_i = inicio;
        nova_rota.rota_f = fim;

        for (int estacao : elementos_retirados)
        {
            nova_rota.InsertEnd(estacao);
        }

        // Adição a rotas
        rotas.push_back(nova_rota);
    }

    return rotas;
}

vector<Rota> ILS_recorte(vector<Rota> rotas)
{
    int random_route = rand() % rotas.size();               // Seleciona aleatoriamente uma das rotas
    int metade_tam = (rotas[random_route].rotaTam - 2) / 2; // Exclui depositos e pega o ponto médio

    // Achando ponto médio
    No *limiar_esq = rotas[random_route].rota_i;
    int index_aux = 0;
    while (index_aux != metade_tam)
    {
        limiar_esq = limiar_esq->proximo;
        index_aux++;
    }

    No *limiar_dir = limiar_esq->proximo;

    // Variaveis futuras para calculo de gasto
    int estacao_a0X = rotas[random_route].rota_i->proximo->estacao;  // Primeira estação antes do recorte, vai fazer parte do limiar entre recorte após este
    int estacao_aY0 = rotas[random_route].rota_f->anterior->estacao; // Ultima estação antes do deposito antes do recorte

    // Unindo centros
    rotas[random_route].rota_i->proximo->anterior = rotas[random_route].rota_f->anterior;
    rotas[random_route].rota_f->anterior->proximo = rotas[random_route].rota_i->proximo;

    // Conectando limiar_esq
    limiar_esq->proximo = rotas[random_route].rota_f;
    rotas[random_route].rota_f->anterior = limiar_esq;

    // Conectando limiar_dir
    limiar_dir->anterior = rotas[random_route].rota_i;
    rotas[random_route].rota_i->proximo = limiar_dir;

    // Refatoramento do calculo
    rotas[random_route].custo_total_d1 = rotas[random_route].custo_total_d1 - p.matriz_custo[0][estacao_a0X] - p.matriz_custo[estacao_aY0][0] - p.matriz_custo[limiar_esq->estacao][limiar_dir->estacao] + p.matriz_custo[0][limiar_dir->estacao] + p.matriz_custo[estacao_aY0][estacao_a0X] + p.matriz_custo[limiar_esq->estacao][0];
    rotas[random_route].custo_total_d2 = rotas[random_route].custo_total_d2 - p.matriz_custo[estacao_a0X][0] - p.matriz_custo[0][estacao_aY0] - p.matriz_custo[limiar_dir->estacao][limiar_esq->estacao] + p.matriz_custo[limiar_dir->estacao][0] + p.matriz_custo[estacao_a0X][estacao_aY0] + p.matriz_custo[0][limiar_esq->estacao];

    return rotas;
}

vector<Rota> Perturbacao(vector<Rota> rotas, PerturbacaoOpt perturbacao_escolhida, int grau_perturbacao)
{

    switch (perturbacao_escolhida)
    {
    case SWITCH:
        return ILS_switch(rotas, 3 * grau_perturbacao);
    case RELOCATE:
        return ILS_relocate(rotas, grau_perturbacao, grau_perturbacao);
    case RELOCATE_REVERSE:
        return ILS_relocate(rotas, grau_perturbacao, grau_perturbacao, true);
    case NEWROUTE:
        return ILS_newroute(rotas, 2 * grau_perturbacao);
    case RECORTE:
        return ILS_recorte(rotas);
    default:
        return rotas;
    }
}

void *executeThreadsILS(void *atributos)
{
    ParametrosILS *parametros_ILS = (ParametrosILS *)atributos;

    Solucao solucao_encontrada = ILS(parametros_ILS->solucao, parametros_ILS->max_iteracoes, parametros_ILS->max_sem_melhora);

    // Após cada função terminar de executar, comparamos os valores
    // É necessario que a barreira seja inicializada fora desta função com pthread_barrier_init(&barreira_ILS, nullptr, NUM_THREADS);
    pthread_barrier_wait(&barreira_ILS);

    // Entrada em SC
    pthread_mutex_lock(&mutex_melhorSolucaoCheck_ILS);
    if (solucao_encontrada.custo_total < melhor_solucao.custo_total)
        melhor_solucao = solucao_encontrada;
    pthread_mutex_unlock(&mutex_melhorSolucaoCheck_ILS); // Saída da SC

    delete parametros_ILS;
    return nullptr;
}

Solucao ILS(Solucao solucao, int max_iteracoes, int max_sem_melhora)
{
    int iteracoes_current = 0;
    int sem_melhora_current = 0;

    while (iteracoes_current < max_iteracoes && sem_melhora_current < max_sem_melhora)
    {
        int grau_perturbacao = (sem_melhora_current / (max_sem_melhora / 7)) + 1;
        PerturbacaoOpt opcao_perturbacao;
        int index_escolha = rand() % 5;
        switch (index_escolha)
        {
        case 0:
            opcao_perturbacao = SWITCH;
            break;
        case 1:
            opcao_perturbacao = RELOCATE;
            break;
        case 2:
            opcao_perturbacao = RELOCATE_REVERSE;
            break;
        case 3:
            opcao_perturbacao = NEWROUTE;
            break;
        case 4:
            opcao_perturbacao = RECORTE;
            break;
        default:
            break;
        }

        vector<Rota> rotas_copia = solucao.rotas;

        vector<Rota> rotas_encontradas = Perturbacao(rotas_copia, opcao_perturbacao, grau_perturbacao);
        rotas_encontradas = MelhorarSolucao(rotas_encontradas);

        int custo_atual = SomaCusto(solucao.rotas);
        int custo_novo = SomaCusto(rotas_encontradas);
        bool demandas_validas = true;
        for (Rota rota : rotas_encontradas)
            demandas_validas = demandas_validas && ValidaDemanda(rota);

        if ((custo_novo < custo_atual) && (demandas_validas))
        {
            solucao.rotas = rotas_encontradas;
            sem_melhora_current = 0;
            // Linha de treino e output
        }
        else
        {
            sem_melhora_current++;
        }
        iteracoes_current++;
    }

    // Montando objeto Solucao
    Solucao solucao_encontrada;
    solucao_encontrada.rotas = solucao.rotas;
    solucao_encontrada.veiculos_usados = solucao.rotas.size();

    // Calculo do custo total por direção
    int custo_total_d1 = 0;
    int custo_total_d2 = 0;

    for (Rota rota : solucao_encontrada.rotas)
    {
        custo_total_d1 += rota.custo_total_d1;
        custo_total_d2 += rota.custo_total_d2;
    }
    custo_total_d1 >= custo_total_d2 ? solucao_encontrada.custo_total = custo_total_d1 : solucao_encontrada.custo_total = custo_total_d2;

    return solucao_encontrada;
}