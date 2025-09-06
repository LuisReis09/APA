#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <cstdlib>
#include <cmath>
#include <utility>
#include <map>
#include <algorithm>
#include "metodos.hpp"
using namespace std;


Solucao VizinhoMaisProximo()
{
    /*
        Implementa o algoritmo de vizinho mais próximo para cada conjunto
        Retorna a lista de rotas (listas de índices) e o custo total
    */

    vector<vector<int>> rotas;
    vector<int> necessidades_rotas;
    int custo_total = 0;
    

    // Inicialização das estações que precisam ser visitadas
    vector<int> restam_visitar;
    for (int i = 1; i < p.matriz_custo[0].size(); i++)
        restam_visitar.push_back(i);

    for (int i = 0; i < p.qnt_veiculos; i++)
    {
        necessidades_rotas.push_back(0);
        vector<int> inicio = {0};
        rotas.push_back(inicio);
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
    // Estrutura do tipo: {1, {2, 5}}
    map<int, map<int, int>> fila_prioridade; // Se trata de um mapa de mapas

    // Cada fila de prioridade recebe uma cópia de fila
    for (int r = 0; r < rotas.size(); r++)
        fila_prioridade[r] = fila; // Estrutura do tipo {id_rota : {{1, 2}, {3, 4} }}

    vector<int> rotas_a_atualizar;
    for (int id_rota = 0; id_rota < rotas.size(); id_rota++)
        rotas_a_atualizar.push_back(id_rota);

    // Enquanto houver estações para visitar
    while (restam_visitar.size() != 0)
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

        int melhor_rota = (melhor_it != rotas_a_atualizar.end()) ? *melhor_it : -1;

        // Seleciona a estação de menor custo : Necessario achar o indice
        int index_menorCusto;
        auto it = min_element( // Retorna o par chave : valor
            fila_prioridade[melhor_rota].begin(),
            fila_prioridade[melhor_rota].end(),
            [](const auto &a, const auto &b)
            { return a.second < b.second; });

        int estacao_escolhida = fila_prioridade[melhor_rota][it->second]; // Supõe se que os elementos de menor custo vem primeiro
        int custo_escolhido = fila_prioridade[melhor_rota][estacao_escolhida];

        rotas[melhor_rota].push_back(estacao_escolhida);
        necessidades_rotas[melhor_rota] += p.demandas[estacao_escolhida];

        custo_total += custo_escolhido;

        // Removendo por valor
        restam_visitar.erase(
            remove(restam_visitar.begin(),
                   restam_visitar.end(),
                   estacao_escolhida),
            restam_visitar.end());

        // Para a rota que acabou de inserir, é necessário reajustar sua fila de prioridades, considerando a nova estação final
        map<int, int> ajuste;
        for (int estacao : restam_visitar)
        {
            if (std::abs(necessidades_rotas[melhor_rota] + p.demandas[estacao]) <= p.capacidade_max)
                ajuste[estacao] = p.matriz_custo[estacao_escolhida][estacao];
        }
        fila_prioridade[melhor_rota] = ajuste;

        // Remove a estação escolhida de todas as filas
        for (int i : rotas_a_atualizar)
            fila_prioridade[i].erase(estacao_escolhida);

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
    for (vector<int> rota : rotas)
    {
        custo_total += p.matriz_custo[rota[rota.size() - 1]][0];
        rota.push_back(0);
    }

    // Construção do Objeto Solução e do array de rotas
    Solucao solucao_encontrada;
    solucao_encontrada.custo_total = custo_total;
    solucao_encontrada.veiculos_usados = p.qnt_veiculos;
    solucao_encontrada.veiculos_disponiveis = 0;

    vector<int> rota_vazia = {0, 0};
    vector<Rota> rotas_solucao;
    for (vector<int> rota : rotas)
    {
        if (rota == rota_vazia)
        {
            solucao_encontrada.veiculos_usados--;
            solucao_encontrada.veiculos_disponiveis++;
        }

        // Construção do objeto Rota
        Rota route;
        No *inicio;
        No *fim;
        inicio->estacao = 0;
        fim->estacao = 0;

        route.rota_i = inicio;
        route.rota_f = fim;
        route.custo_total_d1 = 0;
        route.custo_total_d2 = 0;

        No *no_atual = route.rota_i;
        no_atual->soma_demandas_d1 = 0;
        no_atual->soma_demandas_d2 = 0;

        for (int index_rota = 1; index_rota < rota.size() - 1; index_rota++)
        {
            // Construção de um novo nó e inserção na Rota

            No *novo_no;
            novo_no->soma_demandas_d1 = no_atual->soma_demandas_d1;
            novo_no->soma_demandas_d2 = no_atual->soma_demandas_d2;

            novo_no->estacao = rota[index_rota];

            no_atual->proximo = novo_no;
            no_atual->custo_d1 = p.matriz_custo[no_atual->estacao][novo_no->estacao];
            route.custo_total_d1 += no_atual->custo_d1;

            if (index_rota != 1)
            { // Nesse caso em que index_rota == 1, no_atual é nó de inicio, logo, não tem anterior
                no_atual->custo_d2 = p.matriz_custo[no_atual->anterior->estacao][no_atual->estacao];
                no_atual->anterior->soma_demandas_d2 += p.demandas[novo_no->estacao];
            }

            novo_no->anterior = no_atual;
            route.custo_total_d2 += p.matriz_custo[index_rota][no_atual->estacao];

            novo_no->soma_demandas_d1 += p.demandas[novo_no->estacao];
            no_atual->soma_demandas_d2 += p.demandas[novo_no->estacao];

            // soma de d2 em rota_i sempre será igual ao somatorio de d2
            route.rota_i->soma_demandas_d2 += novo_no->soma_demandas_d1;

            no_atual = novo_no;
        }
        no_atual->proximo = route.rota_f;
        route.rota_f->anterior = no_atual;
        route.rota_f->soma_demandas_d1 = route.rota_i->soma_demandas_d2;

        rotas_solucao.push_back(route);
    }

    solucao_encontrada.rotas = rotas_solucao;

    return solucao_encontrada;
}