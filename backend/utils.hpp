#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include <iostream>
#include <vector>
#include <ctime>
#include <utility>
#include <string>
#include <random>
#include "structures.hpp"

/**
 * Instância de Problema que MelhoresVerticesserá utilizada por todas as funções que envolverem problema,
 * a fim de reduzir a empilhagem e desempilhagem do mesmo parametro múltiplas vezes.
 * Deve ser instânciada por meio da função `SetProblema`, usando como parâmetro um problema ou seus atributos
 */
extern Problema p;

/**
 * Função que inicializa a instância principal de problema
 * @param Problema que será copiado para a instância
 */
void SetProblema(Problema prob)
{
    p = prob;
}

/**
 * Função que inicializa a instância principal de problema
 * @param qnt_veiculos Inteiro positivo. Quantia de veiculos que podem ser usados neste problema
 * @param capacidade_max Inteiro positivo. Capacidade máxima de bicicletas por caminhão
 * @param demandas Vetor de inteiros. Demandas de cada estação.
 * @param qnt_estacoes Inteiro positivo. Quantia de estações neste problema
 * @param veiculos_disponiveis Quantia de veiculos ainda disponiveis
 * @param matriz_custo Matriz de inteiros mostrando o custo de transição de uma estação para outra.
 */
void SetProblema(
    int qnt_veiculos,
    int capacidade_max,
    vector<int> demandas,
    int qnt_estacoes,
    int veiculos_disponiveis,
    vector<vector<int>> matriz_custo)
{
    p.qnt_veiculos = qnt_veiculos;
    p.capacidade_max = capacidade_max;
    p.demandas = demandas;
    p.qnt_estacoes = qnt_estacoes;
    p.veiculos_disponiveis = veiculos_disponiveis;
    p.matriz_custo = matriz_custo;
}

/**
 * @return Problema p: instância de problema que está sendo utilizada em todas as funções do arquivo.
 */
Problema GetProblema()
{
    return p;
}

typedef struct
{
    bool success;      // Indica se a leitura foi bem sucedida
    string message;    // Mensagem de erro ou sucesso
    Problema problema; // Estrutura do problema lido
} RespostaLeitura;

RespostaLeitura LerDadosStr(const string conteudo_arq)
{
    istringstream input(conteudo_arq);

    RespostaLeitura resultado;
    string aux;

    try
    {
        Problema retorno;

        // ---- Leitura básica ----
        if (!(input >> retorno.qnt_estacoes) || retorno.qnt_estacoes <= 0)
            throw runtime_error("Número inválido de estações.");

        if (!(input >> retorno.qnt_veiculos) || retorno.qnt_veiculos <= 0)
            throw runtime_error("Número inválido de veículos.");

        if (!(input >> retorno.capacidade_max) || retorno.capacidade_max <= 0)
            throw runtime_error("Capacidade máxima inválida.");

        retorno.veiculos_disponiveis = retorno.qnt_veiculos;

        getline(input, aux); // consome resto da linha

        // ---- Demandas ----
        retorno.demandas.resize(retorno.qnt_estacoes);
        for (int i = 0; i < retorno.qnt_estacoes; i++)
        {
            if (!(input >> retorno.demandas[i]))
                throw runtime_error("Demandas incompletas no arquivo.");

            if (abs(retorno.demandas[i]) > retorno.capacidade_max)
            {
                throw runtime_error(
                    "Demanda da estação " + to_string(i) +
                    " excede a capacidade máxima de um veículo.");
            }
        }

        getline(input, aux); // consome resto da linha

        // ---- Matriz de custos ----
        retorno.matriz_custo.resize(retorno.qnt_estacoes + 1,
                                    vector<int>(retorno.qnt_estacoes + 1));
        for (int i = 0; i <= retorno.qnt_estacoes; i++)
        {
            for (int j = 0; j <= retorno.qnt_estacoes; j++)
            {
                if (!(input >> retorno.matriz_custo[i][j]))
                {
                    throw runtime_error("Matriz de custo incompleta.");
                }
            }
        }

        // Dados extras?
        int lixo;
        if (input >> lixo)
        {
            throw runtime_error("Dados extras encontrados no fim do arquivo.");
        }

        resultado.success = true;
        resultado.message = "Leitura concluída com sucesso.";
        resultado.problema = move(retorno);
    }
    catch (const exception &e)
    {
        resultado.success = false;
        resultado.message = e.what();
    }

    return resultado;
}

