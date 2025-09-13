#include <bits/stdc++.h>
#define endl "\n"

using namespace std;
#define _                         \
    ios_base::sync_with_stdio(0); \
    cin.tie(0);

// ================================================================================== //
//                               Estruturas Globais                                   //
// ================================================================================== //
class Problema
{
public:
    int qnt_veiculos;
    int capacidade_max;
    vector<int> demandas; // Array de inteiros
    int qnt_estacoes;
    int veiculos_disponiveis;
    vector<vector<int>> matriz_custo;
};

class No
{
public:
    int estacao;
    No *proximo;  // Ponteiro para próximo nó
    No *anterior; // Ponteiro para o nó anterior

    No(int e = 0) : estacao(e), proximo(nullptr), anterior(nullptr) {}
};

typedef enum
{
    INICIO_FIM,
    FIM_INICIO
} DirecaoRota;

class Rota
{
public:
    No *rota_i; // Início da lista encadeada
    No *rota_f; // Fim da lista encadeada
    int custo_total_d1, custo_total_d2;
    int rotaTam; // Número de estações na rota (excluindo os depósitos)
    DirecaoRota direcao_atual;

    // Construtor padrão
    Rota() : custo_total_d1(0), custo_total_d2(0),
             rotaTam(0), direcao_atual(INICIO_FIM)
    {
        rota_i = new No(0); // Nó inicial (depósito)
        rota_f = new No(0); // Nó final (depósito)
        rota_i->proximo = rota_f;
        rota_f->anterior = rota_i;
    }

    // Construtor de cópia (deep copy)
    Rota(const Rota &other)
    {
        custo_total_d1 = other.custo_total_d1;
        custo_total_d2 = other.custo_total_d2;
        rotaTam = other.rotaTam;
        direcao_atual = other.direcao_atual;
        if (other.rota_i)
        {
            rota_i = copiaLista(other.rota_i, other.rota_f, &rota_f);
        }
        else
        {
            rota_i = nullptr;
            rota_f = nullptr;
        }
    }

    // Operador de atribuição (deep copy)
    Rota &operator=(const Rota &other)
    {
        if (this == &other)
            return *this;

        // limpa(); // libera a lista atual

        custo_total_d1 = other.custo_total_d1;
        custo_total_d2 = other.custo_total_d2;
        rotaTam = other.rotaTam;
        direcao_atual = other.direcao_atual;
        rota_i = copiaLista(other.rota_i, other.rota_f, &rota_f);

        return *this;
    }

    int stationAtIndex(int posicao)
    {
        if (posicao > (rotaTam + 1) || posicao < 0)
            return -1;

        No *atual = rota_i;
        int index_atual = 0;
        while (index_atual != posicao)
        {
            atual = atual->proximo;
            index_atual++;
        }
        return index_atual;
    }

    void InsertBegin(int estacao)
    {
        extern Problema p;
        No *novo = new No(estacao);
        novo->proximo = rota_i->proximo;
        novo->anterior = rota_i;
        rota_i->proximo->anterior = novo;
        rota_i->proximo = novo;
        rotaTam++;

        // Retira o custo matriz[0][rota_i->proximo->estacao] do custo_total_d1
        // E incrementa o custo do galpão até a nova estação + o custo da nova estação até a antiga primeira estação
        custo_total_d1 -= p.matriz_custo[0][novo->proximo->estacao];
        custo_total_d1 += p.matriz_custo[0][estacao] + p.matriz_custo[estacao][novo->proximo->estacao];

        // Retira o custo matriz[rota_i->proximo->estacao][0] do custo_total_d2
        // E incrementa o custo da antiga primeira estação até a nova estação + o custo da nova estação até o galpão
        custo_total_d2 -= p.matriz_custo[novo->proximo->estacao][0];
        custo_total_d2 += p.matriz_custo[novo->proximo->estacao][estacao] + p.matriz_custo[estacao][0];

        // Agora temos a estação inserida e o novo custo atualizado
    }

