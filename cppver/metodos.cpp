#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <random>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cmath>
#include <utility>
#include <thread>
#include <map>
#include <algorithm>
#include "metodos.hpp"
using namespace std;

Solucao VizinhoMaisProximo()
{
    /*
        Implementa o algoritmo de vizinho mais próximo para cada conjunto
        Retorna a lista de rotas (listas de índices) e o custo total
    */

    vector<vector<int>> rotas(p.qnt_veiculos, vector<int>(0));
    vector<int> necessidades_rotas(p.qnt_veiculos, 0);
    vector<vector<int>> rotas(p.qnt_veiculos, vector<int>(0));
    vector<int> necessidades_rotas(p.qnt_estacoes);
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

        for(int index = 1 ; index < rota.size() - 1; index++){
            route.InsertEnd(rota[index]);
        }
        rotas_solucao.push_back(route);
    }

    solucao_encontrada.rotas = rotas_solucao;

    return solucao_encontrada;
}

vector<Rota> MelhorarSolucao(vector<Rota> rotas)
{
    int index_rota_origem = 0;
    for (Rota rota : rotas)
    {
        // Itera por cada estação em rota

        No *no_atual = rota.rota_i;
        int index_rota_destino = 0;
        while (no_atual->proximo != rota.rota_f)
        {
            int index_rota_destino = 0;
            for (Rota rota_destino : rotas)
            {
                for (int pos = 1; pos < rota_destino.rotaTam - 1; pos++)
                {
                    if (no_atual->estacao == 0) // Nunca mover depositos
                        continue;

                    // Cópia para teste
                    Rota nova_rota_origem = rota;
                    Rota nova_rota_destino;

                    // Remoção da estação atual pra nova_rota_origem
                    // Achando a posição do alvo
                    

                    No* aux = nova_rota_origem.rota_i; int posicao = 0;
                    while (aux->estacao != no_atual->estacao){
                        posicao++;
                        aux = aux->proximo;
                    }
                    nova_rota_origem.RemoveAt(posicao);

                    index_rota_origem == index_rota_destino
                        ? nova_rota_destino = nova_rota_origem
                        : nova_rota_destino = rota_destino;

                    // Inserção da estação na posição válida
                    nova_rota_destino.InsertAt(posicao, no_atual->estacao);
                    
                    // Checa viabilidade
                    if (!ValidaDemanda(nova_rota_destino) || !ValidaDemanda(nova_rota_origem))
                        continue;

                    // Calculo de custos
                    int custo_atual;
                    int custo_novo;

                    if (index_rota_destino == index_rota_origem)
                    {
                        rota.direcao_atual >= INICIO_FIM ? custo_atual = rota.custo_total_d1 : custo_atual = rota.custo_total_d2;
                        custo_novo = SomaCustoRota(nova_rota_destino);
                    }
                    else
                    {
                        custo_atual = SomaCusto({rota, rota_destino});
                        custo_novo = SomaCusto({nova_rota_origem, nova_rota_destino});
                    }

                    // Aplica a mudança se melhorou
                    if (custo_novo < custo_atual)
                    {
                        rotas[index_rota_destino] = nova_rota_destino;
                        if (index_rota_destino != index_rota_origem)
                            rotas[index_rota_origem] = nova_rota_origem;
                        return MelhorarSolucao(rotas);
                    }
                }
                index_rota_destino++;
            }

            no_atual = no_atual->proximo;
        }

        index_rota_origem++;
    }

    return rotas;
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

    typedef struct {
        int i_rota;
        Rota rota;
        int indice;
        DirecaoRota direcao;
    } Configuracao;

    // para todas as estacoes restantes (que não foram inseridas no triangulo inical)
    for(int estacao: estacoes_restantes){
        int menor_custo = INFINITY;
        Configuracao melhor_config = {
            -1, 
            Rota(), 
            -1, 
            DirecaoRota::INICIO_FIM
        };

        // Ultima rota verificada
        Configuracao ultima_config = {
            rotas.size() -1,
            Rota(),
            -1, 
            DirecaoRota::INICIO_FIM
        };
        // para cada rota
        for(int iRota = 0; iRota < rotas.size(); iRota++){
            if(rotas[iRota].rota_i == nullptr) {
                ultima_config.i_rota = iRota;
                ultima_config.direcao = DirecaoRota::INICIO_FIM;
                break;
            }
            // PARA A ROTA NO SENTIDO INICIO-FIM
            Rota rota = rotas[iRota];

            No* aux = rotas[iRota].rota_i;
            int i = 0;
            while(aux->proximo){
                
                // calcula o custo de insercao neste nó;
                int custo_insercao = p.matriz_custo[aux->estacao][estacao] + p.matriz_custo[estacao][aux->proximo->estacao];
                int custo = custo_insercao - p.matriz_custo[aux->estacao][aux->proximo->estacao];
                
                if(custo < menor_custo){
                    if(VerificaNovaDemanda(rotas[iRota], i, estacao)){
                        menor_custo = custo;
                        melhor_config = {
                            iRota, 
                            rotas[iRota], 
                            i, 
                            DirecaoRota::INICIO_FIM
                        };
                    }
                }
                
                aux = aux->proximo;
                i++;
            }

            // percorrer ao contrario tambem
            aux = rotas[iRota].rota_f;
            i = 0;
            while(aux->anterior){
                
                // calcula o custo de insercao neste nó;
                int custo_insercao = p.matriz_custo[aux->estacao][estacao] + p.matriz_custo[estacao][aux->anterior->estacao];
                int custo = custo_insercao - p.matriz_custo[aux->estacao][aux->anterior->estacao];
    
                if(custo < menor_custo){
                    if(VerificaNovaDemanda(rotas[iRota], i, estacao)){
                        menor_custo = custo;
                        melhor_config = {
                            iRota, 
                            rotas[iRota], 
                            i, 
                            DirecaoRota::FIM_INICIO
                        };
                    }
                }

                aux = aux->anterior;
                i++;
            }

        }
        if(
            menor_custo <= (p.matriz_custo[0][estacao] + p.matriz_custo[estacao][0])
            || 
            caminhoes_restantes == 0
        ){
            // inserção da melhor configuração na rota definida
            rotas[melhor_config.i_rota].direcao_atual = melhor_config.direcao;
            rotas[melhor_config.i_rota].InsertAt(melhor_config.indice, estacao);
        }else{
            // inserção no inicio, pois é uma nova rota
            rotas[ultima_config.i_rota].InsertBegin(estacao);
        }
    }

    for(Rota rota: rotas){
        if(rota.direcao_atual == DirecaoRota::INICIO_FIM)
            s.custo_total += rota.custo_total_d1;
        else    
            s.custo_total += rota.custo_total_d2;
    }

    s.rotas = rotas;
    s.veiculos_disponiveis = caminhoes_restantes;
    s.veiculos_usados = p.qnt_veiculos - caminhoes_restantes;

    return s;
}

