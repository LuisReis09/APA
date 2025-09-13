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
#include "utils.hpp"
using namespace std;

Problema p;

// seeds pra função de problema aleatorio
mt19937 gen(time(0));

void SetProblema(Problema prob)
{
    p = prob;
}

void SetProblema(
    int qnt_veiculos,
    int capacidade_max,
    vector<int> demandas,
    int qnt_estacoes,
    int veiculos_disponiveis,
    vector<vector<int>> matriz_custo)
{
    p.capacidade_max = capacidade_max;
    p.demandas = demandas;
    p.matriz_custo = matriz_custo;
    p.qnt_estacoes = qnt_estacoes;
    p.qnt_veiculos = qnt_veiculos;
    p.veiculos_disponiveis = veiculos_disponiveis;
}
Problema GetProblema()
{
    return p;
}

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

int VerificaSolucao(vector<Rota> rotas)
{
    /*
        Verifica se as rotas passadas são possiveis
    */

    // Inicialização de visitados
    vector<int> visitados;
    for (int i : p.matriz_custo[0])
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
            while (no_atual->estacao)
            {
                soma_carga += p.demandas[no_atual->estacao];

                // Checagem se a carga é possível
                if (abs(soma_carga) > p.capacidade_max)
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
                custo_total += p.matriz_custo[no_anterior->estacao][no_atual->estacao];
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

pair<int, int> MelhoresVertices()
{
    int qtd_estacoes = p.qnt_estacoes;
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

bool ValidaDemanda(Rota rota)
{
    int demanda_total = 0;

    if (rota.direcao_atual == DirecaoRota::INICIO_FIM)
    {
        No *aux = rota.rota_i->proximo; // pula a garagem (se primeiro valor da rota é a garagem)
        // Enquanto não voltar pra garagem (na qual estação vale 0) e houver Nos na rota
        while (aux && aux->estacao)
        {
            demanda_total += p.demandas[aux->estacao - 1];
            if (abs(demanda_total) > p.capacidade_max)
                return false;
            aux = aux->proximo;
        }
    }
    else
    {
        // se a rota já tiver sido findada (possui a estacao 0, garagem, no fim)
        // atribui o anterior para começar a avaliar as demandas
        // se não, inicia pelo último elemento, que não é a garagem, portanto, possui demanda
        No *aux = rota.rota_f->estacao ? rota.rota_f : rota.rota_f->anterior;

        // enquanto não chegar na garagem, onde a rota DEVE iniciar
        while (aux->estacao)
        {
            demanda_total += p.demandas[aux->estacao - 1];
            if (abs(demanda_total) > p.capacidade_max)
                return false;
            aux = aux->anterior;
        }
    }

    return true;
}

int SomaCustoRota(Rota rota)
{
    int custo_total = 0;

    if (rota.direcao_atual == DirecaoRota::INICIO_FIM)
    {
        No *aux = rota.rota_i;
        if (!aux)
            return -1;
        while (aux && aux->estacao)
        {
            custo_total += p.matriz_custo[aux->estacao][aux->proximo->estacao];
            aux = aux->proximo;
        }
    }
    else
    {
        No *aux = rota.rota_f;
        if (!aux)
            return -1;
        while (aux && aux->estacao)
        {
            custo_total += p.matriz_custo[aux->estacao][aux->anterior->estacao];
            aux = aux->anterior;
        }
    }

    return custo_total;
}

int SomaCusto(vector<Rota> rotas)
{
    int custo_total = 0;

    for (int i = 0; i < rotas.size(); i++)
    {
        if (rotas[i].direcao_atual == DirecaoRota::INICIO_FIM)
        {
            No *aux = rotas[i].rota_i;
            if (!aux)
                continue;
            while (aux && aux->estacao)
            {
                custo_total += p.matriz_custo[aux->estacao][aux->proximo->estacao];
                aux = aux->proximo;
            }
        }
        else
        {
            No *aux = rotas[i].rota_f;
            if (!aux)
                continue;
            while (aux && aux->estacao)
            {
                custo_total += p.matriz_custo[aux->estacao][aux->anterior->estacao];
                aux = aux->anterior;
            }
        }
    }

    return custo_total;
}

bool VerificaNovaDemanda(Rota rota, int antecessor, int novaEstacao)
{
    int demanda_total = 0;

    if (rota.direcao_atual == DirecaoRota::INICIO_FIM)
    {
        No *aux = rota.rota_i;
        // segue enquanto há um proximo e ainda não chegou na estação dada
        while (aux && aux->estacao)
        {
            demanda_total += p.demandas[aux->estacao - 1];
            if (aux->estacao == antecessor)
                break;
            aux = aux->proximo;
        }
        // se acabou a rota e não chegou no indice passado
        if (!aux->estacao)
            return false;

        // verifica se após a inserção da nova demanda, a soma continua válida
        demanda_total += p.demandas[novaEstacao - 1];
        if (abs(demanda_total) > p.capacidade_max)
            return false;

        // para o resto da rota, verifica sua validade após a inserção da nova demanda
        while (aux && aux->estacao)
        {
            demanda_total += p.demandas[aux->estacao - 1];
            if (abs(demanda_total) > p.capacidade_max)
                return false;
            aux = aux->proximo;
        }
    }
    else
    {
        No *aux = rota.rota_f;
        // segue enquanto há um proximo e ainda não chegou na estação dada
        while (aux && aux->estacao)
        {
            demanda_total += p.demandas[aux->estacao - 1];
            if (aux->estacao == antecessor)
                break;
            aux = aux->anterior;
        }
        // se acabou a rota e não chegou no indice passado
        if (!aux->estacao)
            return false;

        // verifica se após a inserção da nova demanda, a soma continua válida
        demanda_total += p.demandas[novaEstacao - 1];
        if (abs(demanda_total) > p.capacidade_max)
            return false;

        // para o resto da rota, verifica sua validade após a inserção da nova demanda
        while (aux && aux->estacao)
        {
            demanda_total += p.demandas[aux->estacao - 1];
            if (abs(demanda_total) > p.capacidade_max)
                return false;
            aux = aux->anterior;
        }
    }
    return true;
}

int CustoInsercao(Rota rota, int antecessor, int novaEstacao)
{
    if (rota.direcao_atual == DirecaoRota::INICIO_FIM)
    {
        No *aux = rota.rota_i;
        while (aux->proximo)
        {
            if (aux->estacao == antecessor)
            {
                if (!aux->proximo)
                    return -1; // tentando inserir no fim da rota (que presumivelmente já voltou pra garagem e o último valor é 0)
                int novo_custo = p.matriz_custo[aux->estacao][novaEstacao] + p.matriz_custo[novaEstacao][aux->proximo->estacao];
                return novo_custo - p.matriz_custo[aux->estacao][aux->proximo->estacao];
            }
            aux = aux->proximo;
        }
    }
    else
    {
        No *aux = rota.rota_f;
        while (aux->proximo)
        {
            if (aux->estacao == antecessor)
            {
                if (!aux->anterior)
                    return -1; // tentando inserir no fim da rota (que presumivelmente já voltou pra garagem e o último valor é 0)
                int novo_custo = p.matriz_custo[aux->estacao][novaEstacao] + p.matriz_custo[novaEstacao][aux->anterior->estacao];
                return novo_custo - p.matriz_custo[aux->estacao][aux->anterior->estacao];
            }
            aux = aux->anterior;
        }
    }

    // se já passou do 0, ao final ou inicio da rota, não é possível inserir
    // foi passado um antecessor que não está na rota
    return -1;
}

Problema ProblemaAleatorio(int n, int m, int c, int max)
{
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

void PrintProblema(string p_name)
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

void PrintSolucao(Solucao s, string s_name)
{
    cout << "====================================" << endl;
    cout << "           " << s_name << endl;
    cout << "====================================" << endl
         << endl;
    cout << "Custo total:   " << s.custo_total << endl;
    cout << "Qtd veiculos:  " << s.veiculos_usados << endl;
    cout << "Veiculos disp: " << s.veiculos_disponiveis << endl;
    cout << "Rotas: " << endl;

    int i = 0;
    for (Rota rota : s.rotas)
    { // melhore, herick. ()(So me matando pra melhorar)
        // rt like e bookmark KSKDKASDv v ctrl+s, plsFOI
        // segmentation faul (saranhe, tsugi)
        if (rota.direcao_atual == DirecaoRota::INICIO_FIM)
        {
            cout << "\tCusto: " << rota.custo_total_d1 << endl;
            cout << "\tRota " << i << ": (";

            No *aux = rota.rota_i;
            if (!aux)
            {
                cout << "(0 -> 0)";
                continue;
            }
            while (true)
            {
                cout << aux->estacao;
                aux = aux->proximo;
                if (!aux->proximo)
                    cout << " -> ";
                else
                {
                    cout << ")\n";
                    break;
                }
            }
        }
        else
        {
            cout << "\tCusto: " << rota.custo_total_d2 << endl;
            cout << "\tRota " << i << ": (";

            No *aux = rota.rota_f;
            if (!aux)
            {
                cout << "(0 -> 0)";
                continue;
            }
            while (true)
            {
                cout << aux->estacao;
                aux = aux->anterior;
                if (!aux->anterior)
                    cout << " -> ";
                else
                {
                    cout << ")\n";
                    break;
                }
            }
        }

        i++;
    }
    cout << endl;

    return;
}

void PrintRotas(vector<Rota> rotas)
{
    for (Rota rota : rotas)
    {
        rota.printRota();
    }
}

// Função que inicia a cronometragem, retorna um objeto clock_t
clock_t ComecarCronometro()
{
    clock_t inicio = clock();
    return inicio;
}
float getDuracao(clock_t cronometro_inicio)
{
    clock_t duracao = clock() - cronometro_inicio;
    return (float)duracao / CLOCKS_PER_SEC;
}