    void InsertEnd(int estacao)
    {
        extern Problema p;
        No *novo = new No(estacao);
        novo->anterior = rota_f->anterior;
        novo->proximo = rota_f;
        rota_f->anterior->proximo = novo;
        rota_f->anterior = novo;
        rotaTam++;

        // Retira o custo matriz[rota_f->anterior->estacao][0] do custo_total_d1
        // E incrementa o custo da antiga última estação até a nova estação + o custo da nova estação até o galpão
        custo_total_d1 -= p.matriz_custo[novo->anterior->estacao][0];
        custo_total_d1 += p.matriz_custo[novo->anterior->estacao][estacao] + p.matriz_custo[estacao][0];

        // Retira o custo matriz[0][rota_f->anterior->estacao] do custo_total_d2
        // E incrementa o custo do galpão até a nova estação + o custo da nova estação até a antiga última estação
        custo_total_d2 -= p.matriz_custo[0][novo->anterior->estacao];
        custo_total_d2 += p.matriz_custo[0][estacao] + p.matriz_custo[estacao][novo->anterior->estacao];

        // Agora temos a estação inserida e o novo custo atualizado
    }

    void InsertAt(int posicao, int estacao)
    {
        extern Problema p;
        if (posicao < 1 || posicao > (rotaTam + 1))
            return; // Posição inválida

        // Se a posição for mais perto do início, percorre a partir do início
        // Caso contrário, percorre a partir do fim

        No *novo = new No(estacao);
        No *atual;

        if (posicao <= rotaTam / 2)
        {
            atual = rota_i->proximo;
            for (int i = 1; i < posicao; i++)
                atual = atual->proximo;

            // Quando eu chego na posição que eu quero, a estação vai ser o novo elemento daquela posição
            // Ou seja, o atual que está na posição posicao vai ser empurrado para frente, ficando na posição posicao + 1
            novo->proximo = atual;
            novo->anterior = atual->anterior;
            atual->anterior->proximo = novo;
            atual->anterior = novo;
            rotaTam++;
        }
        else
        {
            atual = rota_f->anterior;
            for (int i = rotaTam; i > posicao; i--)
                atual = atual->anterior;

            // Quando eu chego na posição que eu quero, a estação vai ser o novo elemento daquela posição
            // Ou seja, o atual que está na posição posicao vai ser empurrado para frente, ficando na posição posicao + 1
            novo->proximo = atual;
            novo->anterior = atual->anterior;
            atual->anterior->proximo = novo;
            atual->anterior = novo;
            rotaTam++;
        }

        // Atualiza custos da direcao INICIO_FIM
        custo_total_d1 -= p.matriz_custo[novo->anterior->estacao][atual->estacao];
        custo_total_d1 += p.matriz_custo[novo->anterior->estacao][estacao] + p.matriz_custo[estacao][atual->estacao];

        // Atualiza custos da direcao FIM_INICIO
        custo_total_d2 -= p.matriz_custo[atual->estacao][novo->anterior->estacao];
        custo_total_d2 += p.matriz_custo[atual->estacao][estacao] + p.matriz_custo[estacao][novo->anterior->estacao];
    }

