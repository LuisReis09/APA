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
<<<<<<< HEAD
#include <map>
=======
>>>>>>> 639e0c8c1f8daa6652f49496ee987dd2ba6dce85
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

<<<<<<< HEAD
    // Linha em branco
    getline(MyFile, aux);

    // Leitura da matriz de custo
    int index = 0;
    problema_iniciado.matriz_custo.resize(problema_iniciado.qnt_estacoes + 1);

    while (getline(MyFile, aux))
    {
        aux.erase(aux.find_last_not_of(" \n\r\t") + 1);
        istringstream iss(aux);
        int valor;

        while (iss >> valor)
            problema_iniciado.matriz_custo[index].push_back(valor);
        index++;
    }

    // Fechamento de arquivo
=======
    // // Linha em branco
    // getline(MyFile, aux);
    
    // Falta ler a matriz de custo 

>>>>>>> 639e0c8c1f8daa6652f49496ee987dd2ba6dce85
    MyFile.close();

    return problema_iniciado;
}

<<<<<<< HEAD
=======
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

>>>>>>> 639e0c8c1f8daa6652f49496ee987dd2ba6dce85
int VerificaSolucao(vector<vector<int>> matriz, vector<int> necessidades, int cap_max, vector<Rota> rotas)
{
    /*
        Verifica se as rotas passadas são possiveis
<<<<<<< HEAD
=======

>>>>>>> 639e0c8c1f8daa6652f49496ee987dd2ba6dce85
    */

    // Inicialização de visitados
    vector<int> visitados;
    for (int i : matriz[0])
        visitados.push_back(0);

    int custo_total = 0;

    for (Rota route : rotas)
    {
        int soma_carga = 0;

<<<<<<< HEAD
        if (route.rota_i->estacao == 0 && route.rota_i->proximo->estacao == 0)
        {

            // Este loop itera por todos os nós de uma rota
            No *no_atual = route.rota_i->proximo; // Primeiro nó é o nó após a garagem
            No *no_anterior = route.rota_i;
            while (no_atual->proximo != NULL)
            {
                soma_carga += necessidades[no_atual->estacao];

                // Checagem se a carga é possível
                if (abs(soma_carga) > cap_max)
                    return 0;

                if (no_atual->estacao != 0)
                {
                    if (visitados[no_atual->estacao - 1])
                    {
                        return 0;
                    }
                    else
                    {
                        visitados[no_atual->estacao - 1] = 1;
                    }
                }
                custo_total += matriz[no_anterior->estacao][no_atual.estacao];
                no_anterior = no_atual;
                no_atual = no_atual->proximo;
            }
        }
    }

    // Se nenhuma das estações foi visitada:
    for (int k = 0 ; k < visitados.size() ; k++){
        if (!visitados[k])
            return 0;
    }

    return custo_total;
}


pair<int, int> MelhoresVertices(Problema p)
{
=======
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
>>>>>>> 639e0c8c1f8daa6652f49496ee987dd2ba6dce85
    int qtd_estacoes = p.matriz_custo.size() - 1;
    pair<int, int> melhores_vertices;
    int maior_distancia = 0;

<<<<<<< HEAD
    for (int i = 0; i < qtd_estacoes; i++)
    {
        for (int j = i + 1; j < qtd_estacoes; j++)
        {
            if (abs(p.demandas[i] + p.demandas[j]) > p.capacidade_max)
                continue;

            // direcao 1: garagem -> i -> j -> galpao
            int distancia1 = p.matriz_custo[0][i + 1] + p.matriz_custo[i + 1][j + 1] + p.matriz_custo[j + 1][0];

            // direcao 2: garagem -> j -> i -> galpao
            int distancia2 = p.matriz_custo[0][j + 1] + p.matriz_custo[j + 1][i + 1] + p.matriz_custo[i + 1][0];

            if (distancia1 > distancia2 && distancia1 > maior_distancia)
            {
                maior_distancia = distancia1;
                melhores_vertices = make_pair(i + 1, j + 1);
            }
            else if (distancia2 > maior_distancia)
            {
                maior_distancia = distancia2;
                melhores_vertices = make_pair(j + 1, i + 1);
=======
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
>>>>>>> 639e0c8c1f8daa6652f49496ee987dd2ba6dce85
            }
        }
    }

    return melhores_vertices;
}

<<<<<<< HEAD
bool ValidaDemanda(Problema problema, Rota rota){
    int demanda_total = 0;

    No* aux = rota.rota_i;
    
    while(aux){
        // se não for a garagem
        if(aux->estacao != 0){
            demanda_total += problema.demandas[aux->estacao];
            if(abs(demanda_total) > problema.capacidade_max)
                return false;
        }
        aux = aux->proximo;
    }

    return true;
}

int SomaCustoRota(Problema problema, Rota rota){
    int custo_total = 0;

    No* aux = rota.rota_i;
    
    if(!aux) return -1;

    while(aux->proximo){
        // dá pra diminuir essas atribuições, mas fica pra um momento de otimização do código, uma vez compreendido por todos
        int i = aux->estacao;
        int j = aux->proximo->estacao;

        custo_total += problema.matriz_custo[i][j];

        aux = aux->proximo;
    }

    return custo_total;
}

int SomaCusto(Problema problema, Rota rotas[]){
    int custo_total = 0;

    for(int i = 0; i < problema.qnt_veiculos; i++){
        No* aux = rotas[i].rota_i;
        
        if(!aux) continue;

        while(aux->proximo){
            int i = aux->estacao;
            int j = aux->proximo->estacao;

            custo_total += problema.matriz_custo[i][j];

            aux = aux->proximo;
        }
    }

    return custo_total;
}

