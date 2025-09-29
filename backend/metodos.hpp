#ifndef _METODOS_HPP_
#define _METODOS_HPP_

#include <iostream>
#include <stdlib.h>
#include <vector>
#include "structures.hpp"
#include "utils.hpp"
#include <time.h>
#include <map>
#include "VND1.hpp"
#include <functional>
#include <random>
#include <algorithm>

using namespace std;

/**
 * @brief Utilizando do algoritmo guloso de Vizinho Mais Proximo, procura dentro dos dados do problema estabelecido e cria uma solução
 * ---
 * @return Retorna um objeto Solução com as rotas achadas
 */
Solucao VizinhoMaisProximo()
{
    vector<vector<int>> rotas;
    rotas.reserve(p.qnt_veiculos);
    vector<int> visitados(p.qnt_estacoes, false);
    int custo_total = 0;
    int qtd_iteracoes = p.qnt_estacoes;

    typedef struct{
        int lower = 0;
        int upper = p.capacidade_max;
        int prefix = 0;
    } Range;

    vector<Range> range_demandas(p.qnt_veiculos);

    typedef vector<vector<pair<int, int>>> FilaPrioridade;
    FilaPrioridade fila_prioridade(p.qnt_estacoes + 1);

    // Para cada estação ...
    for (int i = 0; i < (p.qnt_estacoes + 1); i++)
    {

        // Calcula os custos a partir daquele ponto
        vector<pair<int, int>> custos(p.qnt_estacoes);
        for (int j = 0; j < p.qnt_estacoes; j++)
        {
            custos[j] = make_pair(j + 1, p.matriz_custo[i][j + 1]);
        }

        // E ordena os custos de maneira crescente
        sort(custos.begin(), custos.end(),
             [](const pair<int, int> &a, const pair<int, int> &b)
             {
                 return a.second < b.second;
             });

        // Por fim adiciona os custos achados à fila de prioridade
        fila_prioridade[i] = custos;
    }

    // A cada iteração uma estação encontrará uma rota a pertencer
    while(qtd_iteracoes--){
        int melhor_rota = -1, melhor_custo = MAXX_INT, melhor_estacao;

        // Para cada rota ...
        for (int i = 0; i < rotas.size(); i++)
        {
            int ultimo_elemento = rotas[i].back();

            // Percorre a fila de prioridade
            for (int j = 0; j < fila_prioridade[ultimo_elemento].size(); j++)
            {
                int estacao = fila_prioridade[ultimo_elemento][j].first;
                int custo = fila_prioridade[ultimo_elemento][j].second;

                // Testa se a rota, ao adicionar essa estação, permanece válida
                int prefix = range_demandas[i].prefix + p.demandas[estacao - 1];
                bool pode_inserir = (max(range_demandas[i].lower, -prefix) <= min(range_demandas[i].upper, p.capacidade_max - prefix));
                // bool pode_inserir = true;
                if (pode_inserir && !visitados[estacao - 1])
                {
                    if (custo < melhor_custo) // Se a inserção daquela estação for de fato a menor encontrada até então
                    {                         // Substitui os valores de melhor_custo, melhor_rota e melhor_estacao
                        melhor_custo = custo;
                        melhor_rota = i;
                        melhor_estacao = estacao;
                    }
                    break; // nao adianta olhar as outras, pq serao mais caras
                }
            }
        }

        if(melhor_rota == -1){
            // Se não encontrou nenhuma rota possível de inserção, cria uma nova rota o mais próximo não visitado.
            melhor_estacao = -1;
            for(int i = 0; i < fila_prioridade[0].size(); i++){
                if(!visitados[fila_prioridade[0][i].first - 1]){
                    melhor_estacao = fila_prioridade[0][i].first;
                    break;
                }
            }

            rotas.push_back({0, melhor_estacao});
            custo_total += p.matriz_custo[0][melhor_estacao];
            visitados[melhor_estacao - 1] = true;

            melhor_rota = rotas.size() - 1;
            range_demandas[melhor_rota].prefix += p.demandas[melhor_estacao - 1];
            range_demandas[melhor_rota].lower = max(range_demandas[melhor_rota].lower, -range_demandas[melhor_rota].prefix);
            range_demandas[melhor_rota].upper = min(range_demandas[melhor_rota].upper, p.capacidade_max - range_demandas[melhor_rota].prefix);
        }else{
            // Agora que temos a melhor rota e a melhor estação, inserimos, caso não compense mais ir da base a estacao

            int custo_nova_rota = p.matriz_custo[0][melhor_estacao] + p.matriz_custo[melhor_estacao][0];

            if(custo_nova_rota < melhor_custo){
                // Caso em que compensa mais criar uma nova rota
                rotas.push_back({0, melhor_estacao});
                custo_total += p.matriz_custo[0][melhor_estacao];
                visitados[melhor_estacao - 1] = true;

                melhor_rota = rotas.size() - 1;
                range_demandas[melhor_rota].prefix += p.demandas[melhor_estacao - 1];
                range_demandas[melhor_rota].lower = max(range_demandas[melhor_rota].lower, -range_demandas[melhor_rota].prefix);
                range_demandas[melhor_rota].upper = min(range_demandas[melhor_rota].upper, p.capacidade_max - range_demandas[melhor_rota].prefix);

            }else{
                // Caso em que a rota encontrada realmente é melhor
                rotas[melhor_rota].push_back(melhor_estacao);
                custo_total += melhor_custo;
                visitados[melhor_estacao - 1] = true;

                range_demandas[melhor_rota].prefix += p.demandas[melhor_estacao - 1];
                range_demandas[melhor_rota].lower = max(range_demandas[melhor_rota].lower, -range_demandas[melhor_rota].prefix);
                range_demandas[melhor_rota].upper = min(range_demandas[melhor_rota].upper, p.capacidade_max - range_demandas[melhor_rota].prefix);
            }
        }

        // Remove a estação escolhida das filas de prioridades
        for (int i = 0; i < fila_prioridade.size(); i++)
        {
            for (int j = 0; j < fila_prioridade[i].size(); j++)
            {
                if (fila_prioridade[i][j].first == melhor_estacao)
                {
                    fila_prioridade[i].erase(fila_prioridade[i].begin() + j);
                    break;
                }
            }
        }
    }

    // Construção do objeto Solucao
    Solucao solucao;
    solucao.rotas.resize(rotas.size());
    for (int i = 0; i < rotas.size(); i++)
    {
        custo_total += p.matriz_custo[rotas[i].back()][0];
        rotas[i].push_back(0); // volta para o depósito, rota ficara com formato {0, X, Y, Z, 0}
        solucao.rotas[i] = rotas[i];
    }

    solucao.custo_total = custo_total;
    solucao.veiculos_usados = solucao.rotas.size();
    solucao.veiculos_disponiveis = p.qnt_veiculos - solucao.veiculos_usados;
    return solucao;
}