    /*
        Recebe um vetor de estações e insere todas na posição indicada.
        Atualiza os custos da rota de acordo com as inserções.
    */
    void InsertArrayAt(int posicao, vector<int> estacoes)
    {
        extern Problema p;
        No *novos_nos[estacoes.size()];
        novos_nos[0] = new No(estacoes[0]);

        int custo_insercao = 0, custo_insercao_2 = 0;

        for (size_t i = 1; i < estacoes.size(); i++)
        {
            novos_nos[i] = new No(estacoes[i]);
            novos_nos[i - 1]->proximo = novos_nos[i];
            novos_nos[i]->anterior = novos_nos[i - 1];
            custo_insercao += p.matriz_custo[estacoes[i - 1]][estacoes[i]];
            custo_insercao_2 += p.matriz_custo[estacoes[i]][estacoes[i - 1]];
        }

        No *atual;
        if (posicao <= rotaTam / 2)
        {
            atual = rota_i->proximo;
            for (int i = 1; i < posicao; i++)
                atual = atual->proximo;

            custo_insercao -= p.matriz_custo[atual->anterior->estacao][atual->estacao];
            custo_insercao_2 -= p.matriz_custo[atual->estacao][atual->anterior->estacao];

            // Conecta o primeiro novo nó ao nó anterior ao atual
            novos_nos[0]->anterior = atual->anterior;
            atual->anterior->proximo = novos_nos[0];
            custo_insercao += p.matriz_custo[atual->anterior->estacao][novos_nos[0]->estacao];
            custo_insercao_2 += p.matriz_custo[novos_nos[0]->estacao][atual->anterior->estacao];

            // Conecta o último novo nó ao nó atual
            novos_nos[estacoes.size() - 1]->proximo = atual;
            atual->anterior = novos_nos[estacoes.size() - 1];
            custo_insercao += p.matriz_custo[atual->anterior->estacao][atual->estacao];
            custo_insercao_2 += p.matriz_custo[atual->estacao][atual->anterior->estacao];

            // Atualiza os custos da rota
            custo_total_d1 += custo_insercao;
            custo_total_d2 += custo_insercao_2;

            // Incrementa a quantidade de estações na rota
            rotaTam += estacoes.size();
        }
        else
        {
            atual = rota_f->anterior;
            for (int i = rotaTam; i > posicao; i--)
                atual = atual->anterior;

            custo_insercao -= p.matriz_custo[atual->anterior->estacao][atual->estacao];
            custo_insercao_2 -= p.matriz_custo[atual->estacao][atual->anterior->estacao];

            // Conecta o primeiro novo nó ao nó anterior ao atual
            novos_nos[0]->anterior = atual->anterior;
            atual->anterior->proximo = novos_nos[0];
            custo_insercao += p.matriz_custo[atual->anterior->estacao][novos_nos[0]->estacao];
            custo_insercao_2 += p.matriz_custo[novos_nos[0]->estacao][atual->anterior->estacao];

            // Conecta o último novo nó ao nó atual
            novos_nos[estacoes.size() - 1]->proximo = atual;
            atual->anterior = novos_nos[estacoes.size() - 1];
            custo_insercao += p.matriz_custo[atual->anterior->estacao][atual->estacao];
            custo_insercao_2 += p.matriz_custo[atual->estacao][atual->anterior->estacao];

            // Atualiza os custos da rota
            custo_total_d1 += custo_insercao;
            custo_total_d2 += custo_insercao_2;

            // Incrementa a quantidade de estações na rota
            rotaTam += estacoes.size();
        }
    }

    void RemoveAt(int posicao)
    {
        extern Problema p;
        if (posicao < 1 || posicao > rotaTam)
            return; // Posição inválida

        No *atual;

        if (posicao <= rotaTam / 2)
        {
            atual = rota_i->proximo;
            for (int i = 1; i < posicao; i++)
                atual = atual->proximo;

            atual->anterior->proximo = atual->proximo;
            atual->proximo->anterior = atual->anterior;
            rotaTam--;
        }
        else
        {
            atual = rota_f->anterior;
            for (int i = rotaTam; i > posicao; i--)
                atual = atual->anterior;

            atual->anterior->proximo = atual->proximo;
            atual->proximo->anterior = atual->anterior;
            rotaTam--;
        }

        // Atualiza custos da direcao INICIO_FIM
        custo_total_d1 -= p.matriz_custo[atual->anterior->estacao][atual->estacao];
        custo_total_d1 -= p.matriz_custo[atual->estacao][atual->proximo->estacao];
        custo_total_d1 += p.matriz_custo[atual->anterior->estacao][atual->proximo->estacao];

        // Atualiza custos da direcao FIM_INICIO
        custo_total_d2 -= p.matriz_custo[atual->proximo->estacao][atual->estacao];
        custo_total_d2 -= p.matriz_custo[atual->estacao][atual->anterior->estacao];
        custo_total_d2 += p.matriz_custo[atual->proximo->estacao][atual->anterior->estacao];

        delete atual; // Libera memória do nó removido
    }

    // Destrutor
    ~Rota()
    {
        limpa();
    }

private:
    // Copia profunda da lista encadeada
    No *copiaLista(No *inicio, No *fim, No **novoFim)
    {

        No *novoInicio = new No(*inicio); // construtor de cópia de No
        novoInicio->anterior = nullptr;

        No *atualNovo = novoInicio;
        No *atualVelho = inicio->proximo;

        while (atualVelho)
        {
            No *novoNo = new No(*atualVelho); // copia todos os campos
            atualNovo->proximo = novoNo;
            novoNo->anterior = atualNovo;

            atualNovo = novoNo;

            if (atualVelho == fim)
            {
                *novoFim = atualNovo;
                break;
            }

            atualVelho = atualVelho->proximo;
        }

        return novoInicio;
    }