/* 
    Aplicar testes de swap na rota e retornar a melhor rota encontrada 
    ou a mesma que recebeu, caso não haja melhora
 */
Rota VND_Swap(Rota const r){
    Rota r_ret = Rota();
    int melhor_custo = INFINITY;
    int custo_anterior, custo_swap, novo_custo;

    No *melhor_aux1 = nullptr, *melhor_aux2 = nullptr;
    
    // cria uma cópia da rota recebida, a qual será modificada e retornada, 
    // de acordo com o algoritmo
    {
        No* aux = r.rota_i->proximo;
        while(aux->proximo){
            r_ret.InsertEnd(aux->estacao);
            aux = aux->proximo;
        }
        r_ret.direcao_atual = r.direcao_atual;
        r_ret.custo_total_d1 = r.custo_total_d1;
        r_ret.custo_total_d2 = r.custo_total_d2;
        r_ret.rotaTam = r.rotaTam;
    }


    if(r.direcao_atual == DirecaoRota::INICIO_FIM){
        // começa do primeiro elemento, ignorando a garagem
        No* aux1 = r_ret.rota_i->proximo;

        if(!aux1) return r_ret;

        // termina quando chegar na garagem
        while(aux1->proximo){

            // mesma lógica que a anterior, porém começando do próximo de aux1
            No* aux2 = aux1->proximo;
            if(!aux2){
                // só há um elemento
                if(aux1 == r_ret.rota_i->proximo)
                    return r_ret;
                break;
            }
            
            while(aux2->proximo){

                custo_anterior = p.matriz_custo[aux1->anterior->estacao][aux1->estacao] + 
                                 p.matriz_custo[aux1->estacao][aux1->proximo->estacao];
                custo_anterior += p.matriz_custo[aux2->anterior->estacao][aux2->estacao] + 
                                  p.matriz_custo[aux2->estacao][aux2->proximo->estacao];

                custo_swap = p.matriz_custo[aux1->anterior->estacao][aux2->estacao] + 
                             p.matriz_custo[aux2->estacao][aux1->proximo->estacao];
                custo_swap += p.matriz_custo[aux2->anterior->estacao][aux1->estacao] + 
                              p.matriz_custo[aux1->estacao][aux2->proximo->estacao];

                novo_custo = custo_swap - custo_anterior;

                if(novo_custo < melhor_custo){
                    // atualiza melhor custo
                    melhor_custo = novo_custo;

                    // guarda as estações
                    melhor_aux1 = aux1;
                    melhor_aux2 = aux2;
                }
                aux2 = aux2->proximo;
            }
            aux1 = aux1->proximo;
        }

        if(melhor_custo < 0){
            int temp = melhor_aux1->estacao;
            melhor_aux1->estacao = melhor_aux2->estacao;
            melhor_aux2->estacao = temp;

            r_ret.custo_total_d1 += melhor_custo;
        }
    }else{

        // começa do primeiro elemento, ignorando a garagem
        No* aux1 = r_ret.rota_i->anterior;

        if(!aux1) return r_ret;

        // termina quando chegar na garagem
        while(aux1->anterior){

            // mesma lógica que a anterior, porém começando do próximo de aux1
            No* aux2 = aux1->anterior;
            if(!aux2){
                // só há um elemento
                if(aux1 == r_ret.rota_f->anterior)
                    return r_ret;
                break;
            }
            
            while(aux2->anterior){

                custo_anterior = p.matriz_custo[aux1->proximo->estacao][aux1->estacao] + 
                                 p.matriz_custo[aux1->estacao][aux1->anterior->estacao];
                custo_anterior += p.matriz_custo[aux2->proximo->estacao][aux2->estacao] + 
                                  p.matriz_custo[aux2->estacao][aux2->anterior->estacao];

                custo_swap = p.matriz_custo[aux1->proximo->estacao][aux2->estacao] + 
                             p.matriz_custo[aux2->estacao][aux1->anterior->estacao];
                custo_swap += p.matriz_custo[aux2->proximo->estacao][aux1->estacao] + 
                              p.matriz_custo[aux1->estacao][aux2->anterior->estacao];

                novo_custo = custo_swap - custo_anterior;

                if(novo_custo < melhor_custo){
                    // atualiza melhor custo
                    melhor_custo = novo_custo;

                    // guarda as estações
                    melhor_aux1 = aux1;
                    melhor_aux2 = aux2;
                }
                aux2 = aux2->anterior;
            }
            aux1 = aux1->anterior;
        }

        if(melhor_custo < 0){
            int temp = melhor_aux1->estacao;
            melhor_aux1->estacao = melhor_aux2->estacao;
            melhor_aux2->estacao = temp;

            r_ret.custo_total_d2 += melhor_custo;
        }

    }

    return r_ret;
}