/**
 * Função que printa o problema adquirido
 * @param prob Problema que será printado
 */
void PrintProblema(Problema prob)
{
    cout << "Número de estações: " << prob.qnt_estacoes << endl;
    cout << "Número de veículos: " << prob.qnt_veiculos << endl;
    cout << "Capacidade máxima: " << prob.capacidade_max << endl;
    cout << "Demandas: ";
    for (int i = 0; i < prob.qnt_estacoes; i++)
    {
        cout << prob.demandas[i] << " ";
    }
    cout << endl;
    cout << "Matriz de custos: " << endl;
    for (int i = 0; i <= prob.qnt_estacoes; i++)
    {
        for (int j = 0; j <= prob.qnt_estacoes; j++)
        {
            cout << prob.matriz_custo[i][j] << " ";
        }
        cout << endl;
    }
}

/**
 *  @brief Verifica se a demanda de uma rota está respeitando a capacidade máxima especificada
 *
 *  ---
 *  @param rota será verificada na função
 *
 *  ---
 *  @return `bool` - Indica se a demanda de uma rota é válida, baseando-se no `Problema p`
 * */
bool VerificaDemanda(const vector<int> &rota)
{
    int n_rota = rota.size();
    int max = 0, min = p.demandas[rota[1] - 1];
    int prefix = 0;
    for (int i = 1; i < n_rota - 1; i++)
    {
        prefix += p.demandas[rota[i] - 1];

        if (prefix > max)
            max = prefix;
        else if (prefix < min)
            min = prefix;
    }

    // ajustando o intervalo de checagem
    min = -min;
    max = p.capacidade_max - max;

    if (max < min || max < 0 || min > p.capacidade_max)
    {
        return false;
    }

    // verifica se algum numero de bicicletas inicial é válido.
    for (int i = max; i >= min && i >= 0; i--)
        if (i <= p.capacidade_max)
            return true;

    return false;
}

/**
 *  @brief Calcula o custo de uma dada rota
 *
 *  ---
 *  @param rota que terá seu custo calculado
 *
 *  ---
 *  @return `int` - Custo total calculado
 * */
int CalculaCusto(vector<int> rota)
{
    int custo = 0;
    for (int i = 0; i < rota.size() - 1; i++)
    {
        custo += p.matriz_custo[rota[i]][rota[i + 1]];
    }
    return custo;
}

/**
 *  @brief Verifica se as demandas de `n` rotas estão respeitando a capacidade máxima especificada
 *
 *  ---
 *  @param rotas Array de rotas que serão verificadas
 *
 *  ---
 *  @return `bool` - Indica se as demandas das rotas são válidas, baseando-se no `Problema p`
 * */
bool VerificaDemandas(const vector<vector<int>> &rotas)
{
    /* Para cada rota, aplica a função que veriifca individualmente */
    for (const vector<int> &rota : rotas)
    {
        if (!VerificaDemanda(rota))
        {
            return false;
        }
    }
    return true;
}

/**
 *  @brief Varre os custos, procurando pelo triângulo mais caro, a fim de minimizar esse custo de outras formas depois.
 *
 *  ---
 *  @return `pair<int, int>` - Valores `<i, j>` que formam a rota inicial de maior custo juntamente com a garagem,
 * seguindo o modelo `[0 -> i -> j -> 0]`
 * */