    // Libera memória da lista
    void limpa()
    {
        No *atual = rota_i;
        while (atual)
        {
            No *prox = atual->proximo;
            delete atual;
            atual = prox;
        }
        rota_i = rota_f = nullptr;
        rotaTam = 0;
        custo_total_d1 = custo_total_d2 = 0;
    }
};

class Solucao
{
public:
    int custo_total;
    int veiculos_usados;
    int veiculos_disponiveis;
    vector<Rota> rotas;
};

// ================================================================================== //
//                               Variáveis Globais                                    //
// ================================================================================== //
Problema p = Problema();
Solucao melhor_solucao;

// ================================================================================== //
//                                    FUNCOES                                         //
// ================================================================================== //
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

Solucao VizinhoMaisProximo()
{
    /*
        Implementa o algoritmo de vizinho mais próximo para cada conjunto
        Retorna a lista de rotas (listas de índices) e o custo total
    */

    vector<vector<int>> rotas(p.qnt_veiculos, vector<int>(0));
    vector<int> necessidades_rotas(p.qnt_veiculos, 0);
    int custo_total = 0;

    // Inicialização das estações que precisam ser visitadas
    vector<int> restam_visitar;
    for (int i = 1; i < p.matriz_custo[0].size(); i++)
        restam_visitar.push_back(i);

    for (int i = 0; i < p.qnt_veiculos; i++)
    {
        necessidades_rotas.push_back(0);
        vector<int> inicio = {0};
        rotas.push_back(inicio);
    }

    // Monta fila de prioridade por rota
    // tipo {{1, 2}, {3, 4}}
    map<int, int> fila;

    // Para que uma estação seja visitavel, as somas das necessidades
    // Devem estar entre [-p.capacidade_max, p.capacidade_max]
    for (int estacao : restam_visitar)
        fila[estacao] = p.matriz_custo[0][estacao]; // {id_estacao : custo}

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
    while (restam_visitar.size())
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
        necessidades_rotas[melhor_rota] += p.demandas[estacao_escolhida];

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
            if (std::abs(necessidades_rotas[melhor_rota] + p.demandas[estacao]) <= p.capacidade_max)
                ajuste[estacao] = p.matriz_custo[estacao_escolhida][estacao];
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
        custo_total += p.matriz_custo[rota[rota.size() - 1]][0];
        rota.push_back(0);
    }

    // Construção do Objeto Solução e do array de rotas
    Solucao solucao_encontrada;
    solucao_encontrada.custo_total = custo_total;
    solucao_encontrada.veiculos_usados = p.qnt_veiculos;
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

        for (int index = 1; index < rota.size() - 1; index++)
        {
            route.InsertEnd(rota[index]);
        }
        rotas_solucao.push_back(route);
    }

    solucao_encontrada.rotas = rotas_solucao;

    return solucao_encontrada;
}

