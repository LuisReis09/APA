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
#include <map>
#include <algorithm>
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
    MyFile.close();

    return problema_iniciado;
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
                custo_total += matriz[no_anterior->estacao][no_atual->estacao];
                no_anterior = no_atual;
                no_atual = no_atual->proximo;
            }
        }
    }

    // Se nenhuma das estações foi visitada:
    for (int k = 0; k < visitados.size(); k++)
    {
        if (!visitados[k])
            return 0;
    }

    return custo_total;
}

pair<int, int> MelhoresVertices(Problema p)
{
    int qtd_estacoes = p.matriz_custo.size() - 1;
    pair<int, int> melhores_vertices;
    int maior_distancia = 0;

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
            }
        }
    }

    return melhores_vertices;
}

bool ValidaDemanda(Problema problema, Rota rota)
{
    int demanda_total = 0;

    No *aux = rota.rota_i;

    while (aux)
    {
        // se não for a garagem
        if (aux->estacao != 0)
        {
            demanda_total += problema.demandas[aux->estacao];
            if (abs(demanda_total) > problema.capacidade_max)
                return false;
        }
        aux = aux->proximo;
    }

    return true;
}

int SomaCustoRota(Problema problema, Rota rota)
{
    int custo_total = 0;

    No *aux = rota.rota_i;

    if (!aux)
        return -1;

    while (aux->proximo)
    {
        // dá pra diminuir essas atribuições, mas fica pra um momento de otimização do código, uma vez compreendido por todos
        int i = aux->estacao;
        int j = aux->proximo->estacao;

        custo_total += problema.matriz_custo[i][j];

        aux = aux->proximo;
    }

    return custo_total;
}

int SomaCusto(Problema problema, Rota rotas[])
{
    int custo_total = 0;

    for (int i = 0; i < problema.qnt_veiculos; i++)
    {
        No *aux = rotas[i].rota_i;

        if (!aux)
            continue;

        while (aux->proximo)
        {
            int i = aux->estacao;
            int j = aux->proximo->estacao;

            custo_total += problema.matriz_custo[i][j];

            aux = aux->proximo;
        }
    }

    return custo_total;
}

bool VerificaNovaDemanda(Problema problema, Rota rota, int antecessor, int novaEstacao)
{
    int demanda_total = 0;
    No *aux = rota.rota_i;

    // segue enquanto há um proximo e ainda não chegou na estação dada
    while (aux)
    {
        demanda_total += problema.demandas[aux->estacao];

        if (aux->estacao == antecessor)
            break;

        aux = aux->proximo;
    }
    // se acabou a rota e não chegou no indice passado
    if (!aux)
        return false;

    // verifica se após a inserção da nova demanda, a soma continua válida
    demanda_total += problema.demandas[novaEstacao];
    if (abs(demanda_total) > problema.capacidade_max)
        return false;

    // para o resto da rota, verifica sua validade após a inserção da nova demanda
    while (aux)
    {
        demanda_total += problema.demandas[aux->estacao];
        if (abs(demanda_total) > problema.capacidade_max)
            return false;

        aux = aux->proximo;
    }

    return true;
}