Solucao VizinhoMaisProximo2()
{
    vector<vector<int>> rotas;
    rotas.reserve(p.qnt_veiculos);
    vector<int> visitados(p.qnt_estacoes, false);
    int custo_total = 0;
    int qtd_iteracoes = p.qnt_estacoes;

    typedef struct{
        int lower = 0;
        int upper = p.capacidade_max;
        int prefix = 0;
    } Range;

    vector<Range> range_demandas(p.qnt_veiculos);

    typedef vector<vector<pair<int, int>>> FilaPrioridade;
    FilaPrioridade fila_prioridade(p.qnt_estacoes + 1);

    // Para cada estação ...
    for (int i = 0; i < (p.qnt_estacoes + 1); i++)
    {

        // Calcula os custos a partir daquele ponto
        vector<pair<int, int>> custos(p.qnt_estacoes);
        for (int j = 0; j < p.qnt_estacoes; j++)
        {
            custos[j] = make_pair(j + 1, p.matriz_custo[i][j + 1]);
        }

        // E ordena os custos de maneira crescente
        sort(custos.begin(), custos.end(),
             [](const pair<int, int> &a, const pair<int, int> &b)
             {
                 return a.second < b.second;
             });

        // Por fim adiciona os custos achados à fila de prioridade
        fila_prioridade[i] = custos;
    }

    // A cada iteração uma estação encontrará uma rota a pertencer
    while(qtd_iteracoes--){
        int melhor_rota = -1, melhor_custo = MAXX_INT, melhor_estacao;

        // Para cada rota ...
        for (int i = 0; i < rotas.size(); i++)
        {
            int ultimo_elemento = rotas[i].back();

            // Percorre a fila de prioridade
            for (int j = 0; j < fila_prioridade[ultimo_elemento].size(); j++)
            {
                int estacao = fila_prioridade[ultimo_elemento][j].first;
                int custo = fila_prioridade[ultimo_elemento][j].second;

                // Testa se a rota, ao adicionar essa estação, permanece válida
                int prefix = range_demandas[i].prefix + p.demandas[estacao - 1];
                bool pode_inserir = (max(range_demandas[i].lower, -prefix) <= min(range_demandas[i].upper, p.capacidade_max - prefix));
                // bool pode_inserir = true;
                if (pode_inserir && !visitados[estacao - 1])
                {
                    if (custo < melhor_custo) // Se a inserção daquela estação for de fato a menor encontrada até então
                    {                         // Substitui os valores de melhor_custo, melhor_rota e melhor_estacao
                        melhor_custo = custo;
                        melhor_rota = i;
                        melhor_estacao = estacao;
                    }
                    break; // nao adianta olhar as outras, pq serao mais caras
                }
            }
        }

        if(melhor_rota == -1){
            // Se não encontrou nenhuma rota possível de inserção, cria uma nova rota o mais próximo não visitado.
            melhor_estacao = -1;
            for(int i = 0; i < fila_prioridade[0].size(); i++){
                if(!visitados[fila_prioridade[0][i].first - 1]){
                    melhor_estacao = fila_prioridade[0][i].first;
                    break;
                }
            }

            rotas.push_back({0, melhor_estacao});
            custo_total += p.matriz_custo[0][melhor_estacao];
            visitados[melhor_estacao - 1] = true;

            melhor_rota = rotas.size() - 1;
            range_demandas[melhor_rota].prefix += p.demandas[melhor_estacao - 1];
            range_demandas[melhor_rota].lower = max(range_demandas[melhor_rota].lower, -range_demandas[melhor_rota].prefix);
            range_demandas[melhor_rota].upper = min(range_demandas[melhor_rota].upper, p.capacidade_max - range_demandas[melhor_rota].prefix);
        }else{
            // Agora que temos a melhor rota e a melhor estação, inserimos
            rotas[melhor_rota].push_back(melhor_estacao);
            custo_total += melhor_custo;
            visitados[melhor_estacao - 1] = true;

            range_demandas[melhor_rota].prefix += p.demandas[melhor_estacao - 1];
            range_demandas[melhor_rota].lower = max(range_demandas[melhor_rota].lower, -range_demandas[melhor_rota].prefix);
            range_demandas[melhor_rota].upper = min(range_demandas[melhor_rota].upper, p.capacidade_max - range_demandas[melhor_rota].prefix);
        }

        // Remove a estação escolhida das filas de prioridades
        for (int i = 0; i < fila_prioridade.size(); i++)
        {
            for (int j = 0; j < fila_prioridade[i].size(); j++)
            {
                if (fila_prioridade[i][j].first == melhor_estacao)
                {
                    fila_prioridade[i].erase(fila_prioridade[i].begin() + j);
                    break;
                }
            }
        }
    }

    // Construção do objeto Solucao
    Solucao solucao;
    solucao.rotas.resize(rotas.size());
    for (int i = 0; i < rotas.size(); i++)
    {
        custo_total += p.matriz_custo[rotas[i].back()][0];
        rotas[i].push_back(0); // volta para o depósito, rota ficara com formato {0, X, Y, Z, 0}
        solucao.rotas[i] = rotas[i];
    }

    solucao.custo_total = custo_total;
    solucao.veiculos_usados = solucao.rotas.size();
    solucao.veiculos_disponiveis = p.qnt_veiculos - solucao.veiculos_usados;
    return solucao;
}