Solucao InsercaoMaisBarata()
{
    Solucao s;
    vector<Rota> rotas(p.qnt_veiculos, Rota());

    // rota inicial:
    // 0 -> triang_inicial.first -> triang_inicial.second -> 0
    pair<int, int> triang_inicial = MelhoresVertices();
    rotas[0].InsertEnd(triang_inicial.first);
    rotas[0].InsertEnd(triang_inicial.second);
    int caminhoes_restantes = p.qnt_veiculos - 1;

    // Faz uma lista de estações que não estão
    // no triângulo inicial da primeira rota
    vector<int> estacoes_restantes(p.qnt_estacoes - 2);
    for (int i = 0; i < p.qnt_estacoes; i++)
    {
        if (i == triang_inicial.first || i == triang_inicial.second)
            continue;
        estacoes_restantes.push_back(i);
    }

    // Verifica onde é melhor inserir essa estação.
    // Se a relação do novo custo for negativa (diminui o custo total da rota) ou 0, insere nesta mesma rota;
    // Se for positiva, tenta inserir em outra rota (se possível)

    typedef struct
    {
        int i_rota;
        Rota rota;
        int indice;
        DirecaoRota direcao;
    } Configuracao;

    // para todas as estacoes restantes (que não foram inseridas no triangulo inical)
    for (int estacao : estacoes_restantes)
    {
        int menor_custo = INFINITY;
        Configuracao melhor_config = {
            -1,
            Rota(),
            -1,
            DirecaoRota::INICIO_FIM};

        // Ultima rota verificada
        Configuracao ultima_config = {
            rotas.size() - 1,
            Rota(),
            -1,
            DirecaoRota::INICIO_FIM};
        // para cada rota
        for (int iRota = 0; iRota < rotas.size(); iRota++)
        {
            if (rotas[iRota].rota_i == nullptr)
            {
                ultima_config.i_rota = iRota;
                ultima_config.direcao = DirecaoRota::INICIO_FIM;
                break;
            }
            // PARA A ROTA NO SENTIDO INICIO-FIM
            Rota rota = rotas[iRota];

            No *aux = rotas[iRota].rota_i;
            int i = 0;
            while (aux && aux->estacao)
            {

                // calcula o custo de insercao neste nó;
                int custo_insercao = p.matriz_custo[aux->estacao][estacao] + p.matriz_custo[estacao][aux->proximo->estacao];
                int custo = custo_insercao - p.matriz_custo[aux->estacao][aux->proximo->estacao];

                if (custo < menor_custo)
                {
                    if (VerificaNovaDemanda(rotas[iRota], i, estacao))
                    {
                        menor_custo = custo;
                        melhor_config = {
                            iRota,
                            rotas[iRota],
                            i,
                            DirecaoRota::INICIO_FIM};
                    }
                }

                aux = aux->proximo;
                i++;
            }

            // percorrer ao contrario tambem
            aux = rotas[iRota].rota_f;
            i = 0;
            while (aux && aux->estacao)
            {

                // calcula o custo de insercao neste nó;
                int custo_insercao = p.matriz_custo[aux->estacao][estacao] + p.matriz_custo[estacao][aux->anterior->estacao];
                int custo = custo_insercao - p.matriz_custo[aux->estacao][aux->anterior->estacao];

                if (custo < menor_custo)
                {
                    if (VerificaNovaDemanda(rotas[iRota], i, estacao))
                    {
                        menor_custo = custo;
                        melhor_config = {
                            iRota,
                            rotas[iRota],
                            i,
                            DirecaoRota::FIM_INICIO};
                    }
                }

                aux = aux->anterior;
                i++;
            }
        }
        if (
            menor_custo <= (p.matriz_custo[0][estacao] + p.matriz_custo[estacao][0]) ||
            caminhoes_restantes == 0)
        {
            // inserção da melhor configuração na rota definida
            rotas[melhor_config.i_rota].direcao_atual = melhor_config.direcao;
            rotas[melhor_config.i_rota].InsertAt(melhor_config.indice, estacao);
        }
        else
        {
            // inserção no inicio, pois é uma nova rota
            rotas[ultima_config.i_rota].InsertBegin(estacao);
        }
    }

    for (Rota rota : rotas)
    {
        if (rota.direcao_atual == DirecaoRota::INICIO_FIM)
            s.custo_total += rota.custo_total_d1;
        else
            s.custo_total += rota.custo_total_d2;
    }

    s.rotas = rotas;
    s.veiculos_disponiveis = caminhoes_restantes;
    s.veiculos_usados = p.qnt_veiculos - caminhoes_restantes;

    return s;
}

