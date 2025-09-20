#ifndef _METODOS_HPP_
#define _METODOS_HPP_

#include <iostream>
#include <stdlib.h>
#include <vector>
#include "structures.hpp"
#include "utils.hpp"
// #include "threadpool.hpp"
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

                    // Define estações de destino analisadas
                    est2_anterior = rotas[id_rota_destino][id_estacao_destino - 1];
                    est2 = rotas[id_rota_destino][id_estacao_destino];
                    est2_proximo = rotas[id_rota_destino][id_estacao_destino + 1];

                    if (est1 == est2)
                        continue;

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

vector<int> VND_Shift_N(const vector<int> &rota)
{
    vector<int> rota_ret = rota;
    int melhor_custo = CalculaCusto(rota);

    int nMax = rota.size() - 2;
    if (nMax > 4)
        nMax = 4; // limite no tamanho do bloco

    for (int i = 1; i < rota_ret.size() - 1; i++)
    {
        for (int j = i + 1; j < rota_ret.size() - 1; j++)
        {
            int max_seg = min(nMax, (int)rota_ret.size() - 2 - i);
            if (max_seg < 1)
                continue;

            int segmento = 1 + rand() % max_seg;

            vector<int> bloco(rota_ret.begin() + i, rota_ret.begin() + i + segmento);

            vector<int> rota_teste = rota_ret;
            rota_teste.erase(rota_teste.begin() + i, rota_teste.begin() + i + segmento);

            int pos = j - segmento;
            if (pos < 1)
                pos = 1;
            if (pos > (int)rota_teste.size() - 1)
                pos = rota_teste.size() - 1;

            rota_teste.insert(rota_teste.begin() + pos, bloco.begin(), bloco.end());

            int custo_shift = CalculaCusto(rota_teste);
            if (custo_shift < melhor_custo && VerificaDemanda(rota_teste))
            {
                rota_ret = rota_teste;
                melhor_custo = custo_shift;
            }
        }
    }
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
        VND_Shift_N,
        VND_Swap,
        VND_Inversion,
        VND_ReInsertion,
    };

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
        VND_Shift_N,
        VND_ReInsertion,
        VND_Inversion,
    };

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
    // Ignora rotas vazias
    if (r1.size() < 3 || r2.size() < 3)
        return CalculaCusto(r1) + CalculaCusto(r2);

    // Custo combinado das rotas
    int melhor_custo = CalculaCusto(r1) + CalculaCusto(r2);
    int novo_custo;
    vector<int> copy_r1, copy_r2;
    srand(time(NULL)); // seed para aleatorizar o tamanho do segmento

    int limite = r1.size() > r2.size() ? r2.size() - 3 : r1.size() - 3;

    // para cada rota, tenta fazer um swap de segmentos:
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
 * @brief Algoritmo VND aplicado entre duas rotas. Re-insere uma fatia de 1 a n elementos de r1 em r2
 *
 * ---
 * @param r1 Primeira rota a ser modificada
 * @param r2 Segunda rota a ser modificada
 * ---
 *
 * @return Retorna o custo melhorado da solução após o VND entre essas duas rotas
 */