/**
 * @brief Utilizando do algoritmo guloso de Inserção Mais Barata, procura dentro dos dados do problema estabelecido e cria uma solução
 * ---
 * @return Retorna um objeto Solução com as rotas achadas
 */
Solucao IMB()
{
    // Cria um vetor de rotas do tipo {0, 0}
    vector<vector<int>> rotas(p.qnt_veiculos, vector<int>(2, 0));
    rotas[0].insert(rotas[0].begin() + 1, 1); // inicia a primeira rota com a estação 1, ficando {0, 1, 0}
    for (int i = 0; i < p.qnt_veiculos; i++)
        rotas[i].reserve(p.qnt_estacoes / p.qnt_veiculos + 2);

    int caminhoes_usados = 1;
    int custo_total = 0;

    // Para cada estação ...
    for (int i = 2; i <= p.qnt_estacoes; i++)
    {
        // Inicialização de variaveis auxiliares
        int menor_custo = MAXX_INT;
        int melhor_rota = -1;
        int melhor_posicao;

        // Para cada caminhão já usado (ou seja, rotas)
        for (int j = 0; j < caminhoes_usados; j++)
        {
            // Examina cada termo daquela rota
            for (int k = 1; k < rotas[j].size(); k++)
            {
                // Calculo do custo daquele inserção naquele ponto
                int custo = p.matriz_custo[rotas[j][k - 1]][i] + p.matriz_custo[i][rotas[j][k]] - p.matriz_custo[rotas[j][k - 1]][rotas[j][k]];
                if (custo < menor_custo && InsertionTest(rotas[j], k, i))
                { // Se for, atualiza as variaveis auxiliares
                    menor_custo = custo;
                    melhor_rota = j;
                    melhor_posicao = k;
                }
            }
        }

        // Se não encontrou nenhuma rota possível de inserção, cria uma nova rota.
        // Não precisamos, em teoria, verificar se há caminhões disponíveis, pois o problema é inviável se não houver.
        if (melhor_rota == -1)
        {
            rotas[caminhoes_usados] = {0, i, 0};
            custo_total += p.matriz_custo[0][i] + p.matriz_custo[i][0];
            caminhoes_usados++;
        }
        else
        { // Insere a rota com a nova estação
            rotas[melhor_rota].insert(rotas[melhor_rota].begin() + melhor_posicao, i);
            custo_total += menor_custo;
        }
    }

    // Montagem do objeto Solução
    Solucao solucao;
    solucao.rotas.resize(caminhoes_usados);
    for (int i = 0; i < caminhoes_usados; i++)
        solucao.rotas[i] = rotas[i];

    solucao.custo_total = custo_total;
    solucao.veiculos_usados = solucao.rotas.size();
    solucao.veiculos_disponiveis = p.qnt_veiculos - solucao.veiculos_usados;
    return solucao;
}