vector<Rota> MelhorarSolucao(vector<Rota> &rotas)
{
melhorar_solucao:
    int index_rota_origem = 0;
    for (Rota rota : rotas)
    {
        // Itera por cada estação em rota

        No *no_atual = rota.rota_i->proximo;
        int index_rota_destino = 0;
        int pos_origem = 1;
        while (no_atual->estacao)
        {
            int index_rota_destino = 0;
            for (Rota rota_destino : rotas)
            {
                No *no_destino = rota_destino.rota_i->proximo;
                int pos = 1;

                while (no_destino->estacao)
                {
                    int custo_remocao, custo_insercao, ganho_ao_remover, ganho_ao_inserir;

                    if (rota.direcao_atual == DirecaoRota::INICIO_FIM)
                    {
                        custo_remocao = p.matriz_custo[no_atual->anterior->estacao][no_atual->proximo->estacao] -
                                        p.matriz_custo[no_atual->anterior->estacao][no_atual->estacao] -
                                        p.matriz_custo[no_atual->estacao][no_atual->proximo->estacao];

                        custo_insercao = p.matriz_custo[no_destino->anterior->estacao][no_atual->estacao] +
                                         p.matriz_custo[no_atual->estacao][no_destino->estacao] -
                                         p.matriz_custo[no_destino->anterior->estacao][no_destino->estacao];

                        ganho_ao_remover = (rota.custo_total_d1 + custo_remocao) - rota.custo_total_d1;
                        ganho_ao_inserir = (rota_destino.custo_total_d1 + custo_insercao) - rota_destino.custo_total_d1;
                    }
                    else
                    {
                        custo_remocao = p.matriz_custo[no_atual->proximo->estacao][no_atual->anterior->estacao] -
                                        p.matriz_custo[no_atual->proximo->estacao][no_atual->estacao] -
                                        p.matriz_custo[no_atual->estacao][no_atual->anterior->estacao];

                        custo_insercao = p.matriz_custo[no_destino->anterior->estacao][no_atual->estacao] +
                                         p.matriz_custo[no_atual->estacao][no_destino->estacao] -
                                         p.matriz_custo[no_destino->anterior->estacao][no_destino->estacao];

                        ganho_ao_remover = (rota.custo_total_d2 + custo_remocao) - rota.custo_total_d2;
                        ganho_ao_inserir = (rota_destino.custo_total_d2 + custo_insercao) - rota_destino.custo_total_d2;
                    }

                    if (ganho_ao_remover > ganho_ao_inserir)
                    {
                        no_destino = no_destino->proximo;
                        pos++;
                        continue;
                    }

                    // Cópia para teste
                    Rota nova_rota_origem = rota;
                    Rota nova_rota_destino;

                    if (index_rota_origem == index_rota_destino)
                    {
                        nova_rota_destino = nova_rota_origem;
                    }
                    else
                    {
                        nova_rota_destino = rotas[index_rota_destino];
                    }

                    nova_rota_origem.RemoveAt(pos_origem);
                    nova_rota_destino.InsertAt(pos, no_atual->estacao);

                    if (ValidaDemanda(nova_rota_origem) &&
                        ValidaDemanda(nova_rota_destino))
                    {
                        rotas[index_rota_origem] = nova_rota_origem;
                        rotas[index_rota_destino] = nova_rota_destino;
                        goto melhorar_solucao;
                    }

                    no_destino = rota_destino.rota_i->proximo;
                    pos++;
                }
                index_rota_destino++;
            }

            no_atual = no_atual->proximo;
            pos_origem++;
        }

        index_rota_origem++;
    }

    return rotas;
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

// ================================================================================== //
//                                      MAIN                                          //
// ================================================================================== //
int main()
{
    _

            cin >>
        p.qnt_estacoes >> p.qnt_veiculos >> p.capacidade_max;

    p.demandas.resize(p.qnt_estacoes);
    p.matriz_custo.resize(p.qnt_estacoes + 1, vector<int>(p.qnt_estacoes + 1));

    for (int i = 0; i < p.qnt_estacoes; i++)
        cin >> p.demandas[i];

    for (int i = 0; i <= p.qnt_estacoes; i++)
        for (int j = 0; j <= p.qnt_estacoes; j++)
            cin >> p.matriz_custo[i][j];

    melhor_solucao = InsercaoMaisBarata();
    melhor_solucao.rotas = MelhorarSolucao(melhor_solucao.rotas);

    cout << "Custo total: " << melhor_solucao.custo_total << endl;
    cout << "Veículos usados: " << melhor_solucao.veiculos_usados << endl;
    for (Rota rota : melhor_solucao.rotas)
    {
        if (rota.rotaTam == 0)
            continue;

        if (rota.direcao_atual == DirecaoRota::INICIO_FIM)
        {
            No *aux = rota.rota_i;
            while (aux)
            {
                cout << aux->estacao << " ";
                aux = aux->proximo;
            }
            cout << endl;
        }
        else
        {
            No *aux = rota.rota_f;
            while (aux)
            {
                cout << aux->estacao << " ";
                aux = aux->anterior;
            }
            cout << endl;
        }
    }

    return 0;
}