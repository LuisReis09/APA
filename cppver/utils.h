#ifndef UTILS_H
#define UTILS_H
#include "structures.h"
#include <string>

using namespace std;

Problema LerDados(string filePath);

/**
 * Recebe um `Problema p` e o mostra no terminal com um título `p_name`
 */
void PrintProblema(Problema p, string p_name);

/**
 * ## Melhores Vértices
 * ---
 * @param Problema_p
 *
 * Problema que contém atributos necessários para o cálculo, tais como:
 *
 *  - `vector<vector<int>> matriz_custo`;
 *
 *  - `vector<int> demandas`;
 *
 *  - `int capacidade_max`.
 *
 * ---
 * @return
 * - `pair<int, int> vertices`: Vertices que, juntamente com a garagem, formam
 * melhor triângulo inicial para inserção mais barata. Considerando que, para que
 * eles sejam bons, a distância entre os três deve ser a maior possível.
 *
 */
pair<int, int> MelhoresVertices(Problema p);

int VerificaSolucao(vector<vector<int>> matriz, vector<int> necessidades, int cap_max, vector<Rota> rotas);

/**
 * @param problema Usado para verificar as demandas da rota dada;
 * @param rota Define a rota que será avaliada
 *
 * ---
 * @return bool que indica se a rota é possível de acordo com o acúmulo das demandas na rota e a capacidade dos caminhões
 */
bool ValidaDemanda(Problema p, Rota r);

/**
 * @param problema Usado para verificar os custos da rota dada;
 * @param rota Define a rota que será avaliada
 *
 * ---
 * @return `int` que representa a soma dos custos da rota, `-1` se a rota estiver vazia
 */
int SomaCustoRota(Problema p, Rota r);

/**
 * @param problema Usado para verificar o custo total das rotas;
 * @param rotas Define as rotas que serão avaliadas
 *
 * ---
 * @return `int` que representa a soma dos custos das rotas
 */
int SomaCusto(Problema p, Rota rotas[]);

pair<int, int> MelhoresVertices(Problema p);

Problema ProblemaAleatorio(int n, int m, int c, int max);
void PrintProblema(Problema p, string p_name);

/**
 * @param problema Usado para verificar as demandas
 * @param rota Onde será avaliada a inserção de uma demanda
 * @param antecessor estacao que precederá a nova estação na possível nova rota
 * @param novaEstacao índice indicador da nova estação a ser verificada
 *
 * ---
 * @return `bool` que indica se a nova demanda é válida na possição inserida
 */
bool VerificaNovaDemanda(Problema p, Rota r, int antecessor, int novaEstacao);

/**
 * @param problema Usado para verificar os custos
 * @param rota Onde será avaliada a inserção de uma estação
 * @param antecessor estação que precederá a nova estação na nova rota avaliada
 * @param novaEstacao índice indicador da nova estação a ser verificada
 *
 * ---
 * @return `int` que indica a diferença de custo ao inserir a nova estação em determidado ponto
 */
int CustoInsercao(Problema p, Rota r, int antecessor, int novaEstacao);

/*
 * # Problema Aletório:
 * ---
 * ### Parâmetros:
 *   - `int n`:    Quantidade de estações de bicicletas
 *   - `int m`:    Quantidade de caminhões
 *   - `int c`:    Capacidade máxima de cada caminhão
 *   - `int max`:  Custo máximo entre as estações
 * ---
 * ### Retorno:
 *   - `Problema p`: Um  objeto contendo um problema aleatório, repeitando as condições estabelecidas.
 */
Problema ProblemaAleatorio(int n, int m, int c, int max);

// Vulgo BuscaGulosa2 na versão python
Solucao VizinhoMaisProximo(vector<vector<int>> matriz, int capmax_caminhao, vector<int> necessidades, int qtd_caminhoes);

int TestaConjunto(int conjunto)


#endif