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
     * Instância de Problema que será utilizada por todas as funções que envolverem problema,
     * a fim de reduzir a empilhagem e desempilhagem do mesmo parametro múltiplas vezes.
     * Deve ser instânciada por meio da função `SetProblema`, usando como parâmetro um problema ou seus atributos
     */
    extern Problema p;

    /**
     * Função que inicializa a instância principal de problema
     * @param Problema que será copiado para a instância
     */
    void SetProblema(Problema prob){
        p = prob;
    }

    void SetProblema(
        int qnt_veiculos,
        int capacidade_max,
        vector<int> demandas,
        int qnt_estacoes,
        int veiculos_disponiveis,
        vector<vector<int>> matriz_custo){
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
    Problema GetProblema(){
        return p;
    }

    void PrintProblema(Problema prob){
        cout << "Número de estações: " << prob.qnt_estacoes << endl;
        cout << "Número de veículos: " << prob.qnt_veiculos << endl;
        cout << "Capacidade máxima: " << prob.capacidade_max << endl;
        cout << "Demandas: ";
        for(int i = 0; i < prob.qnt_estacoes; i++){
            cout << prob.demandas[i] << " ";
        }
        cout << endl;
        cout << "Matriz de custos: " << endl;
        for(int i = 0; i <= prob.qnt_estacoes; i++){
            for(int j = 0; j <= prob.qnt_estacoes; j++){
                cout << prob.matriz_custo[i][j] << " ";
            }
            cout << endl;
        }
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
    bool VerificaSolucao(const vector<Rota>& rotas, bool verbose = false){
        vector<bool> visitados(p.qnt_estacoes, false);
        int soma_demandas;

        /* Testa cada rota */
        int ind_rota = 0;
        for(const Rota& rota : rotas){
            soma_demandas = 0;

            /* Verifica se a rota começa e termina na garagem (0) */
            if(rota.estacoes[0] != 0 && rota.estacoes[rota.estacoes.size() - 1] != 0){
                if (verbose) 
                    cerr << "Rota [" << ind_rota <<"] nao começa ou termina na garagem." << endl;
                return false;
            }
            /* Percorre o miolo da lista dada, fazendo as devidas verifcações */
            if(rota.direcao_atual == DirecaoRota::INICIO_FIM){   

                for(size_t i = 1; i < rota.estacoes.size() - 1; i++){
                    int estacao = rota.estacoes[i];

                    /* Caso haja uma estação no meio da rota */
                    if(!estacao){
                        if (verbose)
                            cerr << "Rota ["<< ind_rota << "] com galpao fora do lugar" << endl;
                        return false;
                    }

                    /* Caso hajam estacoes repetidas */
                    if(visitados[estacao - 1]){
                        if (verbose)
                            cerr << "Estacao " << estacao << " visitada mais de uma vez: " << estacao << endl;
                        return false;
                    }
                    
                    /* Verifica se a demanda continua limitada à capacidade do caminhão */
                    soma_demandas += p.demandas[estacao - 1];
                    if(abs(soma_demandas) > p.capacidade_max){
                        if (verbose)
                            cerr << "Capacidade excedida na estacao: " << estacao << endl;
                        return false;
                    }

                    /* marca a estação como visitada */
                    visitados[estacao - 1] = true;
                }

            /* Caso a rota esteja na direção contrária: */
            }else{

                /* Percorre o miolo da lista dada, fazendo as devidas verifcações */
                for(size_t i = rota.estacoes.size() - 2; i >=0; i--){
                    int estacao = rota.estacoes[i];

                    /* Caso haja uma estação no meio da rota */
                    if(!estacao){
                        if (verbose)
                            cerr << "Rota ["<< ind_rota << "] com galpao fora do lugar" << endl;
                        return false;
                    }

                    /* Caso hajam estacoes repetidas */
                    if(visitados[estacao - 1]){
                        if (verbose)
                            cerr << "Estacao " << estacao << " visitada mais de uma vez: " << estacao << endl;
                        return false;
                    }
                    
                    /* Verifica se a demanda continua limitada à capacidade do caminhão */
                    soma_demandas += p.demandas[estacao - 1];
                    if(abs(soma_demandas) > p.capacidade_max){
                        if (verbose)
                            cerr << "Capacidade excedida na estacao: " << estacao << endl;
                        return false;
                    }
                    /* marca a estação como visitada */
                    visitados[estacao - 1] = true;
                }
            }

            ind_rota++;
        }

        /* Verifica se todas as estacoes foram devidamente alocadas nas rotas */
        for (size_t i = 0; i < visitados.size(); i++){
            if(!visitados[i]){
                if (verbose)
                    cerr << "Estação "<< visitados[i] <<" nao visitada: " << i + 1 << endl;
                return false;
            }
        }
    }

    /**
     *  @brief Retona um par de `Demanda` e `Custo` da rota dada;
     * 
     * ---
     *  @param rota define sobre que rota a função irá iterar
     *
     * ---
     *  @return `pair<int, int>` - que está no formato <demanda, custo>
     * */
    pair<int, int> Demanda_Custo_Rota(const Rota& rota){
        int demanda = 0;
        int custo = 0;
        int estacao_atual, estacao_prox;

        /* se a rota estiver na direção INICIO_FIM */ 
        if(rota.direcao_atual == INICIO_FIM){
            custo = p.matriz_custo[0][rota.estacoes[1]];
            /* varrendo as estações, acumula o custo e a demanda da rota */
            for(size_t i = 1; i < rota.estacoes.size()-1; i++){
                estacao_atual = rota.estacoes[i];
                estacao_prox = rota.estacoes[i + 1];

                demanda += p.demandas[estacao_atual - 1];
                custo += p.matriz_custo[estacao_atual][estacao_prox];
                if(abs(demanda) > p.capacidade_max){
                    return make_pair(-1, -1); // sinaliza que a demanda estourou
                }
            }
        } else {
            /* mesma lógica, porém no sentido oposto */
            custo = p.matriz_custo[0][rota.estacoes[rota.estacoes.size() - 2]];
            for(size_t i = rota.estacoes.size() - 2; i > 0; i--){
                estacao_atual = rota.estacoes[i];
                estacao_prox = rota.estacoes[i - 1];

                demanda += p.demandas[estacao_atual - 1];
                custo += p.matriz_custo[estacao_atual][estacao_prox];
                if(abs(demanda) > p.capacidade_max){
                    return make_pair(-1, -1); // sinaliza que a demanda estourou
                }
            }
        }
        
        return make_pair(demanda, custo);
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
    bool VerificaDemanda(const Rota& rota){
        int demanda = 0;

        /* Se a rota estiver na direção INICIO_FIM */
        if(rota.direcao_atual == INICIO_FIM){
            /* Para cada estação, exceto as garagens */
            for(size_t i = 1; i < rota.estacoes.size()-1; i++){
                /* acumula a demanda e verifica se esta repeita a capacidade máxima imposta */
                demanda += p.demandas[rota.estacoes[i] - 1];
                if (abs(demanda) > p.capacidade_max) {
                    /* Retorna falso, caso exceda a capacidade */
                    return false;
                }
            }
        } else {
            /* mesma lógica, percorrendo ao contrário */
            for(size_t i = rota.estacoes.size() - 2; i > 0; i--){
                demanda += p.demandas[rota.estacoes[i] - 1];
                if (abs(demanda) > p.capacidade_max) {
                    return false;
                }
            }
        }
        /* retorna verdadeiro caso a demanda da rota esteja válida */
        return true;
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
    bool VerificaDemandas(const vector<Rota>& rotas){
        /* Para cada rota, aplica a função que veriifca individualmente */
        for(const Rota& rota : rotas){
            if(!VerificaDemanda(rota)){
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
    pair<int, int> MelhoresVertices(){
        int melhor_1, melhor_2;
        int dist1, dist2, maior_dist = 0;

        /* Para cada par de estações */
        for (int i = 1; i <= p.qnt_estacoes; i++){
            for (int j = i+1; j <= p.qnt_estacoes; j++){
                
                /* Ignora caso demandas excedam a capacidade especificada */
                if(abs(p.demandas[i-1] + p.demandas[j-1]) > p.capacidade_max){
                    continue;
                }

                /* Verifica em ambas as direções */
                dist1 = p.matriz_custo[0][i] + p.matriz_custo[i][j] + p.matriz_custo[j][0];
                dist2 = p.matriz_custo[0][j] + p.matriz_custo[j][i] + p.matriz_custo[i][0];

                /* Se a distância 1 for melhor, salva as estações e atualiza a melhor distância */
                if(dist1 > maior_dist){
                    melhor_1 = i;
                    melhor_2 = j;
                    maior_dist = dist1;
                }
                /* se a distância 2 for melhor, salva as estações de forma invertida e atualiza a melhor distância*/
                if(dist2 > maior_dist){
                    melhor_1 = j;
                    melhor_2 = i;
                    maior_dist = dist2;
                }
            }
        }

        /* retorna o melhor par */
        return make_pair(melhor_1, melhor_2);
    }

    bool VerificaNovaDemanda(Rota& rota, int ind_antigo, int ind_novo){
        if(ind_antigo == ind_novo)
            return true; 

        int soma_demandas = 0;
        
        for(int i = 1; i < ind_novo; i++){
            if(i == ind_antigo) continue;
            soma_demandas += p.demandas[rota.estacoes[i] - 1];
            if(abs(soma_demandas) > p.capacidade_max)
                return false;
        }

        soma_demandas += p.demandas[rota.estacoes[ind_antigo] - 1];
        if(abs(soma_demandas) > p.capacidade_max)
            return false;

        for(int i = ind_novo; i < rota.estacoes.size() - 1; i++){
            if(i == ind_antigo) continue;
            soma_demandas += p.demandas[rota.estacoes[i] - 1];
            if(abs(soma_demandas) > p.capacidade_max)
                return false;
        }

        return true;
    }


    bool VerificaNovoElemento(Rota& rota, int posicao, int estacao){
        int soma_demandas = 0;

        for(int i = 1; i < posicao; i++){
            soma_demandas += p.demandas[rota.estacoes[i] - 1];
        }

        soma_demandas += p.demandas[estacao - 1];
        if (abs(soma_demandas) > p.capacidade_max)
            return false;
        
        for(int i = posicao; i < rota.estacoes.size() - 1; i++){
            soma_demandas += p.demandas[rota.estacoes[i] - 1];
            if (abs(soma_demandas) > p.capacidade_max)
                return false;
        }

        return true;
    }

    int CustoTotal(vector<Rota>& rotas){
        int custo_total = 0;

        for(Rota& rota : rotas){
            for(size_t i = 0; i < rota.estacoes.size() - 1; i++){
                custo_total += p.matriz_custo[rota.estacoes[i]][rota.estacoes[i + 1]];
            }
        }

        return custo_total;
    }

    int SomaCusto(Rota& rota){
        int custo = 0;

        for(size_t i = 0; i < rota.estacoes.size() - 1; i++){
            custo += p.matriz_custo[rota.estacoes[i]][rota.estacoes[i + 1]];
        }

        return custo;
    }

    int RandomEntre(int min, int max) {
        static random_device rd;   
        static mt19937 gen(rd());  
        uniform_int_distribution<> dis(min, max);
        return dis(gen);
    }

#endif