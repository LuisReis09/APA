#ifndef _METODOS_HPP_
#define _METODOS_HPP_

#include <iostream>
#include <stdlib.h>
#include <vector>
#include "structures.hpp"
#include "utils.hpp"
#include "threadpool.hpp"
#include <time.h>
#include <map>
#include <functional>
#include <random>
#include <algorithm>

#define MAXX_INT 2147483647

using namespace std;

/**
 * @brief Utilizando do algoritmo guloso de Vizinho Mais Proximo, procura dentro dos dados do problema estabelecido e cria uma solução
 * ---
 * @return Retorna um objeto Solução com as rotas achadas
 */
Solucao VizinhoMaisProximo()
{
    vector<vector<int>> rotas(p.qnt_veiculos, vector<int>(1, 0)); // inicia todas as rotas com o depósito
    int custo_total = 0;
    int qtd_iteracoes = p.qnt_estacoes;

    /*
        ----- MONTAGEM DOS CUSTOS -----

        Para cada estacao, guarda:
        [
            1: [1: 0, 2: 10, 3: 15 ...] // custos para cada estacao
            2: [1: 5, 2: 0, 3: 20 ...]  // custos para cada estacao
            ...
        ]
    */
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
    while (qtd_iteracoes--)
    {
        int melhor_rota, melhor_custo = MAXX_INT, melhor_estacao;

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
                bool pode_inserir = TestaRota(rotas[i], estacao);
                if (pode_inserir)
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

        // Agora que temos a melhor rota e a melhor estação, inserimos
        rotas[melhor_rota].push_back(melhor_estacao);
        custo_total += melhor_custo;

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
    for (int i = 0; i < rotas.size(); i++)
    {
        if (rotas[i].size() <= 1)
            break;

        custo_total += p.matriz_custo[rotas[i].back()][0];
        rotas[i].push_back(0); // volta para o depósito, rota ficara com formato {0, X, Y, Z, 0}
        solucao.rotas.push_back(rotas[i]);
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
    for (int i = 0; i < p.qnt_veiculos; i++)
        rotas[i].reserve(p.qnt_estacoes / p.qnt_veiculos + 2);

    int caminhoes_usados = 1;
    int custo_total = 0;

    // Para cada estação ...
    for (int i = 1; i < p.qnt_estacoes; i++)
    {
        // Inicialização de variaveis auxiliares
        int menor_custo = MAXX_INT;
        int melhor_rota = -1;
        int melhor_posicao = -1;

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

        // Se houve atualização OU o menor_custo é maior que simplesmente criar uma rota {0, X, 0}
        if (melhor_rota == -1 || menor_custo > (p.matriz_custo[0][i] + p.matriz_custo[i][0]))
        {
            if (caminhoes_usados < p.qnt_veiculos) // Se houver caminhões usaveis ainda, cria rota
            {
                rotas[caminhoes_usados] = {0, i, 0};
                custo_total += p.matriz_custo[0][i] + p.matriz_custo[i][0];
                caminhoes_usados++;
            }
            else // Se não, insere na primeira posição (pós-deposito) da rota achada
            {
                rotas[0].insert(rotas[0].begin() + 1, i);
                custo_total += p.matriz_custo[0][i] + p.matriz_custo[i][rotas[0][2]];
            }
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
 * @brief Para cada combinação de termos das rotas, checa se não é melhor as trocar
 * ---
 * @param rotas Vetor de vetores de inteiros que representam as rotas, com termos sendo as estações
 * @param custo_atual Custo vigente da solução
 * @param iter Quantia de iterações que serão feitas
 */
void MelhorarSolucao(vector<vector<int>> &rotas, int custo_atual, int iter = 1)
{
    if (iter > 500)
        return;

    int est1_anterior, est1, est1_proximo; // Estações da rota de origem
    int est2_anterior, est2, est2_proximo; // Estações da rota de destino

    // Para cada rota "saindo"
    for (int id_rota_origem = 0; id_rota_origem < rotas.size(); id_rota_origem++)
    {
        if (rotas[id_rota_origem].size() <= 2)
            continue; // ignora rotas pequenas

        // Checa termo a termo da rota de origem
        for (int id_estacao_origem = 1; id_estacao_origem < rotas[id_rota_origem].size() - 1; id_estacao_origem++)
        {
            // Define as estações a serem analisadas
            est1_anterior = rotas[id_rota_origem][id_estacao_origem - 1];
            est1 = rotas[id_rota_origem][id_estacao_origem];
            est1_proximo = rotas[id_rota_origem][id_estacao_origem + 1];

            // Para cada rota "recebendo"
            for (int id_rota_destino = 0; id_rota_destino < rotas.size(); id_rota_destino++)
            {
                if (rotas[id_rota_destino].size() <= 2)
                    continue;

                // Checa termo a termo da rota
                for (int id_estacao_destino = 1; id_estacao_destino < rotas[id_rota_destino].size() - 1; id_estacao_destino++)
                {
                    // Se for a mesma rota e estivermos analisando a troca do mesmo termo
                    // Ou de um termo de origem e o seu anterior equivalente na outra rota, pula.
                    if (id_rota_origem == id_rota_destino && (id_estacao_origem == id_estacao_destino || id_estacao_origem == id_estacao_destino - 1))
                        continue;

                    // Define estações de destino analisadas
                    est2_anterior = rotas[id_rota_destino][id_estacao_destino - 1];
                    est2 = rotas[id_rota_destino][id_estacao_destino];
                    est2_proximo = rotas[id_rota_destino][id_estacao_destino + 1];

                    int custo_teste = custo_atual;

                    // Cálculo do custo caso os termos est1 e est2 trocassem de lugar
                    custo_teste += p.matriz_custo[est1_anterior][est2];
                    custo_teste += p.matriz_custo[est2][est1_proximo];
                    custo_teste += p.matriz_custo[est2_anterior][est1];
                    custo_teste += p.matriz_custo[est1][est2_proximo];

                    custo_teste -= p.matriz_custo[est1_anterior][est1];
                    custo_teste -= p.matriz_custo[est1][est1_proximo];
                    custo_teste -= p.matriz_custo[est2_anterior][est2];
                    custo_teste -= p.matriz_custo[est2][est2_proximo];

                    // Se for um custo inferior, trocar
                    if (custo_teste < custo_atual)
                    {
                        swap(rotas[id_rota_origem][id_estacao_origem], rotas[id_rota_destino][id_estacao_destino]);
                        MelhorarSolucao(rotas, custo_teste, iter + 1); // Aplica de novo a função recursivamente
                        return;
                    }
                }
            }
        }
    }
}

/**
 * @brief Para cada combinação de termos em uma rota, as troca e checa se a nova combinação é mais barata
 * ---
 * @param rota Rota a ser analisada e modificada
 *
 * @return Retorna uma nova rota modificada
 */
vector<int> VND_Swap(const vector<int> &rota)
{
    vector<int> rota_ret = rota; // copia a rota antiga
    int melhor_custo = CalculaCusto(rota_ret);
    int custo_swap = 0;

    for (int i = 1; i < rota_ret.size() - 2; i++)
    {
        for (int j = i + 1; j < rota_ret.size() - 1; j++)
        {
            // Para cada combinação de termos em uma dada rota
            vector<int> rota_teste = rota_ret;

            // Os troca
            swap(rota_teste[i], rota_teste[j]);

            // Calcula o custo
            custo_swap = CalculaCusto(rota_teste);

            // Se for menor que o custo atual e for uma rota valida, substitui a rota
            if (custo_swap < melhor_custo && VerificaDemanda(rota_teste))
            {
                rota_ret = rota_teste;
                melhor_custo = custo_swap;
            }
        }
    }
    // Retorna rota modificada
    return rota_ret;
}

/**
 * @brief Algoritmo parte do VND. Para todas as posições em uma rota, checa se mover um termo de posição é benefico ao custo
 *
 * @param rota Rota a ser modificada
 * ---
 * @return Retorna a rota modificada
 */
vector<int> VND_ReInsertion(const vector<int> &rota)
{
    vector<int> rota_ret = rota;
    int melhor_custo = CalculaCusto(rota);
    int custo_reinsertion = 0;

    for (int i = 1; i < rota_ret.size() - 1; i++)
    {
        for (int j = i + 1; j < rota_ret.size() - 1; j++)
        {
            // Para todas as combinações de posição nesta rota
            vector<int> rota_teste = rota_ret;

            // Apaga o termo de sua posição original e a re-insere em outra
            rota_teste.erase(rota_teste.begin() + i);
            rota_teste.insert(rota_teste.begin() + j - 1, rota_ret[i]);

            // Calcula o custo dessa operação
            custo_reinsertion = CalculaCusto(rota_teste);

            // Se for menor que o atual e a rota for válida, substitui a original
            if (custo_reinsertion < melhor_custo && VerificaDemanda(rota_teste))
            {
                rota_ret = rota_teste;
                melhor_custo = custo_reinsertion;
            }
        }
    }
    // Retorna rota modificada
    return rota_ret;
}

/**
 * @brief Algoritmo parte do VND. Para todos os subintervalos da rota, os reverte e vê se é benefico ao custo, se sim, aplica a mudança
 *
 * @param rota Rota a ser modificada
 * ---
 * @return Retorna a rota modificada
 */
vector<int> VND_Inversion(const vector<int> &rota)
{
    vector<int> rota_ret = rota;
    int melhor_custo = CalculaCusto(rota_ret);
    int custo_inversion = 0;

    for (int i = 1; i < rota_ret.size() - 1; i++)
    {
        for (int j = i + 1; j < rota_ret.size() - 1; j++)
        {
            // Para toda sub-fatia da rota...

            vector<int> rota_teste = rota_ret;

            // inverter o intervalo [i, j] na nova rota
            reverse(rota_teste.begin() + i, rota_teste.begin() + j);

            // recalcular o custo da rota_teste
            custo_inversion = CalculaCusto(rota_teste);

            // Se for menor que o atual e a rota for válida, substitui a original
            if (custo_inversion < melhor_custo && VerificaDemanda(rota_teste))
            {
                rota_ret = rota_teste;
                melhor_custo = custo_inversion;
            }
        }
    }

    return rota_ret;
}

/**
 * @brief Algoritmo VND. Aplica as funções VND_Swap, VND_ReInsertion e VND_Inversion em ordem para melhorar custo de uma rota, termina apenas quando as três forem bem sucedidas em melhorar o custo.
 *
 * @param rota Rota a ser modificada e ter seu custo melhorado
 * ---
 */
void VND(vector<int> &rota)
{
    // Custo vigente da rota
    int melhor_custo = CalculaCusto(rota);

    // Funções a serem aplicadas
    vector<function<vector<int>(vector<int>)>> estruturas_VND = {
        VND_Swap,
        VND_ReInsertion,
        VND_Inversion};

    int k = 0; // estrutura swap primeiro
    int N = estruturas_VND.size();
    while (k < N)
    {
        // Aplica a função atual a rota
        vector<int> nova_rota = estruturas_VND[k](rota);
        int novo_custo = CalculaCusto(nova_rota);

        // Se o novo custo for menor que o atual, reseta o contador de funções para melhorar ainda mais a rota
        if (novo_custo < melhor_custo)
        {
            rota = nova_rota;
            melhor_custo = novo_custo;
            k = 0;
        }
        else // Se não for, passa para a proxima função
        {
            k++;
        }
    }

    return;
}

/**
 * @brief Algoritmo VND, mas que retorna o custo melhorado da rota.
 *
 * @param rota Rota a ser modificada
 * ---
 * @return Retorna o custo melhorado da rota
 */
int VNDIntra(vector<int> &rota)
{
    // Custo vigente da rota
    int melhor_custo = CalculaCusto(rota);

    // Funções a serem aplicadas
    vector<function<vector<int>(vector<int>)>> estruturas_VND = {
        VND_Swap,
        VND_ReInsertion,
        VND_Inversion};

    int k = 0; // estrutura swap primeiro
    int N = estruturas_VND.size();
    while (k < N)
    {
        // Aplica a função atual a rota
        vector<int> nova_rota = estruturas_VND[k](rota);
        int novo_custo = CalculaCusto(nova_rota);

        // Se o novo custo for menor que o atual, reseta o contador de funções para melhorar ainda mais a rota
        if (novo_custo < melhor_custo)
        {
            rota = nova_rota;
            melhor_custo = novo_custo;
            k = 0;
        }
        else // Se não for, passa para a proxima função
        {
            k++;
        }
    }

    return melhor_custo;
}

/**
 * @brief Algoritmo VND que aplicará o VND individualmente para cada rota da solução.
 *
 * @param rota Rotas a serem modificadas.
 * ---
 */
void VND(vector<vector<int>> &rotas)
{
    int n_rotas = rotas.size();

    // distribuir as rotas nas threads
    for (int i = 0; i < n_rotas; i++)
    {
        int custo_rota_i = CalculaCusto(rotas[i]);
        VND(rotas[i]);
    }
    return;
}

/**
 * @brief Algoritmo VND Inter-rotas que troca dois termos entre rotas e verifica se o custo é melhor.
 *
 * @param r1 Primeira rota analisada.
 * @param r2 Segunda rota analisada.
 * ---
 * @return Retorna o custo melhorado da rota
 */
int VNDInterSwap(vector<int> &r1, vector<int> &r2)
{
    if (r1.size() < 3 || r2.size() < 3)
        return CalculaCusto(r1) + CalculaCusto(r2);

    int melhor_custo = CalculaCusto(r1) + CalculaCusto(r2);
    int novo_custo;

    // percorre a rota r1 (ignorando a garagem nas pontas)
    for (int i = 1; i < r1.size() - 1; i++)
    {
        // percorre a rota r2 (ignorando a garagem nas pontas)
        for (int j = 1; j < r2.size() - 1; j++)
        {

            vector<int> copy_r1 = r1, copy_r2 = r2;
            swap(copy_r1[i], copy_r2[j]);

            novo_custo = CalculaCusto(copy_r1) + CalculaCusto(copy_r2);

            if (novo_custo < melhor_custo)
            {
                // se o custo for melhor, cria cópias e realiza, de fato, o swap

                // usa essas copias para verificar se as demandas desse swap são válidas
                if (VerificaDemanda(copy_r1) && VerificaDemanda(copy_r2))
                {
                    // atualiza as rotas e o melhor custo para a melhor versão até o momento
                    melhor_custo = novo_custo;
                    r1 = copy_r1;
                    r2 = copy_r2;
                }
            }
        }
    }

    return melhor_custo;
}

/**
 * @brief Algoritmo VND Inter-rotas que pega uma fatia de uma rota r1 e a insere em uma rota r2, então verificando se é benefico ao custo
 *
 * @param r1 Primeira rota analisada.
 * @param r2 Segunda rota analisada.
 * ---
 * @return Retorna o custo melhorado da rota
 */
int VNDInterRelocate(vector<int> &r1, vector<int> &r2)
{
    if (r1.size() < 3 || r2.size() < 3)
        return CalculaCusto(r1) + CalculaCusto(r2);

    // Custo combinado entre rotas
    int melhor_custo = CalculaCusto(r1) + CalculaCusto(r2);
    int novo_custo;

    // percorre a rota r1 (ignorando a garagem nas pontas)
    for (int i = 1; i < r1.size() - 1; i++)
    {
        // percorre a rota r2 (ignorando a garagem nas pontas)
        for (int j = 1; j < r2.size() - 1; j++)
        {
            vector<int> copy_r1 = r1, copy_r2 = r2;

            // Insere a fatia de r1 em r2
            // E apaga esta mesma fatia de r1
            copy_r2.insert(copy_r2.begin() + j, copy_r1[i]);
            copy_r1.erase(copy_r1.begin() + i);

            novo_custo = CalculaCusto(copy_r1) + CalculaCusto(copy_r2);

            // Se o novo custo for menor...
            if (novo_custo < melhor_custo)
            {
                // Checa se ambas rotas são viaveis, se sim, atualiza a rota real
                if (VerificaDemanda(copy_r1) && VerificaDemanda(copy_r2))
                {
                    melhor_custo = novo_custo;
                    r1 = copy_r1;
                    r2 = copy_r2;
                }
            }
        }
    }

    return melhor_custo;
}

/**
 * @brief Algoritmo VND Inter-rotas que troca duas fatias das rotas r1 e r2 as troca entrei, verificando se o custo é menor.
 *
 * @param r1 Primeira rota analisada.
 * @param r2 Segunda rota analisada.
 * ---
 * @return Retorna o custo melhorado da rota
 */
int VNDInterCrossover(vector<int> &r1, vector<int> &r2)
{
    // Ignora rotas muito pequenas
    if (r1.size() < 3 || r2.size() < 3)
        return CalculaCusto(r1) + CalculaCusto(r2);

    // Custo combinado das rotas
    int melhor_custo = CalculaCusto(r1) + CalculaCusto(r2);
    int novo_custo;
    vector<int> copy_r1, copy_r2;
    srand(time(NULL)); // seed para aleatorizar o tamanho do segmento

    int limite = r1.size() > r2.size() ? r2.size() - 3 : r1.size() - 3;

    // para cada segmento, tenta fazer um swap de segmentos:
    for (int i = 1; i < r1.size() - 1; i++)
    {
        for (int j = 1; j < r2.size() - 1; j++)
        {

            // calcula tamanho máximo da fatia
            int max_tam = min({limite, (int)r1.size() - i - 1, (int)r2.size() - j - 1});
            if (max_tam < 2)
                break; // não dá pra formar segmento válido a partir daqui
            int tamanho_seg = 2 + rand() % (max_tam - 1);

            copy_r1 = r1;
            copy_r2 = r2;
            // Troca termo a termo das fatias achadas
            for (int k = 0; k < tamanho_seg; k++)
            {
                swap(copy_r1[i + k], copy_r2[j + k]);
            }

            novo_custo = CalculaCusto(copy_r1) + CalculaCusto(copy_r2);

            // Se o custo obtido for menor que o atual e ambas as novas rotas foram válidas, altera
            if (novo_custo < melhor_custo && VerificaDemanda(copy_r1) && VerificaDemanda(copy_r2))
            {
                melhor_custo = novo_custo;
                r1 = copy_r1;
                r2 = copy_r2;
            }
        }
    }

    return melhor_custo;
}

/**
 * @brief Algoritmo VND aplicado entre duas rotas. Aplica as funções VNDInterSwap, VNDInterRelocate e VNDInterCrossover em ordem para melhorar custo de uma rota, termina apenas quando as três forem bem sucedidas em melhorar o custo.
 * Usado em conjunto com VND intra rotas apresentado anteriormente
 * @param r1 Primeira rota a ser modificada
 * @param r2 Segunda rota a ser modificada
 * ---
 *
 * @return Retorna o custo melhorado da solução após o VND entre essas duas rotas
 */
int VNDInter(vector<int> &r1, vector<int> &r2)
{
    // Custo combinado atual das duas rotas
    int melhor_custo = CalculaCusto(r1) + CalculaCusto(r2);
    int novo_custo;

    // Funções a serem aplicadas
    vector<function<int(vector<int> &, vector<int> &)>> estruturasInterRota = {
        VNDInterSwap,
        VNDInterCrossover,
        VNDInterRelocate};

    int k = 0;
    int N = estruturasInterRota.size();
    while (k < N)
    {
        novo_custo = estruturasInterRota[k](r1, r2);
        if (novo_custo < melhor_custo)
        {
            melhor_custo = novo_custo;
            k = 0;
        }
        else
        {
            k++;
        }

        // se em alguma das estruturas uma delas ficar vazia, retorna, pois só haverá uma rota, afinal
        if (r1.size() < 3 || r2.size() < 3)
        {
            return melhor_custo;
        }
    }

    return melhor_custo;
}

/**
 * @brief Algoritmo VND que aplica primeiro o VND Intra-rota e depois o VND Inter-rotas.
 * @param rotas Vetor de vetor de inteiros. Rotas de uma dada solução
 * ---
 */
void VNDIntraInter(vector<vector<int>> &rotas)
{
    int custo_anterior, novo_custo;
    bool melhorou = true;

    while (melhorou) // Enquanto houver melhora, continue
    {
        melhorou = false;

        // VND INTRA-ROTA
        for (int i = 0; i < (int)rotas.size(); i++)
        { // Rota a rota, aplica o VND intrarota nelas
            custo_anterior = CalculaCusto(rotas[i]);
            novo_custo = VNDIntra(rotas[i]); // Esta função já altera as rotas em si

            // Se o custo for melhor, seta que melhora é true
            if (novo_custo < custo_anterior)
            {
                // cout << "Melhora na rota " << i << "! Diferenca: " << novo_custo - custo_anterior << endl;
                melhorou = true;
            }
        }

        // vnd inter-rotas
        // vamos coletar índices a remover e só apagar depois
        for (int i = 0; i < (int)rotas.size() - 1; i++)
        {
            for (int j = i + 1; j < (int)rotas.size(); j++)
            {
                // Para cada combinação de rotas, aplica o VND Inter-rotas
                custo_anterior = CalculaCusto(rotas[i]) + CalculaCusto(rotas[j]);
                novo_custo = VNDInter(rotas[i], rotas[j]); // Esta função já altera as rotas em si

                if (novo_custo < custo_anterior)
                {
                    // cout << "Melhora nas rotas " << i << " e " << j << " ! Diferenca: " << novo_custo - custo_anterior << endl;
                    melhorou = true;
                }
            }
        }

        // opcional: mostrar custo total aproximado (soma das rotas)
        // cout << "[VND] Custo total: " << CustoTotal(rotas) << " | rotas: " << rotas.size() << endl;
    }
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
    int qtd_rotas = rotas.size();
    int id_rota_origem, id_rota_destino; // index das rotas que terão seus termos trocados entre si
    int pos1, pos2;                      // Posição dos termos que serão trocados na rota de origem e de destino, respectivamente

    id_rota_origem = id_rota_destino = 0;
    while (trocar_a_realizar--)
    {
        if (qtd_rotas > 1)
        {
            // Sorteia duas rotas aleatorias
            id_rota_origem = rand() % qtd_rotas;
            id_rota_destino = rand() % qtd_rotas;
        }

        // O range deve ser [1, rota.size() - 1], para nao incluir o galpao do inicio e do fim
        // Sorteia duas posições nas rotas de origem e destino
        pos1 = rand() % (rotas[id_rota_origem].size() - 2);
        pos2 = rand() % (rotas[id_rota_destino].size() - 2);

        // Para evitar de ser índice 0, onde esta o galpao
        pos1++;
        pos2++;

        // troca os termos de lugar
        swap(rotas[id_rota_origem][pos1], rotas[id_rota_destino][pos2]);
    }
}

/**
 * @brief Algoritmo parte do ILS. Pega uma fatia aleatória de uma rota aleatória e a permuta.
 * ---
 * @param rotas Vetor de vetor de inteiros. Rotas de uma dada solução
 * @param qtd_scrambles Inteiro representando quantas trocas devem ser feitas. Determinada pelo grau de perturbação.
 * ---
 */
void PerturbacaoScramble(vector<vector<int>> &rotas, int qtd_scrambles)
{
    while (qtd_scrambles > 0)
    {

        // Escolhendo uma rota aleatoria
        int id_rota_escolhida;
        do
        {
            id_rota_escolhida = rand() % rotas.size();
        } while (rotas[id_rota_escolhida].size() <= 3);

        // Selecionando qual fatia daquela rota será permutada
        int index_inicio_fatia = 0;
        int index_fim_fatia = 0;

        while (index_inicio_fatia == index_fim_fatia)
        {
            index_inicio_fatia = 1 + rand() % (rotas[id_rota_escolhida].size() - 2); // 1 .. n-2
            index_fim_fatia = 1 + rand() % (rotas[id_rota_escolhida].size() - 2);    // 1 .. n-2
        }

        // Garantindo que os index_inicio é menor
        if (index_inicio_fatia > index_fim_fatia)
            swap(index_inicio_fatia, index_fim_fatia);

        // Scrambling
        // Gerador de numeros aleatorios
        std::random_device rd;
        std::mt19937 g(rd());

        shuffle(rotas[id_rota_escolhida].begin() + index_inicio_fatia,
                rotas[id_rota_escolhida].begin() + index_fim_fatia, g);

        qtd_scrambles--;
    }
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
    vector<int> reversed_routes;
    if (qtd_reverses > rotas.size()) // Se o termo for maior que o numero de rotas, alguma delas vai ser revertida e posta de novo pro normal
        return;                      // Portanto, evitar essa redundância

    while (qtd_reverses > 0)
    {

        int random_route_index = rand() % rotas.size();

        // Verificação se o termo já está no vetor de rotas revertidas.
        if (find(reversed_routes.begin(), reversed_routes.end(), random_route_index) != reversed_routes.end())
        {
            continue;
        }

        while (rotas[random_route_index].size() <= 3)
        { // Rota só tem 1 termo efetivo, sortear dnv
            random_route_index = rand() % rotas.size();
        }

        // Reverter rota
        reverse(rotas[random_route_index].begin(), rotas[random_route_index].end());
        reversed_routes.push_back(random_route_index);

        qtd_reverses--;
    }
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
    // Tal perturbacao nao eh possivel se ja estivermos usando todos os veiculos
    if (p.qnt_veiculos <= rotas.size())
        return;

    vector<int> elementos_retirados(qtd_elementos + 2);
    elementos_retirados[0] = 0;                 // depósito inicial
    elementos_retirados[qtd_elementos + 1] = 0; // depósito final
    int aux = 1;

    while (qtd_elementos--)
    {
        // Sorteia uma rota de origem
        int id_rota_origem = rand() % rotas.size();
        if (rotas[id_rota_origem].size() < 3)
        {
            qtd_elementos++; // tenta de novo
            continue;
        }
        // Index máximo a ser sorteado (evita depositos)
        int id_max = rotas[id_rota_origem].size() - 2;

        if (id_max < 1)
            id_max = 1;

        // Sorteio de indice
        int indice_retirada = RandomEntre(1, id_max);
        // Adiciona os termos sorteados ao vetor de elementos
        elementos_retirados[aux++] = rotas[id_rota_origem][indice_retirada];
        // Retira da rota de origem
        rotas[id_rota_origem].erase(rotas[id_rota_origem].begin() + indice_retirada);

        // Se a rota ficar sem elementos, remove ela
        if (rotas[id_rota_origem].size() < 3)
        {
            rotas.erase(rotas.begin() + id_rota_origem);
        }
    }

    // Cria a nova rota
    rotas.push_back(elementos_retirados);
}

/**
 * @brief Perturbação que troca duas metades de uma rota sorteada.
 * ---
 * @param rotas Array de rotas da solução.
 * @param reverso Booleano indicando se reversão da rota deve ser completa
 */
void PerturbacaoHalfSwap(vector<vector<int>> &rotas, bool reverso = false)
{
    int rota_alvo;

    // Sorteia uma rota_alvo válida
    do
    {
        rota_alvo = rand() % rotas.size();
    } while (rotas[rota_alvo].size() <= 3); // garante que a rota tenha ao menos 2 elementos para trocar

    int metade = 1 + (rotas[rota_alvo].size() - 2) / 2;

    if (!reverso)
    { // Troca as duas metades da rota sorteada, mantendo a ordem da segunda metade
        // Exemplo: Antes: {0, 1, 2, 3, 4, 0} ---> Depois: {0, 3, 4, 1, 2, 0}
        int i = 1, j = metade;

        while (i < metade)
        {
            swap(rotas[rota_alvo][i], rotas[rota_alvo][j]);
            i++;
            j++;
        }
    }
    else
    { // Se a opção de reverso foi habilitada, troca as duas metades da rota sorteada, NÃO mantend a ordem da 2a fatia
        // Exemplo: Antes: {0, 1, 2, 3, 4, 0} ---> Depois: {0, 4, 3, 2, 1, 0}

        // Primeiro e ultimo termo, respectivamente
        int i = 1, j = rotas[rota_alvo].size() - 2;

        while (i < j)
        {
            swap(rotas[rota_alvo][i], rotas[rota_alvo][j]);
            i++;
            j--;
        }
    }
}

/**
 * @brief Perturbação que troca duas metades de duas rotas sorteadas.
 * ---
 * @param rotas Array de rotas da solução.
 */
void PerturbacaoHalfSwapRoutes(vector<vector<int>> &rotas)
{
    if (rotas.size() < 2)
        return; // precisa de ao menos 2 rotas

    // Sorteia duas rotas aleatorias (e de tamanho efetivo maior que 3)
    int rota1, rota2;
    do
    {
        rota1 = rand() % rotas.size();
        rota2 = rand() % rotas.size();
    } while (rota1 == rota2 || rotas[rota1].size() <= 3 || rotas[rota2].size() <= 3);

    // Index da metade da primeira e segunda rota
    int metade1 = 1 + (rotas[rota1].size() - 2) / 2;
    int metade2 = 1 + (rotas[rota2].size() - 2) / 2;

    int i = 1, j = 1;

    // Troca as primeiras metades de cada rota entre si
    while (i < metade1 && j < metade2)
    {
        swap(rotas[rota1][i], rotas[rota2][j]);
        i++;
        j++;
    }
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
        PerturbacaoSwitch(rotas, 1 < nivel_perturbacao);
        break;
    case 2:
        PerturbacaoNewRoute(rotas, 2 * nivel_perturbacao);
        break;
    case 3:
        PerturbacaoHalfSwap(rotas);
        break;
    case 4:
        PerturbacaoHalfSwap(rotas, true);
        break;
    case 5:
        PerturbacaoHalfSwapRoutes(rotas);
        break;
    case 6:
        PerturbacaoScramble(rotas, 2 * nivel_perturbacao);
        break;
    case 7:
        PerturbacaoReverse(rotas, nivel_perturbacao);
        break;
    default:
        break;
    }
}

/**
 * @brief Algoritmo ILS. Provoca várias perturbações em uma dada solução a fim de escapar de ótimos locais
 * ---
 * @param rotas Vetor de vetor de inteiros. Rotas de uma dada solução
 * @param max_iteracoes Máximo de iterações que o ILS deve executar antes de parar de vez.
 * @param max_sem_melhora Máximo de iterações que o ILS tolera sem melhorar o custo encontrado.
 * ---
 */
void ILS(vector<vector<int>> &rotas, int max_iteracoes = 10000, int max_sem_melhora = 1000)
{
    srand(time(NULL));
    int iteracoes = 0, sem_melhora = 0;
    int melhor_custo = CustoTotal(rotas), custo_teste;

    // Enquanto não bater o limite de iterações ou de iterações sem melhora ...
    while (iteracoes < max_iteracoes && sem_melhora < max_sem_melhora)
    {
        // Define a perturbação escolhida e o grau de perturbação
        int opcao_perturbacao = 1 + rand() % 7;
        int nivel_perturbacao = (sem_melhora / (max_sem_melhora / 6)) + 2;

        // Perturbar as rotas
        vector<vector<int>> rotas_copia = rotas;
        Perturbar(rotas_copia, opcao_perturbacao, nivel_perturbacao);

        // A partir dessas rotas modificadas, aplica MelhorarSolucao e VND
        MelhorarSolucao(rotas_copia, CustoTotal(rotas_copia));
        VNDIntraInter(rotas_copia);

        custo_teste = CustoTotal(rotas_copia);

        // Se o custo for de fato melhor e as rotas forem todas válidas, atualizar solução
        if (custo_teste < melhor_custo && VerificaDemandas(rotas_copia))
        {
            rotas = move(rotas_copia);
            melhor_custo = custo_teste;
            Solucao sol = Solucao();
            sol.rotas = rotas;
            sol.custo_total = melhor_custo;
            sol.SalvarSolucao("ILS_melhorou.sol");
            sem_melhora = 0;
            cout << "Melhora! Novo custo: " << melhor_custo << endl;
        }
        else
        {
            sem_melhora++;
        }

        iteracoes++;
    }
}

// Struct auxiliar para uso das threads que carrega os valores dos parametros
struct ThreadData
{
    int thread_id;
    vector<vector<int>> &rotas;
    int max_iteracoes;
    int max_sem_melhora;
    int &melhor_custo;
    ThreadPool &pool;

    ThreadData(int id,
               vector<vector<int>> &r,
               int maxIt,
               int maxNoImprove,
               int &custo,
               ThreadPool &p)
        : thread_id(id),
          rotas(r),
          max_iteracoes(maxIt),
          max_sem_melhora(maxNoImprove),
          melhor_custo(custo),
          pool(p) {}
};

/**
 * @brief Algoritmo ILS em thread individual, parametros a seguir devem estar dentro de um struct ThreadData
 * ---
 * @param rotas Vetor de vetor de inteiros. Rotas de uma dada solução
 * @param max_iteracoes Máximo de iterações que o ILS deve executar antes de parar de vez.
 * @param max_sem_melhora Máximo de iterações que o ILS tolera sem melhorar o custo encontrado.
 * @param melhor_custo Apontador ou endereço para inteiro do melhor custo
 * @param thread_id Identificador da thread executante
 * @param pool Endereço para o pool de threads envolvendo essa thread
 * ---
 */
void ThreadILS(void *data)
{
    ThreadData *threadData = (ThreadData *)data;
    srand(time(NULL) + threadData->thread_id); // seed diferente para cada thread
    cout << "[Thread " << threadData->thread_id << "] Iniciando ILS..." << endl;

    int iteracoes = 0, sem_melhora = 0, custo_teste;

    while (iteracoes < threadData->max_iteracoes && sem_melhora < threadData->max_sem_melhora)
    {
        int opcao_perturbacao = 1 + rand() % 7;
        int nivel_perturbacao = (sem_melhora / (threadData->max_sem_melhora / 6)) + 2;

        threadData->pool.lock();
        vector<vector<int>> rotas_copia = threadData->rotas;
        threadData->pool.release();

        Perturbar(rotas_copia, opcao_perturbacao, nivel_perturbacao);
        cout << "perturbou" << endl;

        MelhorarSolucao(rotas_copia, CustoTotal(rotas_copia));
        cout << "melhorou" << endl;

        VNDIntraInter(rotas_copia);
        cout << "vnd intra inter" << endl;

        int custo_teste = CustoTotal(rotas_copia);

        threadData->pool.lock();
        if (custo_teste < threadData->melhor_custo && VerificaDemandas(rotas_copia))
        {
            threadData->rotas = move(rotas_copia);
            threadData->melhor_custo = custo_teste;
            sem_melhora = 0;
            cout << "[Thread " << threadData->thread_id << "] Melhora! Novo custo: " << threadData->melhor_custo << endl;
        }
        else
        {
            sem_melhora++;
        }
        threadData->pool.release();

        iteracoes++;
    }

    cout << "[Thread " << threadData->thread_id << "] Finalizando." << endl;
}

#endif