/* 
    Aplicar testes de re-insertion na rota e retornar a melhor rota encontrada 
    ou a mesma que recebeu, caso não haja melhora
 */
Rota VND_Re_Insertion(Rota const r){
    Rota r_ret = Rota();
    int melhor_custo = INFINITY;
    int custo_anterior, custo_reinsertion, novo_custo;
    No *melhor_aux1 = nullptr, *melhor_aux2 = nullptr;

    // cópia da rota
    {
        No* aux = r.rota_i->proximo;
        while(aux->proximo){
            r_ret.InsertEnd(aux->estacao);
            aux = aux->proximo;
        }
        r_ret.direcao_atual = r.direcao_atual;
        r_ret.custo_total_d1 = r.custo_total_d1;
        r_ret.custo_total_d2 = r.custo_total_d2;
        r_ret.rotaTam = r.rotaTam;
    }
    
    int i = 0; // mensurar onde seria a inserção
    if(r_ret.direcao_atual == DirecaoRota::INICIO_FIM){
        No* aux1 = r_ret.rota_i->proximo;
        if(!aux1) return r_ret;

        while(aux1->proximo){
            No* aux2 = aux1->proximo;

            if(!aux2){
                // se for um único elemento na rota
                if(aux1 == r_ret.rota_i->proximo)
                    return r_ret;
                break;
            }
            while(aux2->proximo){

                /*
                    Lógica de Re-Insertion
                    - custo anterior: custo do nó em sua posicao inicial + custo do lugar onde vai ser inserido
                    - novo custo: custo do lugar de onde o nó foi retirado + de onde foi inserido
                */

                custo_anterior = p.matriz_custo[aux1->anterior->estacao][aux1->estacao] +
                                 p.matriz_custo[aux1->estacao][aux1->proximo->estacao] +
                                 p.matriz_custo[aux2->estacao][aux2->proximo->estacao];
                

                custo_reinsertion = p.matriz_custo[aux1->anterior->estacao][aux1->proximo->estacao] +
                                    p.matriz_custo[aux2->estacao][aux1->estacao] + 
                                    p.matriz_custo[aux1->estacao][aux2->proximo->estacao];

                novo_custo = custo_reinsertion - custo_anterior;

                if(novo_custo < melhor_custo){


                    melhor_custo = novo_custo;
                    melhor_aux1 = aux1;
                    melhor_aux2 = aux2;
                }

                aux2 = aux2->proximo;
            }
            aux1 = aux1->proximo;
            i++;
        }

        // reinserção:
        if(melhor_custo < 0){
            // remover o aux1
            melhor_aux1->anterior->proximo = melhor_aux1->proximo;
            melhor_aux1->proximo->anterior = melhor_aux1->anterior;

            // inserir aux1 depois de aux2
            melhor_aux1->proximo = melhor_aux2->proximo;
            melhor_aux1->anterior = melhor_aux2;
            melhor_aux2->proximo->anterior = melhor_aux1;
            melhor_aux2->proximo = melhor_aux1;

            r_ret.custo_total_d1 += melhor_custo;
        }
    }else{

        No* aux1 = r_ret.rota_i->anterior;
        if(!aux1) return r_ret;

        while(aux1->anterior){
            No* aux2 = aux1->anterior;

            if(!aux2){
                // se for um único elemento na rota
                if(aux1 == r_ret.rota_i->anterior)
                    return r_ret;
                break;
            }
            while(aux2->anterior){

                /*
                    Lógica de Re-Insertion
                    - custo anterior: custo do nó em sua posicao inicial + custo do lugar onde vai ser inserido
                    - novo custo: custo do lugar de onde o nó foi retirado + de onde foi inserido
                */

                custo_anterior = p.matriz_custo[aux1->proximo->estacao][aux1->estacao] +
                                 p.matriz_custo[aux1->estacao][aux1->anterior->estacao] +
                                 p.matriz_custo[aux2->estacao][aux2->anterior->estacao];
                

                custo_reinsertion = p.matriz_custo[aux1->proximo->estacao][aux1->anterior->estacao] +
                                    p.matriz_custo[aux2->estacao][aux1->estacao] + 
                                    p.matriz_custo[aux1->estacao][aux2->anterior->estacao];

                novo_custo = custo_reinsertion - custo_anterior;

                if(novo_custo < melhor_custo){
                    melhor_custo = novo_custo;
                    melhor_aux1 = aux1;
                    melhor_aux2 = aux2;
                }

                aux2 = aux2->anterior;
            }
            aux1 = aux1->anterior;
        }

        // reinserção
        if(melhor_custo < 0){
            // remover o aux1
            melhor_aux1->proximo->anterior = melhor_aux1->anterior;
            melhor_aux1->anterior->proximo = melhor_aux1->proximo;

            // inserir aux1 depois de aux2
            melhor_aux1->anterior = melhor_aux2->anterior;
            melhor_aux1->proximo = melhor_aux2;
            melhor_aux2->anterior->proximo = melhor_aux1;
            melhor_aux2->anterior = melhor_aux1;

            r_ret.custo_total_d1 += melhor_custo;
        }
    }

    return r_ret;
}