bool VerificaNovaDemanda(Problema problema, Rota rota, int antecessor, int novaEstacao){
    int demanda_total = 0;
    No* aux = rota.rota_i;

    // segue enquanto há um proximo e ainda não chegou na estação dada
    while(aux){ 
        demanda_total += problema.demandas[aux->estacao];

        if(aux->estacao == antecessor) break;

        aux = aux->proximo;
    }
    // se acabou a rota e não chegou no indice passado
    if(!aux) return false;

    // verifica se após a inserção da nova demanda, a soma continua válida
    demanda_total += problema.demandas[novaEstacao];
    if(abs(demanda_total) > problema.capacidade_max)
        return false;

    // para o resto da rota, verifica sua validade após a inserção da nova demanda
    while(aux){
        demanda_total += problema.demandas[aux->estacao];
        if(abs(demanda_total) > problema.capacidade_max)
            return false;

        aux = aux->proximo;
    }

    return true;
}

int CustoInsercao(Problema problema, Rota rota, int antecessor, int novaEstacao){
    No* aux = rota.rota_i;

    while(aux){
        if(aux->estacao == antecessor){
            int i = aux->estacao;
            if(!aux->proximo) return -1; // tentando inserir no fim da rota (que presumivelmente já voltou pra garagem e o último valor é 0)
            int j = aux->proximo->estacao;

            int novo_custo = problema.matriz_custo[i][novaEstacao] + problema.matriz_custo[novaEstacao][j];

            return novo_custo - problema.matriz_custo[i][j];
        }

        aux = aux->proximo;
    }

    // se já passou do 0, ao final da rota, não é possível inserir
    // foi passado um antecessor que não está na rota
    return -1;
}

Problema ProblemaAleatorio(int n, int m, int c, int max)
{
=======
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
>>>>>>> 639e0c8c1f8daa6652f49496ee987dd2ba6dce85
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> random_demanda(-c, c);
    uniform_int_distribution<> random_custo(1, max);

    Problema problema;

    problema.qnt_estacoes = n;
    problema.veiculos_disponiveis = m;
    problema.qnt_veiculos = m;
    problema.capacidade_max = c;
<<<<<<< HEAD

    problema.demandas.resize(n);
    for (int i = 0; i < n; i++)
        problema.demandas[i] = random_demanda(gen);

    problema.matriz_custo.resize(n, vector<int>(n));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            problema.matriz_custo[i][j] = (i != j) ? random_custo(gen) : 0;

    return problema;
}

void PrintProblema(Problema p, string p_name)
{
    cout << endl
         << p_name << endl
         << endl;
    cout << p.qnt_estacoes << endl;
    cout << p.qnt_veiculos << endl;
    cout << p.capacidade_max << endl
         << endl;

    for (int demanda : p.demandas)
        cout << demanda << " ";
    cout << endl
         << endl;

    for (vector<int> custos_linha : p.matriz_custo)
    {
        for (int custo : custos_linha)
=======
    
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
>>>>>>> 639e0c8c1f8daa6652f49496ee987dd2ba6dce85
            cout << custo << " ";
        cout << endl;
    }
    return;
<<<<<<< HEAD
}

Solucao VizinhoMaisProximo(vector<vector<int>> matriz, int capmax_caminhao, vector<int> necessidades, int qtd_caminhoes){
    /*
        Implementa o algoritmo de vizinho mais próximo para cada conjunto
        Retorna a lista de rotas (listas de índices) e o custo total
    */

    vector<Rotas> rotas;
    vector<int> necessidades_rotas;
    int custo_total = 0;

    // Inicialização das estações que precisam ser visitadas
    vector<int> restam_visitar;
    for (int i = 1 ; i < matriz[0].size(); i++)
        restam_visitar.push_back(i);

    for (int i = 0 ; i < qtd_caminhoes ; i++){
        necessidades_rotas.push_back(0);
        Rota inicio; 
        No* no_i;
        no_i->estacao = 0;
        inicio.rota_i = no_i;
        rotas.push_back(inicio);
    }

    // Cada rota guarda sua fila de prioridade de inserções possíveis
    // Caso haja conflito de interesses, a inserção mais barata global
    std::map<int, vector<pair<int,int>>> fila_prioridade;

    // Monta fila de prioridade por rota
    std::map<int, int> fila_aux;

    // Para que uma estação seja visitavel, as somas das necessidades
    // Devem estar entre [-capmax_caminhao, capmax_caminhao]
    for (int estacao : restam_visitar)
        fila_aux[estacao] = matriz[0][estacao];

    // Ordena crescentemente pelo custo de inserção
    vector<pair<int, int>> fila(fila.begin(), fila.end());

    // Ordenar pelo valor (second)
    sort(fila.begin(), fila.end(),
        [](const auto &x, const auto &y) {
            return x.second < y.second;
        });
    
    //  Cada chave é um int ()
    for (int r = 0 ; r < rotas.size() ; r++)
        fila_prioridade[r] = fila;
    
    
    vector<int> rotas_a_atualizar;
    for (int id_rota = 0 ; id_rota < rotas.size() ; id_rota++)
        rotas_a_atualizar.push_back(id_rota);

    
    
    
}
=======
}
>>>>>>> 639e0c8c1f8daa6652f49496ee987dd2ba6dce85
