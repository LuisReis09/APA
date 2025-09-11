#ifndef UTILS_H
#define UTILS_H
#include "structures.hpp"
#include <string>
#include <ctime>
#include <utility>
using namespace std;

typedef struct{
    bool success;
    string message;
    Problema problema;
} RespostaLeitura;


/**
 * Instância de Problema que será utilizada por todas as funções que envolverem problema,
 * a fim de reduzir a empilhagem e desempilhagem do mesmo parametro múltiplas vezes.
 * Deve ser instânciada por meio da função `SetProblema`, usando como parâmetro um problema ou seus atributos
 */
extern Problema p;

/**
 * Função que inicializa a instância principal de problema
 * @param Problema que será copiado para a instância
 */
void SetProblema(Problema prob);

void SetProblema(
    int qnt_veiculos,
    int capacidade_max,
    vector<int> demandas,
    int qnt_estacoes,
    int veiculos_disponiveis,
    vector<vector<int>> matriz_custo);

/**
 * @return Problema p: instância de problema que está sendo utilizada em todas as funções do arquivo.
 */
Problema GetProblema();

Problema LerDados(string filePath);
RespostaLeitura LerDadosStr(const string conteudo_arq);

/**
 * Mostra o problema da instância atual no terminal, exigindo um nome para o problema
 */
void PrintProblema(string p_name);

/**
 * ## Melhores Vértices
 * @return
 * - `pair<int, int> vertices`: Vertices que, juntamente com a garagem, formam
 * melhor triângulo inicial para inserção mais barata. Considerando que, para que
 * eles sejam bons, a distância entre os três deve ser a maior possível.
 *
 */
pair<int, int> MelhoresVertices();

int VerificaSolucao(vector<Rota> rotas);

/**
 * @param rota Define a rota que será avaliada
 *
 * ---
 * @return `bool` que indica se a rota é possível de acordo com o acúmulo das demandas na rota e a capacidade dos caminhões
 */
bool ValidaDemanda(Rota r);

/**
 * @param rota Define a rota que será avaliada
 *
 * ---
 * @return `int` que representa a soma dos custos da rota, `-1` se a rota estiver vazia
 */
int SomaCustoRota(Rota r);

/**
 * @param rotas Define as rotas que serão avaliadas
 *
 * ---
 * @return `int` que representa a soma dos custos das rotas
 */
int SomaCusto(vector<Rota> rotas);

pair<int, int> MelhoresVertices();

Problema ProblemaAleatorio(int n, int m, int c, int max);
void PrintProblema(string p_name);

/**
 * @param rota Onde será avaliada a inserção de uma demanda
 * @param antecessor estacao que precederá a nova estação na possível nova rota
 * @param novaEstacao índice indicador da nova estação a ser verificada
 *
 * ---
 * @return `bool` que indica se a nova demanda é válida na possição inserida
 */
bool VerificaNovaDemanda(Rota r, int antecessor, int novaEstacao);

/**
 * @param rota Onde será avaliada a inserção de uma estação
 * @param antecessor estação que precederá a nova estação na nova rota avaliada
 * @param novaEstacao índice indicador da nova estação a ser verificada
 *
 * ---
 * @return `int` que indica a diferença de custo ao inserir a nova estação em determidado ponto
 */
int CustoInsercao(Rota r, int antecessor, int novaEstacao);

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

/**

 *
 * ---
 * @return `
 */
clock_t ComecarCronometro();
float getDuracao(clock_t cronometro_inicio);

#endif