pair<int, int> MelhoresVertices()
{
    int melhor_1, melhor_2;
    int dist1, dist2, maior_dist = 0;

    /* Para cada par de estações */
    for (int i = 1; i <= p.qnt_estacoes; i++)
    {
        for (int j = i + 1; j <= p.qnt_estacoes; j++)
        {

            /* Ignora caso demandas excedam a capacidade especificada */
            if (abs(p.demandas[i - 1] + p.demandas[j - 1]) > p.capacidade_max)
            {
                continue;
            }

            /* Verifica em ambas as direções */
            dist1 = p.matriz_custo[0][i] + p.matriz_custo[i][j] + p.matriz_custo[j][0];
            dist2 = p.matriz_custo[0][j] + p.matriz_custo[j][i] + p.matriz_custo[i][0];

            /* Se a distância 1 for melhor, salva as estações e atualiza a melhor distância */
            if (dist1 > maior_dist)
            {
                melhor_1 = i;
                melhor_2 = j;
                maior_dist = dist1;
            }
            /* se a distância 2 for melhor, salva as estações de forma invertida e atualiza a melhor distância*/
            if (dist2 > maior_dist)
            {
                melhor_1 = j;
                melhor_2 = i;
                maior_dist = dist2;
            }
        }
    }

    /* retorna o melhor par */
    return make_pair(melhor_1, melhor_2);
}

/**
 *  @brief Verifica se a demanda de uma rota ainda respeitará a capacidade máxima do caminhão caso uma certa estação seja adicionada à rota
 *
 *  ---
 *  @param rota será verificada na função
 *  @param estacao_teste será usada para calcular seu custo de demanda.
 *
 *  ---
 *  @return `bool` - Indica se a demanda de uma rota ainda é válida, baseando-se no `Problema p`
 * */
bool TestaRota(const vector<int> &rota, int estacao_teste)
{
    int carga = p.demandas[estacao_teste - 1];

    for (int i = rota.size() - 1; i > 0; i--)
    {
        carga += p.demandas[rota[i] - 1];
        if (abs(carga) > p.capacidade_max)
            return false;
    }
    return true;
}

/**
 * @brief Checa se, ao adicionar uma certa estação, a demanda continua válida
 * ---
 * @param rota Vetor de inteiros representando a rota
 * @param posicao_destino Posição aonde a nova estação será inserida
 * @param estacao Número identificador da estaço inserida
 *
 * @return Retorna um bool representando se a demanda permanece válida ou não
 */
bool InsertionTest(const vector<int> &rota, int posicao_destino, int estacao)
{
    int prefix = 0;
    int min = p.demandas[rota[1] - 1];
    int max = min;

    for (int i = 1; i < rota.size(); i++)
    {
        if (i == posicao_destino)
        {
            //  acumula e verifica a adicao da demanda da nova estacao
            prefix += p.demandas[estacao - 1];

            if (prefix > max)
                max = prefix;
            else if (prefix < min)
                min = prefix;
        }
        if(rota[i] == 0) continue;
        //  acumula e verifica a adicao da demanda
        prefix += p.demandas[rota[i] - 1];

        if (prefix > max)
            max = prefix;
        else if (prefix < min)
            min = prefix;
    }

    // ajustando o intervalo de checagem
    min = -min;
    max = p.capacidade_max - max;

    if (max < min || max < 0 || min > p.capacidade_max)
    {
        return false;
    }

    // verifica se algum numero de bicicletas inicial é válido.
    for (int i = max; i >= min && i >= 0; i--)
        if (i <= p.capacidade_max)
            return true;

    return false;
}

bool RemovalTest(const vector<int>& rota, int posicao_original){
    int prefix = 0;
    int min = p.demandas[rota[1] - 1];
    int max = min;


    for(int i = 1; i < rota.size(); i++){
        if(i == posicao_original){
            continue;
        }
        //  acumula e verifica a adicao da demanda
        prefix += p.demandas[rota[i] - 1];

        if(rota[i] == 0) continue;

        if(prefix > max)
            max = prefix;
        else if(prefix < min)
            min = prefix;
    }

    // ajustando o intervalo de checagem
    min = -min;
    max = p.capacidade_max - max;

    if(max < min || max < 0 || min > p.capacidade_max){
        return false;
    }

    // verifica se algum numero de bicicletas inicial é válido.
    for(int i = max; i >= min && i >= 0; i--)
        if(i <= p.capacidade_max)
            return true;

    return false;
}