/**
 * @brief Algoritmo parte do ILS. Reverte rotas inteiras.
 * ---
 * @param rotas Vetor de vetor de inteiros. Rotas de uma dada solução
 * @param qtd_reverses Quantia de quantas rotas serão revertidas.
 * ---
 */
void PerturbacaoReverse(vector<vector<int>> &rotas, int qtd_reverses)
{
    if(!qtd_reverses) return;

    int id_a_inverter;

    do{
        id_a_inverter = rand() % rotas.size();
    }while(rotas[id_a_inverter].size() <= 3); // garante que a rota tenha ao menos 2 elementos para inverter

    int aux = 1;
    for(int b = rotas[id_a_inverter].size() - 2; b > aux; b--, aux++){
        swap(rotas[id_a_inverter][aux], rotas[id_a_inverter][b]);
    }

    PerturbacaoReverse(rotas, qtd_reverses - 1);
}

/**
 * @brief Perturbação que cria uma nova rota, movendo elementos de outras rotas para ela
 * Os alvos serao estacoes aleatorias, de outras rotas tambem aleatorias
 * ---
 * @param rotas Array de rotas que serao perturbadas
 * @param qtd_elementos Quantidade de elementos a serem movidos para a nova rota
 */
void PerturbacaoNewRoute(vector<vector<int>> &rotas, int qtd_elementos)
{
    if (p.qnt_veiculos <= rotas.size()) return;

    vector<int> nova_rota; // Inicia apenas com depósito inicial
    nova_rota.reserve(qtd_elementos + 2);
    nova_rota.push_back(0); // Depósito inicial
    
    while (qtd_elementos--)
    {
        int id_rota_origem = rand() % rotas.size();
        if (rotas[id_rota_origem].size() <= 2) continue;
        
        int indice = 1 + rand() % (rotas[id_rota_origem].size() - 2);
        nova_rota.push_back(rotas[id_rota_origem][indice]);
        rotas[id_rota_origem].erase(rotas[id_rota_origem].begin() + indice);
        
    }

    for(int i=0; i<rotas.size(); i++){
        if(rotas[i].size() <= 2){
            rotas.erase(rotas.begin() + i);
            i--;
        }
    }
    
    nova_rota.push_back(0); // Depósito final
    if(nova_rota.size() > 2) // Adiciona a nova rota apenas se tiver elementos
        rotas.push_back(nova_rota);
}

/**
 * @brief Algoritmo parte do ILS. Troca dois termos entre as rotas.
 * ---
 * @param rotas Vetor de vetor de inteiros. Rotas de uma dada solução
 * @param trocar_a_realizar Inteiro representando quantas trocas devem ser feitas. Determinada pelo grau de perturbação.
 * ---
 */
void PerturbacaoSwitch(vector<vector<int>> &rotas, int trocar_a_realizar)
{
    while (trocar_a_realizar--)
    {
        if (rotas.size() < 2) break;
        
        int r1 = rand() % rotas.size();
        int r2 = rand() % rotas.size();
        if (r1 == r2) continue;
        
        if (rotas[r1].size() < 3 || rotas[r2].size() < 3) continue;
        
        int pos1 = 1 + rand() % (rotas[r1].size() - 2);
        int pos2 = 1 + rand() % (rotas[r2].size() - 2);
        
        swap(rotas[r1][pos1], rotas[r2][pos2]);
    }
}

/**
 * @brief Perturbação que troca duas metades de duas rotas sorteadas.
 * ---
 * @param rotas Array de rotas da solução.
 */
void PerturbacaoHalfSwapRoutes(vector<vector<int>> &rotas)
{
    if (rotas.size() < 2) return;

    // Encontrar duas rotas válidas
    int rota1 = -1, rota2 = -1;
    for (int tentativa = 0; tentativa < 50; tentativa++) {
        int r1 = rand() % rotas.size();
        int r2 = rand() % rotas.size();
        
        if (r1 != r2 && rotas[r1].size() >= 4 && rotas[r2].size() >= 4) {
            rota1 = r1;
            rota2 = r2;
            break;
        }
    }
    
    if (rota1 == -1 || rota2 == -1) return; // Não encontrou rotas válidas

    // Calcular limites seguros
    int estacoes1 = rotas[rota1].size() - 2;
    int estacoes2 = rotas[rota2].size() - 2;
    
    // Determinar quantas estações trocar (mínimo 1, máximo metade da menor)
    int max_trocas = min(estacoes1, estacoes2);
    int trocas = max(1, rand() % (max_trocas / 2 + 1));

    // Escolher posições iniciais aleatórias
    int inicio1 = 1 + (rand() % (estacoes1 - trocas + 1));
    int inicio2 = 1 + (rand() % (estacoes2 - trocas + 1));

    // Realizar as trocas
    for (int i = 0; i < trocas; i++) {
        swap(rotas[rota1][inicio1 + i], rotas[rota2][inicio2 + i]);
    }
}

