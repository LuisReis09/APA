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
#include "structures.h"
#include "utils.h"
using namespace std;

Problema LerDados(string filePath)
{
    ifstream MyFile(filePath);
    string aux;
    Problema problema_iniciado;

    // Quantia de estações
    getline(MyFile, aux);
    aux.erase(aux.find_last_not_of(" \n\r\t") + 1);
    problema_iniciado.qnt_estacoes = stoi(aux);

    // Quantia de caminhões
    getline(MyFile, aux);
    aux.erase(aux.find_last_not_of(" \n\r\t") + 1);
    problema_iniciado.qnt_veiculos = stoi(aux);
    problema_iniciado.veiculos_disponiveis = problema_iniciado.qnt_veiculos;

    // Capacidade máxima de cargas
    getline(MyFile, aux);
    aux.erase(aux.find_last_not_of(" \n\r\t") + 1);
    problema_iniciado.capacidade_max = stoi(aux);

    // Linha em branco
    getline(MyFile, aux);

    // Array de inteiros: Demandas
    getline(MyFile, aux);
    aux.erase(aux.find_last_not_of(" \n\r\t") + 1);

    istringstream iss(aux);

    int valor;
    while (iss >> valor)
        problema_iniciado.demandas.push_back(valor);

    // // Linha em branco
    // getline(MyFile, aux);
    
    // Falta ler a matriz de custo 

    MyFile.close();

    return problema_iniciado;
}

/**
 * Função auxiliar para objetos Rotas que calculam seu tamanho, uma vez que a rota é similar a uma linked list
*/
int contaTamRota(Rota rota){

    int counter = 0;    // Contador
    No* no_atual = rota.rota_i;   // Ponto de partida
    while(no_atual->proximo != NULL){  // Enquanto haver um nó próximo ao nó atual, incrementar counter
        counter++;
        no_atual = no_atual->proximo;
    }
    
    return counter; // Retorna a quantia de nós naquela rota
}

int VerificaSolucao(vector<vector<int>> matriz, vector<int> necessidades, int cap_max, vector<Rota> rotas)
{
    /*
        Verifica se as rotas passadas são possiveis

    */

    // Inicialização de visitados
    vector<int> visitados;
    for (int i : matriz[0])
        visitados.push_back(0);

    int custo_total = 0;

    for (Rota route : rotas)
    {
        int soma_carga = 0;

        if (route.rota_i->estacao == 0 && route.rota_i->proximo->estacao == 0){

            // Este loop itera por todos os nós de uma rota
            No* no_atual = route.rota_i->proximo;  // Primeiro nó é o nó após a garagem
            while(no_atual->proximo != NULL){
                soma_carga += necessidades[no_atual->estacao];
                
                // Checagem se a carga é possível
                if (abs(soma_carga) > cap_max){
                    return 0;
                }

                // if(no_atual->estacao != 0){
                //     if(visitad)
                // }
                
            }
        }
    }
}

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
pair<int, int> MelhoresVertices(Problema p){
    int qtd_estacoes = p.matriz_custo.size() - 1;
    pair<int, int> melhores_vertices;
    int maior_distancia = 0;

    for(int i = 0; i < qtd_estacoes; i++){
        for(int j = i+1; j < qtd_estacoes; j++){
            if(abs(p.demandas[i] + p.demandas[j]) > p.capacidade_max)
                continue;
            
            // direcao 1: garagem -> i -> j -> galpao
            int distancia1 = p.matriz_custo[0][i+1] + p.matriz_custo[i+1][j+1] + p.matriz_custo[j+1][0];

            // direcao 2: garagem -> j -> i -> galpao
            int distancia2 = p.matriz_custo[0][j+1] + p.matriz_custo[j+1][i+1] + p.matriz_custo[i+1][0];
            
            if(distancia1 > distancia2 && distancia1 > maior_distancia){
                maior_distancia = distancia1;
                melhores_vertices = make_pair(i+1, j+1);
            }else if(distancia2 > maior_distancia){
                maior_distancia = distancia2;
                melhores_vertices = make_pair(j+1, i+1);
            }
        }
    }

    return melhores_vertices;
}

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
Problema ProblemaAleatorio(int n, int m, int c, int max){
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> random_demanda(-c, c);
    uniform_int_distribution<> random_custo(1, max);

    Problema problema;

    problema.qnt_estacoes = n;
    problema.veiculos_disponiveis = m;
    problema.qnt_veiculos = m;
    problema.capacidade_max = c;
    
    problema.demandas.resize(n);
    for(int i = 0; i < n; i++)
        problema.demandas[i] = random_demanda(gen);
    
    
    problema.matriz_custo.resize(n, vector<int>(n));
    for(int i = 0; i < n; i++)
        for(int j = 0; j < n; j++)
            problema.matriz_custo[i][j] = (i!=j) ? random_custo(gen) : 0;
            
    return problema;
}


/**
 * Recebe um `Problema p` e o mostra no terminal com um título `p_name`
 */
void PrintProblema(Problema p, string p_name){
    cout << endl << p_name << endl << endl;
    cout << p.qnt_estacoes << endl;
    cout << p.qnt_veiculos << endl;
    cout << p.capacidade_max << endl << endl;

    for(int demanda: p.demandas)
        cout << demanda << " ";
    cout << endl << endl;

    for(vector<int> custos_linha: p.matriz_custo){
        for(int custo: custos_linha)
            cout << custo << " ";
        cout << endl;
    }
    return;
}