int CustoInsercao(Problema problema, Rota rota, int antecessor, int novaEstacao)
{
    No *aux = rota.rota_i;

    while (aux)
    {
        if (aux->estacao == antecessor)
        {
            int i = aux->estacao;
            if (!aux->proximo)
                return -1; // tentando inserir no fim da rota (que presumivelmente já voltou pra garagem e o último valor é 0)
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
            cout << custo << " ";
        cout << endl;
    }
    return;
}

Solucao VizinhoMaisProximo(vector<vector<int>> matriz, int capmax_caminhao, vector<int> necessidades, int qtd_caminhoes)
{
    /*
        Implementa o algoritmo de vizinho mais próximo para cada conjunto
        Retorna a lista de rotas (listas de índices) e o custo total
    */

    vector<vector<int>> rotas;
    vector<int> necessidades_rotas;
    int custo_total = 0;

    // Inicialização das estações que precisam ser visitadas
    vector<int> restam_visitar;
    for (int i = 1; i < matriz[0].size(); i++)
        restam_visitar.push_back(i);

    for (int i = 0; i < qtd_caminhoes; i++)
    {
        necessidades_rotas.push_back(0);
        vector<int> inicio = {0};
        rotas.push_back(inicio);
    }

    // Monta fila de prioridade por rota
    // tipo {{1, 2}, {3, 4}}
    map<int, int> fila;

    // Para que uma estação seja visitavel, as somas das necessidades
    // Devem estar entre [-capmax_caminhao, capmax_caminhao]
    for (int estacao : restam_visitar)
        fila[estacao] = matriz[0][estacao]; // {id_estacao : custo}

    // Cada rota guarda sua fila de prioridade de inserções possíveis
    // Caso haja conflito de interesses, a inserção mais barata global
    // Estrutura do tipo: {1, {2, 5}}
    map<int, map<int, int>> fila_prioridade; // Se trata de um mapa de mapas

    // Cada fila de prioridade recebe uma cópia de fila
    for (int r = 0; r < rotas.size(); r++)
        fila_prioridade[r] = fila; // Estrutura do tipo {id_rota : {{1, 2}, {3, 4} }}

    vector<int> rotas_a_atualizar;
    for (int id_rota = 0; id_rota < rotas.size(); id_rota++)
        rotas_a_atualizar.push_back(id_rota);

    // Enquanto houver estações para visitar
    while (restam_visitar.size() != 0)
    {
        // Melhor rota é definida com
        // 1. Menor custo de inserção
        // 2. Mais perto de carga 0
        // encontra a melhor rota em rotas_a_atualizar
        auto melhor_it = std::min_element(
            rotas_a_atualizar.begin(),
            rotas_a_atualizar.end(),
            [&](int a, int b)
            {
                // pega o primeiro par (estacao, custo) de cada fila de prioridade
                auto it_a = fila_prioridade[a].begin();
                auto it_b = fila_prioridade[b].begin();

                int valor_a = it_a->second; // equivalente a next(iter(dict.values()))
                int valor_b = it_b->second;

                // critério 1: menor custo
                if (valor_a != valor_b)
                    return valor_a < valor_b;

                // critério 2: mais perto de 0
                int crit_a = std::abs(necessidades_rotas[a] + it_a->first); // equivalente a next(iter(dict.keys()))
                int crit_b = std::abs(necessidades_rotas[b] + it_b->first);
                return crit_a < crit_b;
            });

        int melhor_rota = (melhor_it != rotas_a_atualizar.end()) ? *melhor_it : -1;

        // Seleciona a estação de menor custo : Necessario achar o indice
        int index_menorCusto;
        auto it = min_element( // Retorna o par chave : valor
            fila_prioridade[melhor_rota].begin(),
            fila_prioridade[melhor_rota].end(),
            [](const auto &a, const auto &b)
            { return a.second < b.second; });

        int estacao_escolhida = fila_prioridade[melhor_rota][it->second]; // Supõe se que os elementos de menor custo vem primeiro
        int custo_escolhido = fila_prioridade[melhor_rota][estacao_escolhida];

        rotas[melhor_rota].push_back(estacao_escolhida);
        necessidades_rotas[melhor_rota] += necessidades[estacao_escolhida];

        custo_total += custo_escolhido;

        // Removendo por valor
        restam_visitar.erase(
            remove(restam_visitar.begin(),
                   restam_visitar.end(),
                   estacao_escolhida),
            restam_visitar.end());

        // Para a rota que acabou de inserir, é necessário reajustar sua fila de prioridades, considerando a nova estação final
        map<int, int> ajuste;
        for (int estacao : restam_visitar)
        {
            if (std::abs(necessidades_rotas[melhor_rota] + necessidades[estacao]) <= capmax_caminhao)
                ajuste[estacao] = matriz[estacao_escolhida][estacao];
        }
        fila_prioridade[melhor_rota] = ajuste;

        // Remove a estação escolhida de todas as filas
        for (int i : rotas_a_atualizar)
            fila_prioridade[i].erase(estacao_escolhida);

        rotas_a_atualizar.erase(
            std::remove_if(
                rotas_a_atualizar.begin(),
                rotas_a_atualizar.end(),
                [&](int i)
                {
                    return fila_prioridade[i].empty();
                    // remove se o mapa dessa rota estiver vazio
                }),
            rotas_a_atualizar.end());
    }

    // Finalmente, cada rota deve retornar ao galpão
    for (vector<int> rota : rotas)
    {
        custo_total += matriz[rota[rota.size() - 1]][0];
        rota.push_back(0);
    }

    // Construção do Objeto Solução e do array de rotas
    Solucao solucao_encontrada;
    solucao_encontrada.custo_total = custo_total;
    solucao_encontrada.veiculos_usados = qtd_caminhoes;
    solucao_encontrada.veiculos_disponiveis = 0;

    vector<int> rota_vazia = {0, 0};
    vector<Rota> rotas_solucao;
    for (vector<int> rota : rotas)
    {
        if (rota == rota_vazia)
        {
            solucao_encontrada.veiculos_usados--;
            solucao_encontrada.veiculos_disponiveis++;
        }

        // Construção do objeto Rota
        Rota route;
        No *inicio;
        No *fim;
        inicio->estacao = 0;
        fim->estacao = 0;

        route.rota_i = inicio;
        route.rota_f = fim;
        route.custo_total_d1 = 0;
        route.custo_total_d2 = 0;

        No *no_atual = route.rota_i;
        no_atual->soma_demandas_d1 = 0;
        no_atual->soma_demandas_d2 = 0;

        for (int index_rota = 1; index_rota < rota.size() - 1; index_rota++)
        {
            // Construção de um novo nó e inserção na Rota

            No *novo_no;
            novo_no->soma_demandas_d1 = no_atual->soma_demandas_d1;
            novo_no->soma_demandas_d2 = no_atual->soma_demandas_d2;

            novo_no->estacao = rota[index_rota];

            no_atual->proximo = novo_no;
            no_atual->custo_d1 = matriz[no_atual->estacao][novo_no->estacao];
            route.custo_total_d1 += no_atual->custo_d1;

            if (index_rota != 1)
            { // Nesse caso em que index_rota == 1, no_atual é nó de inicio, logo, não tem anterior
                no_atual->custo_d2 = matriz[no_atual->anterior->estacao][no_atual->estacao];
                no_atual->anterior->soma_demandas_d2 += necessidades[novo_no->estacao];
            }

            novo_no->anterior = no_atual;
            route.custo_total_d2 += matriz[index_rota][no_atual->estacao];

            novo_no->soma_demandas_d1 += necessidades[novo_no->estacao];
            no_atual->soma_demandas_d2 += necessidades[novo_no->estacao];

            // soma de d2 em rota_i sempre será igual ao somatorio de d2
            route.rota_i->soma_demandas_d2 += novo_no->soma_demandas_d1;

            no_atual = novo_no;
        }
        no_atual->proximo = route.rota_f;
        route.rota_f->anterior = no_atual;
        route.rota_f->soma_demandas_d1 = route.rota_i->soma_demandas_d2;

        rotas_solucao.push_back(route);
    }

    solucao_encontrada.rotas = rotas_solucao;

    return solucao_encontrada;
}