/**
 * @brief Perturbação que troca duas metades de uma rota sorteada.
 * ---
 * @param rotas Array de rotas da solução.
 * @param reverso Booleano indicando se reversão da rota deve ser completa
 */
void PerturbacaoHalfSwap(vector<vector<int>> &rotas)
{
    if (rotas.empty()) return;

    int rota_alvo;

    // Sorteia uma rota válida
    do
    {
        rota_alvo = rand() % rotas.size();
    } while (rotas[rota_alvo].size() < 5); 
    // mínimo 5: {0, a, b, c, 0} → para garantir 2 metades não-triviais

    int n = rotas[rota_alvo].size();
    int metade = n / 2;

    
    // Troca as duas metades mantendo ordem da segunda metade
    // Exemplo: {0, 1, 2, 3, 4, 0} → {0, 3, 4, 1, 2, 0}
    int i = 1, j = metade;
    while (i < metade && j < n - 1)
    {
        swap(rotas[rota_alvo][i], rotas[rota_alvo][j]);
        i++;
        j++;
    }
}

/**
 * @brief Algoritmo parte do ILS. Escolhe duas rotas aleatorias para se fundirem em uma só
 * ---
 * @param rotas Vetor de vetor de inteiros. Rotas de uma dada solução
 * ---
 */
void PerturbacaoMergeRoutes(vector<vector<int>> &rotas)
{
    // Escolhe duas rotas aleatorias para dar merge
    if (rotas.size() == 1)
        return;
    int id1, id2;

    // Sorteia duas rotas aleatorias para se fundirem
    do
    {
        id1 = rand() % rotas.size();
        id2 = rand() % rotas.size();
    } while (id1 == id2);

    rotas[id1].reserve(rotas[id1].size() + rotas[id2].size() - 2);

    rotas[id1].pop_back(); // Remove o depósito final da primeira rota
    for(int j = 1; j < rotas[id2].size(); j++) // Começa do 1 para pular o depósito inicial
        rotas[id1].push_back(rotas[id2][j]); // Adiciona todos os

    rotas.erase(rotas.begin() + id2); // Remove a segunda rota
    return;
}

/**
 * @brief Algoritmo parte do ILS. Perturbar ao, caso haja caminhões o suficiente, quebrar uma rota no meio
 * ---
 * @param rotas Vetor de vetor de inteiros. Rotas de uma dada solução
 * ---
 */
void PerturbacaoSplitRoute(vector<vector<int>> &rotas)
{
    if (rotas.size() >= p.qnt_veiculos) return;

    int id;
    do { id = rand() % rotas.size(); } 
    while (rotas[id].size() < 4);

    int meio = 1 + (rotas[id].size() - 2) / 2; // Ponto de divisão

    // Nova rota (segunda metade)
    vector<int> nova_rota;
    nova_rota.push_back(0); // Depósito inicial
    nova_rota.insert(nova_rota.end(), 
                    rotas[id].begin() + meio, 
                    rotas[id].end() - 1); // Exceto último 0
    nova_rota.push_back(0); // Depósito final

    // Rota original (primeira metade)
    rotas[id].erase(rotas[id].begin() + meio, rotas[id].end());
    rotas[id].push_back(0); // Garante depósito final

    rotas.push_back(nova_rota);
}

/**
 * @brief Algoritmo parte do ILS. Switch-case entre as várias opções de perturbação.
 * ---
 * @param rotas Vetor de vetor de inteiros. Rotas de uma dada solução
 * @param opcao Inteiro randomizado que reflete a escolha da perturbação.
 * @param nivel_perturbacao Grau de perturbação que será utilizado nas funções para determinar o quão grave deve ser a perturbação.
 * ---
 */
void Perturbar(vector<vector<int>> &rotas, int opcao, int nivel_perturbacao)
{
    switch (opcao)
    {
    case 1:
        PerturbacaoNewRoute(rotas, 2 * nivel_perturbacao);
        break;
    case 2:
        PerturbacaoMergeRoutes(rotas);
        break;
    case 3:
        PerturbacaoSplitRoute(rotas);
        break;
    case 4:
        PerturbacaoHalfSwap(rotas);
        break;
    case 5:
        PerturbacaoHalfSwapRoutes(rotas);
        break;
    case 6:
        PerturbacaoReverse(rotas, nivel_perturbacao);
        break;
    }
}