Rota VND_TwoOpt(Rota const r){
    /* 
        Aplicar testes de 2-opt na rota e retornar a melhor rota encontrada 
        ou a mesma que recebeu, caso não haja melhora
     */
}

Solucao VND(Solucao solucao)
{
    int n_rotas = solucao.rotas.size();
    VND_attr attr[n_rotas];
    
    // inicializa os atributos de cada rota
    for(int i = 0; i < n_rotas; i++){
        attr[i].solucao = solucao;
        attr[i].i_rota = i;
        attr[i].rota_retorno = solucao.rotas[i];
    }
    
    // Para cada rota, cria uma thread de execução do VND
    for(int i = 0; i < n_rotas; ){
        // uma thread para cada attr[i]
    }

    // espera as rotas de cada thread
}

void* VND(void* atributos){
    /**
     * Movimentos de vizinhança escolhidos (constante NUM_VND_STRUCTURES define quantas são)
     * 1. Swap (Troca de dois elementos)
     * 2. Re-insertion (Mover um elemento para outra posição)
     * 3. 2-opt (Pegar uma fatia da solução e inverter)
     */

    VND_attr* attr = (VND_attr*) atributos;

    // criando um vetor de estruturas:
    vector<function<Rota(Rota)>> estruturas_VND = {
        VND_Swap,
        VND_Re_Insertion,
        VND_TwoOpt
    };

    // inicia percorrendo as estruturas da primeira (k = 0) até a ultima, while k <= NUM_VND_STRUCTURES
    int k = 0;

    // dependendo da direção, a condicional dentro do loop é diferente
    // para fins de otimização, faz-se a verificação uma única vez, fora do loop
    if(attr->solucao.rotas[attr->i_rota].direcao_atual == DirecaoRota::INICIO_FIM){
        // usa o custo_total na direcao d1
        while(k < NUM_VND_STRUCTURES){
            // chama uma estrutura de vizinhança k e retorna o valor para nova_rota
            Rota nova_rota = estruturas_VND[k](attr->rota_retorno);
            
            if(nova_rota.custo_total_d1 < attr->rota_retorno.custo_total_d1){
                k = 0;
                // essa atribuição ainda tem problemas. criar operador move na classe rota
                attr->rota_retorno = nova_rota; // move evita fazer a cópia da rota
            }else
                k++;
        }
    }
    else{
        // usa o custo_total na direcao d2
        while(k < NUM_VND_STRUCTURES){
            // chama uma estrutura de vizinhança k e retorna o valor para nova_rota
            Rota nova_rota = estruturas_VND[k](attr->rota_retorno);
            
            if(nova_rota.custo_total_d2 < attr->rota_retorno.custo_total_d2){
                k = 0;
                attr->rota_retorno = nova_rota;
            }else
                k++;
        }
    }

    return nullptr;
}