bool ReinsertionTest(const vector<int>& rota, int posicao_original, int posicao_destino){
    int prefix = 0;
    int min = p.demandas[rota[1] - 1];
    int max = min;

    for(int i = 1; i < rota.size(); i++){

        if(i == posicao_original) continue;

        if(i == posicao_destino){
            prefix += p.demandas[rota[posicao_original] - 1];
            if(prefix > max)
                max = prefix;
            else if(prefix < min)
                min = prefix;
        }

        if(rota[i] == 0) continue;

        //  acumula e verifica a adicao da demanda
        prefix += p.demandas[rota[i] - 1];

        if(prefix > max)
            max = prefix;
        else if(prefix < min)
            min = prefix;
    }

    // ajustando o intervalo de checagem
    min = -min;
    max = p.capacidade_max - max;

    if(max < min || max < 0 || min > p.capacidade_max){
        return false;
    }

    // verifica se algum numero de bicicletas inicial é válido.
    for(int i = max; i >= min && i >= 0; i--)
        if(i <= p.capacidade_max)
            return true;

    return false;
}


/**
 * @brief Calcula o custo combinado entre rotas
 * ---
 * @param rotas Vetor de vetor de inteiros com as rotas de uma solução
 *
 * @return `int` - Custo combinado entre todas as rotas
 */
int CustoTotal(vector<vector<int>> &rotas)
{
    int custo_total = 0;

    for (const vector<int> &rota : rotas)
    {
        for (size_t i = 0; i < (rota.size() - 1); i++)
        {
            custo_total += p.matriz_custo[rota[i]][rota[i + 1]];
        }
    }
    return custo_total;
}

int CustoRota(vector<int> &rota){
    int custo = 0;
    for(size_t i=0; i < (rota.size() - 1); i++){
        custo += p.matriz_custo[rota[i]][rota[i+1]];
    }
    return custo;
}

/**
 * @brief Sorteia aleatoriamente entre um intervalo (inclusivo)
 *
 * ---
 * @param max Limite superior do intervalo
 * @param min Limite inferior do intervalo
 *
 * ---
 * @return Retorna um inteiro sorteado entre min e max.
 */
int RandomEntre(int min, int max)
{
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

/**
 *  @brief Verifica as rotas de uma solução com base em regras como:
 * - Posicionamento das garagens nas extremidades;
 * - Não repetição de estações
 * - Demanda acumulada dentro da capacidade de um caminhão em uma rota
 *
 *---
 *  @param rotas Um vector de rotas de uma solução para o Problema p
 *  @param verbose Booleano para log de erros detalhados
 *
 *---
 *  @return `bool` - Indica se as rotas são uma solução válida ou não
 * */
bool VerificaSolucao(const vector<vector<int>> &rotas, bool verbose = false)
{
    vector<bool> visitados(p.qnt_estacoes + 1, false);

    /* Testa cada rota */
    for(int i=0; i < rotas.size(); i++){

        for(int j=0; j < rotas[i].size(); j++){
            if(visitados[rotas[i][j]]){
                if(verbose) cout << "Erro: Estação " << rotas[i][j] << " visitada mais de uma vez." << endl;
                return false;
            }else{
                visitados[rotas[i][j]] = true;
            }
        }


        if(rotas[i].front() != 0 || rotas[i].back() != 0){
            if(verbose) cout << "Erro: Rota " << i+1 << " não começa e termina na garagem." << endl;
            return false;
        }


        if(VerificaDemanda(rotas[i]) == false){
            if(verbose) cout << "Erro: Rota " << i+1 << " não respeita a capacidade máxima." << endl;
            return false;
        }

    }

    return true;
}

#endif