int VNDSwap(vector<vector<int>> &rotas)
{
    int custo_teste;
    int melhor_custo, melhor_troca_r, melhor_troca_e;

    for (int id_r1 = 0; id_r1 < rotas.size(); id_r1++)
    {
        for (int id_e1 = 1; id_e1 < (rotas[id_r1].size() - 1); id_e1++)
        {
            int e1_ant = rotas[id_r1][id_e1 - 1];
            int e1 = rotas[id_r1][id_e1];
            int e1_pos = rotas[id_r1][id_e1 + 1];
            melhor_custo = 0;

            for (int id_r2 = id_r1; id_r2 < rotas.size(); id_r2++)
            {
                for (int id_e2 = 1; id_e2 < (rotas[id_r2].size() - 1); id_e2++)
                {
                    int e2_ant = rotas[id_r2][id_e2 - 1];
                    int e2 = rotas[id_r2][id_e2];
                    int e2_pos = rotas[id_r2][id_e2 + 1];

                    if (e1 == e2)
                    {
                        continue;
                    }

                    custo_teste = -p.matriz_custo[e1_ant][e1];
                    custo_teste -= p.matriz_custo[e1][e1_pos];
                    custo_teste -= p.matriz_custo[e2_ant][e2];
                    custo_teste -= p.matriz_custo[e2][e2_pos];

                    custo_teste += p.matriz_custo[e1_ant][e2];
                    custo_teste += p.matriz_custo[e2][e1_pos];
                    custo_teste += p.matriz_custo[e2_ant][e1];
                    custo_teste += p.matriz_custo[e1][e2_pos];

                    if(custo_teste < melhor_custo){
                        if(id_r1 == id_r2){
                            if(VerificaSwapIntraVND(rotas[id_r1], id_e1, id_e2)){
                                melhor_custo = custo_teste;
                                melhor_troca_r = id_r2;
                                melhor_troca_e = id_e2;
                            }
                        }else{
                            if(VerificaSwapInterVND(rotas[id_r1], id_e1, e2) && VerificaSwapInterVND(rotas[id_r2], id_e2, e1)){
                                melhor_custo = custo_teste;
                                melhor_troca_r = id_r2;
                                melhor_troca_e = id_e2;
                            }
                        }
                    }
                }
            }

            if (melhor_custo < 0)
            {
                swap(rotas[id_r1][id_e1], rotas[melhor_troca_r][melhor_troca_e]);
            }
        }
    }

    return CustoTotal(rotas);
}

