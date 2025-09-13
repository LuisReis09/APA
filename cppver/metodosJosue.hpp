#ifndef METODOS_HPP
#define METODOS_HPP
#include "structures.hpp"
#include "utils.hpp"

const int NUM_VND_STRUCTURES = 3;
/**
 *  @brief
 *  A partir de um triângulo inicial, contendo os 3 pontos mais distantes entre si, sendo um deles obrigatoriamente o galpão:
 *  Pra cada estação ainda não visitada:

 *      - Verificar, em todas as rotas criadas, qual a estação mais proveitosa de se inserir a estação
 *
 *          - Pra isso, precisamos avaliar o menor custo e se a sequência das demandas será possível de ser atendida.
 *          (guardar a melhor configuração em uma variável)

 *      - Se o custo da melhor configuração for negativo, significa que irá reduzir o custo da rota, então pode inserir!

 *      - Se o custo da melhor configuração for positivo, precisamos avaliar se, caso ainda tenham caminhões disponíveis, é melhor criar uma nova rota pra essa estação
 *          (Se compensar, criamos uma nova rota com as arestas [(0, estacao_nova), (estacao_nova, 0)])
 *
 *  ---
 *  @returns
 *  Ao integrarmos todas as estações, finalizamos retornando a solução encontrada pelo algoritmo.
 */
Solucao InsercaoMaisBarata();

/**
 * @brief Executa o algoritmo de Vizinho Mais Proximo e retorna uma solução
 *
 * ---
 * @return Retorna uma objeto Solucao com a rota, custo acumulado, nós, etc.
 */
Solucao VizinhoMaisProximo(); // vulgo BuscaGulosa2, no utils.py

/**
 * @brief Tenta reposicionar as estações entre as rotas para uma possível redução
 * de custo total.
 *
 * ---
 *
 * ---
 * @return `rotas` - um vetor de rotas igual ao de entrada ou de melhor custo total
 */
vector<Rota> MelhorarSolucao();

// Estruturas de vizinhanças a serem aplicadas no algoritmo VND
typedef enum
{
    // Trocar duas estações de lugar
    SWAP,
    // Inverter um trecho da rota
    INVERT_SECTION,
    // Mover uma estação para outra posição na mesma rota
    REINSERTION
} VndOpt;

/**
 * @brief Recebe uma solução obtida através de um algoritmo guloso ou aleatoriamente
 * e, dadas as estruturas de vizinhança descritas, tenta melhorar a solução atual em cada
 * uma das estruturas ou de forma aleatória entre elas.
 *
 * ---
 * @param solucao_atual contendo a solucao que servirá de base para as estruturas de vizinhança
 *
 * ---
 * @return `nova_solucao` - Uma solução que recebeu ou não alterações pelas estruturas de vizinhanças
 */
Solucao VND(Solucao);

typedef enum
{
    // switch de até (2 * grau_perturbacao) estações entre rotas diferentes
    SWITCH,
    // relocate de blocos de (grau_perturbacao) estações entre rotas diferentes
    RELOCATE,
    // relocate de blocos de (grau_perturbacao) estações entre rotas diferentes, invertendo a ordem
    RELOCATE_REVERSE,
    // cria uma nova rota com até (2 * grau_perturbacao) estações retiradas de rotas existentes
    NEWROUTE,
    // inverte uma rota aleatória inteira
    RECORTE
} PerturbacaoOpt;

/**
 * @brief Aplica perturbações para escapar de ótimos locais
 *
 * ---
 * @param rotas rotas nas quais as perturbações serão aplicadas
 * @param PerturbacaoOpt valor que indica qual perturbação deve ser aplicada nas rotas
 * @param grauPerturbacao número de perturbações
 *
 * ---
 * @return `vector<Rota>` que contém novas rotas com as perturbações aplicadas
 */
vector<Rota> Perturbacao(vector<Rota>, PerturbacaoOpt, int);

/**
 * @brief Aplica perturbações aleatórias que fizerem efeito diminuindo o `custo_total`
 *
 * ---
 * @param solucao_atual Uma solução contendo lista de rotas e custo_total
 * @param max_interacoes Quantas vezes, no máximo, as perturbações serão aplicadas nas rotas
 * @param max_sem_melhora Quantas vezes continuar aplicando perturbações mesmo sem melhora
 *
 * ---
 * @return `Solucao` - Uma solução contendo novo custo e uma lista de novas (ou mesmas) rotas, com as perturbações aplicadas, caso efeito de
 *  aplicação fosse positivo
 */
Solucao ILS(Solucao, int, int);

#endif