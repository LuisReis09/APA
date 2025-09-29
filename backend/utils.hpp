#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include <iostream>
#include <vector>
#include <ctime>
#include <utility>
#include <string>
#include <random>
#include "structures.hpp"

#define MAXX_INT 2147483647

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

double GAP(int valor_heuristica, int valor_otimo){
    if(valor_otimo == -1){
        return 100.0f;
    }

    double ret = ((double)(valor_heuristica - valor_otimo) / (double) valor_otimo)  * 100;
    
    return ret;
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
    int n_rota = rota.size() - 1;
    int prefix = 0;

    int lower = 0;
    int upper = p.capacidade_max;

    for (int i = 1; i < n_rota; i++) {
        prefix += p.demandas[rota[i] - 1];

        lower = max(lower, -prefix);
        upper = min(upper, p.capacidade_max - prefix);

        if (lower > upper) 
            return false;
    }

    return true;
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
int CalculaCusto(vector<int>& rota)
{
    int custo = 0;
    for (int i = 0; i < rota.size() - 1; i++)
    {
        custo += p.matriz_custo[rota[i]][rota[i + 1]];
    }
    return custo;
}

// Retorna -1 se nao for possivel, ou o custo da rota, se for possivel.
int CustoDemanda(vector<int> &rota){
    int prefix = 0;
    int lower = 0;
    int upper = p.capacidade_max;
    int custo = 0;
    int N = rota.size() - 1;

    for(int i = 1; i < N; i++){
        prefix += p.demandas[rota[i] - 1];
        lower = max(lower, -prefix);
        upper = min(upper, p.capacidade_max - prefix);

        if(lower > upper) return -1;

        custo += p.matriz_custo[rota[i-1]][rota[i]];
    }

    custo += p.matriz_custo[rota[N-1]][rota[N]];
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
    int lower = 0, upper = p.capacidade_max;

    for(int i=1; i < rota.size(); i++){
        if(i == posicao_destino){
            prefix += p.demandas[estacao - 1];
            lower = max(lower, -prefix);
            upper = min(upper, p.capacidade_max - prefix);
            if (lower > upper) return false;
        }
        else if(rota[i] == 0) continue;
        
        prefix += p.demandas[rota[i] - 1];

        lower = max(lower, -prefix);
        upper = min(upper, p.capacidade_max - prefix);
        if(lower > upper) return false;
    }

    return true;
}

bool RemovalTest(const vector<int> &rota, int posicao_original)
{
    int prefix = 0;
    int lower = 0, upper = p.capacidade_max;

    for(int i=1; i < rota.size() - 1; i++){
        if(i == posicao_original) continue;
        prefix += p.demandas[rota[i] - 1];

        lower = max(lower, -prefix);
        upper = min(upper, p.capacidade_max - prefix);
        if(lower > upper) return false;
    }
    return true;
}

bool ReinsertionTest(const vector<int> &rota, int posicao_original, int posicao_destino)
{
    int prefix = 0;
    int lower = 0, upper = p.capacidade_max;

    for(int i=1; i < rota.size(); i++){
        if(i == posicao_original) continue;
        if(i == posicao_destino){
            prefix += p.demandas[rota[posicao_original] - 1];
            lower = max(lower, -prefix);
            upper = min(upper, p.capacidade_max - prefix);
            if (lower > upper) return false;
        }
        else if(rota[i] == 0){
            continue;
        }

        prefix += p.demandas[rota[i] - 1];

        lower = max(lower, -prefix);
        upper = min(upper, p.capacidade_max - prefix);
        if(lower > upper) return false;
    }

    return true;
}

/**
 * @brief Verifica a inserção de um bloco de `tam_seg` elementos de r1 
 *        (a partir do índice `pos_origem`) na posição `pos_destino` da r2.
 * 
 * @return
 * - `-1` se, por causa das demandas, a alteração não for possível.
 * - `custo` total das rotas caso a inserção seja viável.
 */
int VerificaInterNOpt(const vector<int> &r1, const vector<int> &r2, int pos_destino, int pos_origem, int tam_seg) {
    int custo = 0;
    int anterior, atual;

    // --- Verifica remoção do bloco de r1 ---
    int prefix = 0;
    int lower = 0, upper = p.capacidade_max;

    anterior = r1[0];
    for(int i = 1; i < r1.size() - 1; i++){
        if(i == pos_origem){
            i = i + tam_seg - 1;
            continue;
        }

        atual = r1[i];
        prefix += p.demandas[atual - 1];
        lower = max(lower, -prefix);
        upper = min(upper, p.capacidade_max - prefix);
        if(lower > upper) return -1;

        custo += p.matriz_custo[anterior][atual];
        anterior = atual;
    }

    custo += p.matriz_custo[anterior][0];
    prefix = lower = 0;
    upper = p.capacidade_max;

    anterior = r2[0];
    for(int i = 1; i < r2.size() - 1; i++){
        if(i == pos_destino){
            anterior = r2[i-1];
            for(int j = 0; j < tam_seg; j++){
                atual = r1[pos_origem + j];

                prefix += p.demandas[atual - 1];
                lower = max(lower, -prefix);
                upper = min(upper, p.capacidade_max - prefix);
                if(lower > upper) return -1;

                custo += p.matriz_custo[anterior][atual];
                anterior = atual;
            }

            atual = r2[i];
            prefix += p.demandas[atual - 1];
            lower = max(lower, -prefix);
            upper = min(upper, p.capacidade_max - prefix);
            if(lower > upper) return -1;

            custo += p.matriz_custo[anterior][atual];
            anterior = atual;
        }
        else{
            atual = r2[i];
            prefix += p.demandas[atual - 1];
            lower = max(lower, -prefix);
            upper = min(upper, p.capacidade_max - prefix);
            if(lower > upper) return -1;

            custo += p.matriz_custo[anterior][atual];
            anterior = atual;
        }
    }

    custo += p.matriz_custo[anterior][0];
    
    return custo;
}


bool VerificaSwapInterVND(const vector<int>& rota, int posicao, int elemento){
    int prefix = 0;
    int lower = 0, upper = p.capacidade_max;

    for(int i=1; i < rota.size() - 1; i++){
        if(i == posicao)
            prefix += p.demandas[elemento - 1];
        else
            prefix += p.demandas[rota[i] - 1];

        lower = max(lower, -prefix);
        upper = min(upper, p.capacidade_max - prefix);
        if(lower > upper) return false;
    }

    return true;
}

bool VerificaSwapIntraVND(const vector<int>& rota, int posicao1, int posicao2){
    int prefix = 0;
    int lower = 0, upper = p.capacidade_max;

    for(int i=1; i < rota.size() - 1; i++){
        if(i == posicao1){
            prefix += p.demandas[rota[posicao2] - 1];
        }else if(i == posicao2){
            prefix += p.demandas[rota[posicao1] - 1];
        }else{
            prefix += p.demandas[rota[i] - 1];
        }

        lower = max(lower, -prefix);
        upper = min(upper, p.capacidade_max - prefix);
        if(lower > upper) return false;
    }

    return true;
}

/**
 * @brief Verifica se a retirada do sub array rota[posicao_origem, posicao_origem + tam_seg) e sua 
 * seguinte inserção na posição `posicao_destino` da mesma rota é possível
 * 
 * @return
 *  - Caso não seja, retorna `-1`
 * 
 *  - Caso seja, retorna o custo final da rota ao fazer tal operação
 */
int VerificaIntraShiftn(const vector<int> &rota, int posicao_origem, int posicao_destino, int tam_seg) {
    int prefix = 0;
    int lower = 0;
    int upper = p.capacidade_max;
    int custo = 0;
    int anterior = rota[0], atual;

    // Primeira parte: da base ate antes da posicao_origem
    for(int i = 1; i < posicao_origem; i++){
        atual = rota[i];
        prefix += p.demandas[atual - 1];

        lower = max(lower, -prefix);
        upper = min(upper, p.capacidade_max - prefix);
        if(lower > upper) return -1;

        custo += p.matriz_custo[anterior][atual];
        anterior = atual;
    }

    // Segunda parte: da posicao_origem + tam_seg até antes da posicao_destino
    for(int i = posicao_origem + tam_seg; i < posicao_destino; i++){
        atual = rota[i];
        prefix += p.demandas[atual - 1];

        lower = max(lower, -prefix);
        upper = min(upper, p.capacidade_max - prefix);
        if(lower > upper) return -1;

        custo += p.matriz_custo[anterior][atual];
        anterior = atual;
    }

    // Terceira parte: Inserindo o bloco [posicao_origem, posicao_origem + tam_seg)
    for(int i = 0; i < tam_seg; i++){
        atual = rota[posicao_origem + i];
        prefix += p.demandas[atual - 1];

        lower = max(lower, -prefix);
        upper = min(upper, p.capacidade_max - prefix);
        if(lower > upper) return -1;

        custo += p.matriz_custo[anterior][atual];
        anterior = atual;
    }

    // Quarta parte: intervalo [posicao_destino, galpão_final)
    for(int i = posicao_destino; i < (rota.size()-1); i++){
        atual = rota[i];
        prefix += p.demandas[atual - 1];

        lower = max(lower, -prefix);
        upper = min(upper, p.capacidade_max - prefix);
        if(lower > upper) return -1;

        custo += p.matriz_custo[anterior][atual];
        anterior = atual;
    }

    // Quinta e ultima parte: somar custo da ultima estacao ao galpao
    custo += p.matriz_custo[anterior][0];

    return custo;
}

/**
 * @brief Calcula o custo combinado entre rotas
 * ---
 * @param rotas Vetor de vetor de inteiros com as rotas de uma solução
 *
 * @return `int` - Custo combinado entre todas as rotas
 */
int CustoTotal(const vector<vector<int>> &rotas)
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

int CustoRota(vector<int> &rota)
{
    int custo = 0;
    for (int i = 1; i < rota.size(); i++)
    {
        custo += p.matriz_custo[rota[i-1]][rota[i]];
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
    for (int i = 0; i < rotas.size(); i++)
    {

        for (int j = 1; j < rotas[i].size() - 1; j++)
        {
            if (visitados[rotas[i][j]])
            {
                if (verbose)
                    cout << "Erro: Estação " << rotas[i][j] << " visitada mais de uma vez." << endl;
                return false;
            }
            else
            {
                visitados[rotas[i][j]] = true;
            }
        }

        if (rotas[i].front() != 0 || rotas[i].back() != 0)
        {
            if (verbose)
                cout << "Erro: Rota " << i + 1 << " nao começa ou termina na garagem." << endl;
            return false;
        }

        if (VerificaDemanda(rotas[i]) == false)
        {
            if (verbose)
                cout << "Erro: Rota " << i + 1 << " nao respeita a capacidade maxima." << endl;
            return false;
        }
    }

    for(int i=1; i <= p.qnt_estacoes; i++){
        if(!visitados[i]){
            if(verbose) cout << "Erro: Estação " << i << " não visitada." << endl;
            return false;
        }
    }

    return true;
}

typedef struct{
    bool ok;
    int min, max;
    int problema;
} VI_Resposta;

VI_Resposta VerificaIntervalar(const vector<int> &rota, int a_ignorar = -1, int ind_max = -1)
{
    int n_rota = ind_max == -1 ? rota.size() : ind_max;
    int prefix = 0;

    int lower = 0;
    int upper = p.capacidade_max;

    for (int i = 1; i < n_rota; i++) {
        if(rota[i] == 0 || i == a_ignorar) continue;
        prefix += p.demandas[rota[i] - 1];

        lower = max(lower, -prefix);
        upper = min(upper, p.capacidade_max - prefix);

        if (lower > upper) {
            // inviável
            return {
                .ok = false,
                .min = lower,
                .max = upper,
                .problema = i
            };
        }
    }

    // viável
    return {
        .ok = true,
        .min = lower,
        .max = upper,
        .problema = -1
    };
}

void CorrigeSolucao(vector<vector<int>> &rotas)
{
    int menor_custo_remocao, melhor_a_retirar, custo_retirar;
    int custo_inserir, menor_custo_inserir, melhor_r, melhor_p;
    vector<int> fila_realocacao;

    for(int i = 0; i < rotas.size(); i++){
        while(1){
            VI_Resposta res = VerificaIntervalar(rotas[i]);
            if(res.ok) break;

            menor_custo_remocao = MAXX_INT;
            for(int j = 1; j <= res.problema; j++){
                if(VerificaIntervalar(rotas[i], j, res.problema).ok){
                    custo_retirar = p.matriz_custo[rotas[i][j-1]][rotas[i][j+1]] - (p.matriz_custo[rotas[i][j-1]][rotas[i][j]] + p.matriz_custo[rotas[i][j]][rotas[i][j+1]]);
                    if(custo_retirar < menor_custo_remocao){
                        menor_custo_remocao = custo_retirar;
                        melhor_a_retirar = j;
                    }
                }
            }
    
            fila_realocacao.push_back(rotas[i][melhor_a_retirar]);
            rotas[i].erase(rotas[i].begin() + melhor_a_retirar);
        }
    }

    for(int a_realocar: fila_realocacao){
        menor_custo_inserir = MAXX_INT;
        melhor_r = -1;

        for(int i=0; i < rotas.size(); i++){
            for(int j=1; j < rotas[i].size(); j++){
                if(InsertionTest(rotas[i], j, a_realocar)){
                    custo_inserir = p.matriz_custo[rotas[i][j-1]][a_realocar] + p.matriz_custo[a_realocar][rotas[i][j]] - p.matriz_custo[rotas[i][j-1]][rotas[i][j]];
                    if(custo_inserir < menor_custo_inserir){
                        menor_custo_inserir = custo_inserir;
                        melhor_r = i;
                        melhor_p = j;
                    }
                }
            }
        }

        if(melhor_r != -1){
            rotas[melhor_r].insert(rotas[melhor_r].begin() + melhor_p, a_realocar);
        }else{
            rotas.push_back({0, a_realocar, 0});
        }
    }

    // Remove rotas "mortas" (apenas depósitos)
    for (int i = (int)rotas.size() - 1; i >= 0; i--){
        if (rotas[i].size() <= 2)
            rotas.erase(rotas.begin() + i);
    }
}


/**
 * @brief Tenta inverter um trecho de uma rota, retornando o custo se for válido
 * 
 * @param rota Vetor de inteiros representando a rota
 * @param id_e1 Índice do primeiro elemento do trecho a ser invertido
 * @param id_e2 Índice do último elemento do trecho a ser invertido
 * 
 * @return Retorna o custo total da rota com o trecho invertido, ou -1 se a inversão não for válida
 */
int TentaInverter(vector<int> &rota, int id_e1, int id_e2){
    int prefix = 0;
    int custo_total = 0;
    int estacao_anterior = 0, estacao_atual;
    int lower = 0, upper = p.capacidade_max;

    for(int i=1; i < id_e1; i++){
        prefix += p.demandas[rota[i] - 1];
        lower = max(lower, -prefix);
        upper = min(upper, p.capacidade_max - prefix);
        if(lower > upper) return -1;

        estacao_atual = rota[i];
        custo_total += p.matriz_custo[estacao_anterior][estacao_atual];
        estacao_anterior = estacao_atual;
    }

    // agora comeca a inversao
    for(int i=id_e2; i >= id_e1; i--){
        prefix += p.demandas[rota[i] - 1];
        lower = max(lower, -prefix);
        upper = min(upper, p.capacidade_max - prefix);
        if(lower > upper) return -1;

        estacao_atual = rota[i];
        custo_total += p.matriz_custo[estacao_anterior][estacao_atual];
        estacao_anterior = estacao_atual;
    }

    // agora a parte final, fora da inversao
    for(int i=id_e2 + 1; i < (rota.size() - 1); i++){
        prefix += p.demandas[rota[i] - 1];
        lower = max(lower, -prefix);
        upper = min(upper, p.capacidade_max - prefix);
        if(lower > upper) return -1;

        estacao_atual = rota[i];
        custo_total += p.matriz_custo[estacao_anterior][estacao_atual];
        estacao_anterior = estacao_atual;
    }

    custo_total += p.matriz_custo[estacao_anterior][0]; // volta pro deposito
    return custo_total;
}

void TentaMinimizar(vector<vector<int>>& rotas){
    // Para cada estacao das rotas 2 ... n, tenta colocar na rota 1
    int menor_custo, melhor_posicao;
    for(int i=1; i < rotas.size(); i++){
        for(int j=1; j < rotas[i].size() -1; j++){
            melhor_posicao = -1;
            menor_custo = MAXX_INT;
            for(int k=1; k < rotas[0].size(); k++){
                if(InsertionTest(rotas[0], k, rotas[i][j]) && RemovalTest(rotas[i], j)){
                    int custo = p.matriz_custo[rotas[0][k-1]][rotas[i][j]] +
                                p.matriz_custo[rotas[i][j]][rotas[0][k]] -
                                p.matriz_custo[rotas[0][k-1]][rotas[0][k]];
                    if(custo < menor_custo){
                         menor_custo = custo;
                         melhor_posicao = k;
                    }
                }
            }
            if(melhor_posicao != -1){
                rotas[0].insert(rotas[0].begin() + melhor_posicao, rotas[i][j]);
                rotas[i].erase(rotas[i].begin() + j);
                j--;
            }
        }
    }

    for (int i = rotas.size() - 1; i >= 1; i--) {
        if (rotas[i].size() <= 2) {
            rotas.erase(rotas.begin() + i);
        }
    }
}

void RetiraRotasVazias(vector<vector<int>> &rotas){
    for(int i = rotas.size() - 1; i >= 0; i --){
        if(rotas[i].size() <= 2){
            rotas.erase(rotas.begin() + i);
        }
    }
}
#endif