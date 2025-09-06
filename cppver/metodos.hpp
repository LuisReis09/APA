#ifndef METODOS_HPP
#define METODOS_HPP
#include "structures.hpp"
#include "utils.hpp"

/**
 * 
 * 
 * 
 * 
 */
Solucao VizinhoMaisProximo(); // vulgo BuscaGulosa2, no utils.py

/**
 * @brief Tenta reposicionar as estações entre as rotas para uma possível redução
 * de custo total.
 * 
 * ---
 * @param rotas que contém as rotas de uma dada solução
 * 
 * ---
 * @return `rotas` - um vetor de rotas igual ao de entrada ou de melhor custo total
 */
vector<Rota> MelhorarSolucao(vector<Rota>);

// Estruturas de vizinhanças a serem aplicadas no algoritmo VND
typedef enum{
    // Trocar duas estações de lugar
    SWAP,               
    // Inverte um trecho da rota
    INVERT_SECTION,
    // Mover uma estação para outra posição na mesma rota
    REINSERTION
}VndOpt;

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

typedef enum{
    // Breve descrição do SWITCH
    SWITCH,
    // Breve descrição do RELOCATE
    RELOCATE,
    // Breve descrição do RELOCATE_REVERSE
    RELOCATE_REVERSE,
    // Breve descrição do NEWROUTE
    NEWROUTE,
    // Breve descrição do INVERSAO
    INVERSAO
}PerturbacaoOpt;

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