int VNDTwoOpt(vector<vector<int>> &rotas)
{
    for (int r1 = 0; r1 < rotas.size(); r1++)
    {
        for (int r2 = 0; r2 < rotas.size(); r2++)
        {
            vector<int> &rota1 = rotas[r1];
            vector<int> &rota2 = rotas[r2];

            int n1 = rota1.size();
            int n2 = rota2.size();

            // Caso 1: intra-rota
            if (r1 == r2 && n1 >= 6)
            {
                for (int e1 = 1; e1 < (n1 - 3); e1++)
                {
                    for (int e2 = e1 + 2; e2 < (n1 - 2); e2++)
                    {
                        int a, b, c, d;
                        a = rota1[e1];
                        b = rota1[e1 + 1];
                        c = rota2[e2];
                        d = rota2[e2 + 1];

                        int delta = 0;
                        delta -= p.matriz_custo[a][b];
                        delta -= p.matriz_custo[c][d];
                        delta += p.matriz_custo[a][c];
                        delta += p.matriz_custo[b][d];

                        if (delta < 0)
                        {
                            vector<int> copia = rota1;

                            // Aplicando o reverse na mao
                            int aux1 = e1 + 1, aux2 = e2;
                            while (aux1 < aux2)
                            {
                                swap(copia[aux1], copia[aux2]);
                                aux1++;
                                aux2--;
                            }
                            if (VerificaDemanda(copia) && CustoRota(copia) < CustoRota(rota1))
                            {
                                rota1 = copia;
                            }
                        }
                    }
                }

                // Caso 2: inter-rotas
            }
            else if (n1 >= 4 && n2 >= 4)
            {
                for (int e1 = 1; e1 < (n1 - 2); e1++)
                {
                    for (int e2 = 1; e2 < (n2 - 2); e2++)
                    {
                        int a, b, c, d;
                        a = rota1[e1];
                        b = rota1[e1 + 1];
                        c = rota2[e2];
                        d = rota2[e2 + 1];

                        int delta = 0;
                        delta -= p.matriz_custo[a][b];
                        delta -= p.matriz_custo[c][d];
                        delta += p.matriz_custo[a][c];
                        delta += p.matriz_custo[b][d];

                        if (delta < 0)
                        {
                            vector<int> copia1 = rota1;
                            vector<int> copia2 = rota2;

                            // Aplicando o reverse na mao
                            int aux1 = e1 + 1, aux2 = n1 - 2;
                            while (aux1 < aux2)
                            {
                                swap(copia1[aux1], copia1[aux2]);
                                aux1++;
                                aux2--;
                            }

                            aux1 = e2;
                            aux2 = n2 - 2;
                            while (aux1 < aux2)
                            {
                                swap(copia2[aux1], copia2[aux2]);
                                aux1++;
                                aux2--;
                            }

                            if (VerificaDemanda(copia1) && VerificaDemanda(copia2))
                            {
                                if ((CustoRota(copia1) + CustoRota(copia2)) < (CustoRota(rota1) + CustoRota(rota2)))
                                {
                                    rota1 = copia1;
                                    rota2 = copia2;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return CustoTotal(rotas);
}

int VNDReinsertion(vector<vector<int>> &rotas)
{
    for (int id_r1 = 0; id_r1 < rotas.size(); id_r1++)
    {
        for (int id_e1 = 1; id_e1 < (rotas[id_r1].size() - 1); id_e1++)
        {
            for (int id_r2 = 0; id_r2 < rotas.size(); id_r2++)
            {
                for (int id_e2 = 1; id_e2 < (rotas[id_r2].size() - 1); id_e2++)
                {
                    // Evita reinserção inútil
                    if (id_r1 == id_r2 && (id_e1 == id_e2 || id_e1 == (id_e2 - 1)))
                        continue;

                    int custo_remocao = 0;
                    custo_remocao -= p.matriz_custo[rotas[id_r1][id_e1 - 1]][rotas[id_r1][id_e1]];
                    custo_remocao -= p.matriz_custo[rotas[id_r1][id_e1]][rotas[id_r1][id_e1 + 1]];
                    custo_remocao += p.matriz_custo[rotas[id_r1][id_e1 - 1]][rotas[id_r1][id_e1 + 1]];

                    int custo_insercao = 0;
                    custo_insercao += p.matriz_custo[rotas[id_r2][id_e2 - 1]][rotas[id_r1][id_e1]];
                    custo_insercao += p.matriz_custo[rotas[id_r1][id_e1]][rotas[id_r2][id_e2]];
                    custo_insercao -= p.matriz_custo[rotas[id_r2][id_e2 - 1]][rotas[id_r2][id_e2]];

                    if ((custo_remocao + custo_insercao) < 0)
                    {
                        // Caso intra-rota
                        if (id_r1 == id_r2 && ReinsertionTest(rotas[id_r1], id_e1, id_e2))
                        {
                            int estacao = rotas[id_r1][id_e1];
                            rotas[id_r1].erase(rotas[id_r1].begin() + id_e1);

                            // Inserção segura
                            if (id_e1 < id_e2)
                                rotas[id_r1].insert(rotas[id_r1].begin() + (id_e2 - 1), estacao);
                            else
                                rotas[id_r1].insert(rotas[id_r1].begin() + id_e2, estacao);
                        }
                        // Caso inter-rotas
                        else if (RemovalTest(rotas[id_r1], id_e1) &&
                                 InsertionTest(rotas[id_r2], id_e2, rotas[id_r1][id_e1]))
                        {
                            int estacao = rotas[id_r1][id_e1];
                            rotas[id_r1].erase(rotas[id_r1].begin() + id_e1);

                            rotas[id_r2].insert(rotas[id_r2].begin() + id_e2, estacao);
                        }
                    }
                }
            }
        }
    }

    // Remove rotas "mortas" (só depot)
    for (int i = rotas.size() - 1; i >= 0; i--)
    {
        if (rotas[i].size() < 3)
            rotas.erase(rotas.begin() + i);
    }

    return CustoTotal(rotas);
}

int VNDInvertion(vector<vector<int>> &rotas)
{
    for(int id_r = 0; id_r < rotas.size(); id_r++){
        for(int id_e1 = 1; id_e1 < rotas[id_r].size() - 2; id_e1++){
            for(int id_e2 = id_e1 + 2; id_e2 < (rotas[id_r].size()-1) && id_e2 <= (id_e1 + 15); id_e2++){
                int tentativa_inversao = TentaInverter(rotas[id_r], id_e1, id_e2);
                if(tentativa_inversao != -1){
                    int custo_rota = CustoRota(rotas[id_r]);
                    if(tentativa_inversao < custo_rota){
                        reverse(rotas[id_r].begin() + id_e1, rotas[id_r].begin() + id_e2 + 1);
                        break;
                    }
                }
            }
        }
    }

    return CustoTotal(rotas);
}

int VNDBlockSwap(vector<vector<int>>& rotas, int tamanho_bloco = 4) {
    int n_rotas = rotas.size();

    for(int r1 = 0; r1 < n_rotas; r1++) {
        for(int r2 = r1 + 1; r2 < n_rotas; r2++) {
            vector<int>& rota_orig1 = rotas[r1];
            vector<int>& rota_orig2 = rotas[r2];

            int max_start1 = rota_orig1.size() - tamanho_bloco - 1;
            int max_start2 = rota_orig2.size() - tamanho_bloco - 1;

            int c1 = CustoRota(rota_orig1);
            int c2 = CustoRota(rota_orig2);

            if(max_start1 < 1 || max_start2 < 1) continue; // bloco muito grande

            for(int e1 = 1; e1 <= max_start1; e1++) {
                for(int e2 = 1; e2 <= max_start2; e2++) {
                    // cria cópias temporárias
                    vector<int> rota1 = rota_orig1;
                    vector<int> rota2 = rota_orig2;

                    // faz o swap do bloco
                    for(int aux = 0; aux < tamanho_bloco; aux++){
                        swap(rota1[e1+aux], rota2[e2+aux]);
                    }

                    // verifica se é válido e melhora custo
                    if(VerificaDemanda(rota1) && VerificaDemanda(rota2)) {
                        int cr1 = CustoRota(rota1);
                        int cr2 = CustoRota(rota2);

                        if(cr1 + cr2 < c1 + c2){

                            // aplica o swap nas rotas originais
                            for(int aux = 0; aux < tamanho_bloco; aux++){
                                rota_orig1[e1+aux] = rota1[e1+aux];
                                rota_orig2[e2+aux] = rota2[e2+aux];
                            }
                            
                            c1 = cr1;
                            c2 = cr2;
                        }
                    }
                }
            }
        }
    }

    return CustoTotal(rotas);
}

/**
 * @brief Algoritmo VND. Aplica as funções VNDSwap, VNDTwoOpt e VNDReinsertion em ordem, se todas falharem em diminuir o custo, termina
 * ---
 * @param rotas Vetor de vetor de inteiros. Rotas de uma dada solução
 * ---
 */
int VND2(vector<vector<int>> &rotas)
{
    int melhor_custo = CustoTotal(rotas);
    int teste;
    int k = 1;

    while (k < 6)
    {
        // Itera por cada função de acordo com o valor de vizinhança
        switch (k)
        {
        case 1:
            teste = VNDSwap(rotas);
            break;
        case 2:
            teste = VNDReinsertion(rotas);
            break;
        case 3:
            teste = VNDTwoOpt(rotas);
            break;
        case 4: 
            teste = VNDInvertion(rotas);
            break;
        case 5:
            teste = VNDBlockSwap(rotas);
            break;
        }

        // Se de fato melhorou, resetar progressão para o inicio
        if (teste < melhor_custo)
        {
            melhor_custo = teste;
            k = 1;
        }
        else
        {
            k++;
        }
    }

    return melhor_custo;
}

int RVND(vector<vector<int>> &rotas, int custo_inicial = -1)
{
    int melhor_custo = custo_inicial == -1 ? CustoTotal(rotas) : custo_inicial;

    // Conjunto de vizinhanças (identificadores)
    vector<int> vizinhos = {1, 2, 3, 4, 5};

    bool melhorou = true;
    while (melhorou)
    {
        melhorou = false;

        // Reinicia e embaralha a lista de vizinhanças
        vector<int> lista = vizinhos;
        random_shuffle(lista.begin(), lista.end());

        // Enquanto ainda houver vizinhanças não testadas
        while (!lista.empty())
        {
            int k = lista.back();
            lista.pop_back();

            int teste;
            switch (k)
            {
            case 1:
                teste = VNDSwap(rotas);
                break;
            case 2:
                teste = VNDTwoOpt(rotas);
                break;
            case 3:
                teste = VNDReinsertion(rotas);
                break;
            case 4:
                teste = VNDInvertion(rotas);
                break;
            case 5:
                teste = VNDBlockSwap(rotas);
                break;
            }

            if (teste < melhor_custo)
            {
                melhor_custo = teste;
                melhorou = true;
                break;
            }
        }
    }

    return melhor_custo;
}

/**
 * @brief Algoritmo ILS. Provoca várias perturbações em uma dada solução a fim de escapar de ótimos locais
 * ---
 * @param rotas Vetor de vetor de inteiros. Rotas de uma dada solução
 * @param max_iteracoes Máximo de iterações que o ILS deve executar antes de parar de vez.
 * @param max_sem_melhora Máximo de iterações que o ILS tolera sem melhorar o custo encontrado.
 * ---
 */
void ILS(vector<vector<int>> &rotas, int max_iteracoes = 10000, int max_sem_melhora = 10000)
{

    // srand(time(NULL)); deve ser chamado na main
    int iteracoes = 0, sem_melhora = 0;
    int melhor_custo = CustoTotal(rotas), custo_teste;
    vector<vector<int>> rotas_copia;

    // Enquanto não bater o limite de iterações ou de iterações sem melhora ...
    while (iteracoes < max_iteracoes && sem_melhora < max_sem_melhora)
    {
        // Define a perturbação escolhida e o grau de perturbação
        int opcao_perturbacao = 1 + rand() % 6; // 1 a 6
        int nivel_perturbacao = (sem_melhora / (max_sem_melhora / 5)) + 2;
        
        // Perturbar as rotas
        rotas_copia = rotas;
        Perturbar(rotas_copia, opcao_perturbacao, nivel_perturbacao);
        // cout << "Perturbou -- Opcao perturbacao: " << opcao_perturbacao << "\n";

        CorrigeSolucao(rotas_copia);
        custo_teste = RVND(rotas_copia);

        // Se o custo for de fato melhor e as rotas forem todas válidas, atualizar solução
        if (custo_teste < melhor_custo)
        {
            rotas = rotas_copia;
            melhor_custo = custo_teste;
            sem_melhora = 0;

            Solucao sol;
            sol.custo_total = melhor_custo;
            sol.rotas = rotas;
            sol.SalvarSolucao("ils_teste.txt");

            cout << "Melhora! Novo custo: " << melhor_custo << endl;
        }
        else
        {
            sem_melhora++;
        }

        iteracoes++;
    }

    return;
}

#endif