int VNDInterNOpt1(vector<int> &r1, vector<int> &r2)
{
    // cout << "[VNDInterNOpt1]: " << endl;

    // se alguma rota não tiver pelo menos duas estacoes, pula
    if (r1.size() < 4 || r2.size() < 4)
    {
        return CalculaCusto(r1) + CalculaCusto(r2);
    }

    // cout << "Tamanho de r1: " << r1.size() << endl;
    // cout << "Tamanho de r2: " << r2.size() << endl;

    // Custo combinado das rotas
    int melhor_custo = CalculaCusto(r1) + CalculaCusto(r2);
    int novo_custo;
    vector<int> copy_r1, copy_r2;

    int max_seg = r1.size() - 3; // no máximo até antes do depósito
    if (max_seg < 4)
        return melhor_custo; // não tem segmento válido

    int tamanho_seg = 2 + rand() % (max_seg - 3);

    // Para toda combinação de segmentos entre r1 e r2
    for (int i = 1; i + tamanho_seg < r1.size() - 1; i++)
    {
        for (int j = 1; j < r2.size() - 1; j++)
        {
            copy_r1 = r1;
            copy_r2 = r2;

            if (i + tamanho_seg >= copy_r1.size() - 1)
            {
                continue;
            }

            // cout << "i = " << i << ", j = " << j << ", tam_seg = " << tamanho_seg << endl;
            copy_r2.insert(copy_r2.begin() + j, copy_r1.begin() + i, copy_r1.begin() + i + tamanho_seg);
            copy_r1.erase(copy_r1.begin() + i, copy_r1.begin() + i + tamanho_seg);

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

    // cout << "OK" << endl;
    return melhor_custo;
}

/**
 * @brief Algoritmo parte do VND. Cria uma perturbação ao trocar duas duplas entre rotas.
 *
 * ---
 * @param r1 Primeira rota a ser modificada
 * @param r2 Segunda rota a ser modificada
 * ---
 *
 * @return Retorna o custo melhorado da solução após o VND entre essas duas rotas
 */
int VNDInterSwap2x2(vector<int> &r1, vector<int> &r2)
{
    // cout << "swap2x2: " << endl;
    if (r1.size() < 3 || r2.size() < 3)
    {
        return CalculaCusto(r1) + CalculaCusto(r2);
    }

    // cout << "Tamanho de r1: " << r1.size() << endl;
    // cout << "Tamanho de r2: " << r2.size() << endl;

    // Custo combinado das rotas
    int melhor_custo = CalculaCusto(r1) + CalculaCusto(r2);
    int novo_custo;
    vector<int> copy_r1, copy_r2;

    // para toda combinação de termos de r1 e r2...
    for (int i = 1; i < r1.size() - 2; i++)
    {
        for (int j = 1; j < r2.size() - 2; j++)
        {
            copy_r1 = r1;
            copy_r2 = r2;

            // cout << "i = " << i << ", j = " << j << endl;
            // Troca duas duplas entre as duas rotas
            swap(copy_r1[i], copy_r2[j]);
            swap(copy_r1[i + 1], copy_r2[j + 1]);

            // Calcula o novo custo
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
    // cout << "OK" << endl;
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
        VNDInterRelocate,
        VNDInterSwap2x2,
        VNDInterCrossover,
        VNDInterNOpt1,
    };

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
 * @brief Algoritmo parte do ILS. Reverte rotas inteiras.
 * ---
 * @param rotas Vetor de vetor de inteiros. Rotas de uma dada solução
 * @param qtd_reverses Quantia de quantas rotas serão revertidas.
 * ---
 */
void PerturbacaoReverse(vector<vector<int>> &rotas)
{
    int id_a_inverter;

    do{
        id_a_inverter = rand() % rotas.size();
    }while(rotas[id_a_inverter].size() <= 3); // garante que a rota tenha ao menos 2 elementos para inverter

    int aux = 1;
    for(int b = rotas[id_a_inverter].size() - 2; b > aux; b--, aux++){
        swap(rotas[id_a_inverter][aux], rotas[id_a_inverter][b]);
    }
    return;
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

    vector<int> nova_rota = {0}; // Inicia apenas com depósito inicial
    nova_rota.reserve(qtd_elementos + 2);
    
    while (qtd_elementos--)
    {
        int id_rota_origem = rand() % rotas.size();
        if (rotas[id_rota_origem].size() <= 2) continue;
        
        int indice = 1 + rand() % (rotas[id_rota_origem].size() - 2);
        nova_rota.push_back(rotas[id_rota_origem][indice]);
        rotas[id_rota_origem].erase(rotas[id_rota_origem].begin() + indice);
        
        // Remove rota se ficou vazia (apenas [0,0])
        if (rotas[id_rota_origem].size() == 2) {
            rotas.erase(rotas.begin() + id_rota_origem);
        }
    }
    
    nova_rota.push_back(0); // Depósito final
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
void PerturbacaoHalfSwap(vector<vector<int>> &rotas, bool reverso = false)
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

    if (!reverso)
    {
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
    else
    {
        // Reverso: inverte toda a parte interna da rota
        // Exemplo: {0, 1, 2, 3, 4, 0} → {0, 4, 3, 2, 1, 0}
        int i = 1, j = n - 2;
        while (i < j)
        {
            swap(rotas[rota_alvo][i], rotas[rota_alvo][j]);
            i++;
            j--;
        }
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
    rotas[id].erase(rotas[id].begin() + meio, rotas[id].end() - 1);
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
    // case 1:
    //     PerturbacaoNewRoute(rotas, 2 * nivel_perturbacao);
    //     break;
    case 2:
        PerturbacaoReverse(rotas);
        break;
    case 3:
        PerturbacaoMergeRoutes(rotas);
        break;
    case 4:
        PerturbacaoSplitRoute(rotas);
        break;
    case 5:
        PerturbacaoHalfSwap(rotas, false);
        break;
    case 6:
        PerturbacaoHalfSwap(rotas, true);
        break;
    case 7:
        PerturbacaoHalfSwapRoutes(rotas);
    default:
        break;
    }
}

bool VerificaSwapVND(const vector<int>& rota, int posicao, int elemento){
    int prefix = 0;
    int min = MAXX_INT, max = 0;

    for(int i=1; i < rota.size() - 1; i++){
        if(i == posicao)
            prefix += p.demandas[elemento - 1];
        else
            prefix += p.demandas[rota[i] - 1];


        if (prefix > max)
            max = prefix;
        if (prefix < min)
            min = prefix;
    }

    min = -min;
    max = p.capacidade_max - max;
    if (max < min || min > p.capacidade_max || max > p.capacidade_max)
        return false;

    for (int i = max; i >= min; i--)
    {
        if (i <= p.capacidade_max)
            return true;
    }

    return false;
}

int VNDSwap(vector<vector<int>> &rotas, int custo_antigo)
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

                    if(custo_teste < melhor_custo && VerificaSwapVND(rotas[id_r1], id_e1, e2) && VerificaSwapVND(rotas[id_r2], id_e2, e1)){
                        melhor_custo = custo_teste;
                        melhor_troca_r = id_r2;
                        melhor_troca_e = id_e2;
                    }
                }
            }

            if (melhor_custo < 0)
            {
                swap(rotas[id_r1][id_e1], rotas[melhor_troca_r][melhor_troca_e]);
            }
        }
    }

    custo_antigo = CustoTotal(rotas);
    return custo_antigo;
}

int VNDTwoOpt(vector<vector<int>> &rotas, int custo_antigo)
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

    custo_antigo = CustoTotal(rotas);
    return custo_antigo;
}

int VNDReinsertion(vector<vector<int>> &rotas, int custo_antigo)
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

    custo_antigo = CustoTotal(rotas);
    return custo_antigo;
}

/**
 * @brief Algoritmo VND. Aplica as funções VNDSwap, VNDTwoOpt e VNDReinsertion em ordem, se todas falharem em diminuir o custo, termina
 * ---
 * @param rotas Vetor de vetor de inteiros. Rotas de uma dada solução
 * ---
 */
void VND2(vector<vector<int>> &rotas, int max_sem_melhora = 25)
{
    int melhor_custo = CustoTotal(rotas);
    int teste;
    // int iter = 200;
    int k = 1;

    int sem_melhora = 0;

    while (sem_melhora < max_sem_melhora)
    {
        k = rand() % 3 + 1; // Randomiza a ordem das funções

        // Itera por cada função de acordo com o valor de vizinhança
        switch (k)
        {
        case 1:
            teste = VNDSwap(rotas, melhor_custo);
            break;
        case 2:
            teste = VNDTwoOpt(rotas, melhor_custo);
            break;
        case 3:
            teste = VNDReinsertion(rotas, melhor_custo);
            break;
        }

        // Se de fato melhorou, resetar progressão para o inicio
        if (teste < melhor_custo)
        {
            melhor_custo = teste;
            sem_melhora = 0;
        }
        else
        {
            sem_melhora++;
            // k++;
        }
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
void ILS(vector<vector<int>> &rotas, int max_iteracoes, int max_sem_melhora)
{

    // srand(time(NULL)); deve ser chamado na main
    int iteracoes = 0, sem_melhora = 0;
    int melhor_custo = CustoTotal(rotas), custo_teste;
    vector<vector<int>> rotas_copia;

    // Enquanto não bater o limite de iterações ou de iterações sem melhora ...
    while (iteracoes < max_iteracoes && sem_melhora < max_sem_melhora)
    {
        // if(iteracoes & 1) cout << "It: " << iteracoes << endl;
        // Define a perturbação escolhida e o grau de perturbação
        int opcao_perturbacao = 1 + rand() % 4; // 1 a 4
        int nivel_perturbacao = (sem_melhora / (max_sem_melhora / 6)) + 2;
        
        // Perturbar as rotas
        rotas_copia = rotas;
        Perturbar(rotas_copia, opcao_perturbacao, nivel_perturbacao);
        // cout << "Perturbou -- Opcao perturbacao: " << opcao_perturbacao << "\n";
        
        // A partir dessas rotas modificadas, aplica VND
        // VNDIntraInter(rotas_copia);

        VND2(rotas_copia);
        // cout << "Passou pelo VND\n";
        custo_teste = CustoTotal(rotas_copia);

        // Se o custo for de fato melhor e as rotas forem todas válidas, atualizar solução
        if (custo_teste < melhor_custo && VerificaDemandas(rotas_copia))
        {
            rotas = rotas_copia;
            melhor_custo = custo_teste;
            sem_melhora = 0;

            // cout << "Melhora! Novo custo: " << melhor_custo << endl;
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