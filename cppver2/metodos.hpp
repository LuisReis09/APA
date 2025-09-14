#ifndef _METODOS_HPP_
#define _METODOS_HPP_

    #include <iostream>
    #include <stdlib.h>
    #include <vector>
    #include "structures.hpp"
    #include "utils.hpp"
    #include <time.h>
    #include <map>
    #include <algorithm>

    #define INT_MAX 2147483647

    using namespace std;

    /**
     *  @brief `Algoritmo Guloso` de vizinho
     *
     *  ---
     *  @param rota será verificada na função
     *
     *  ---
     *  @return `bool` - Indica se a demanda de uma rota é válida, baseando-se no `Problema p`
     * */
    Solucao VizinhoMaisProximo() {
        vector<vector<int>> rotas(p.qnt_veiculos, vector<int>(1, 0)); // inicia todas as rotas com o depósito
        vector<int> necessidades(p.qnt_veiculos, 0);
        vector<int> restam_visitar(p.qnt_estacoes);

        for(int i=0; i<p.qnt_estacoes; i++){
            restam_visitar[i] = i+1;
        }

        typedef vector<pair<int, int>> FilaPrioridade;

        vector<FilaPrioridade> filas_prioridade(p.qnt_veiculos);

        FilaPrioridade modelo_inicial(p.qnt_estacoes);
        for(int i=0; i<p.qnt_estacoes; i++){
            modelo_inicial[i] = make_pair(i+1, p.matriz_custo[0][i+1]);
        }

        sort(modelo_inicial.begin(), modelo_inicial.end(),
            [](const pair<int, int>& a, const pair<int, int>& b) {
                return a.second < b.second;
            });

        for(int i=0; i<p.qnt_veiculos; i++){
            filas_prioridade[i] = modelo_inicial;
        }

        while(restam_visitar.size()){
            // Primeiro descobrimos a melhor rota para inserir
            int menor_custo = INT_MAX;
            int melhor_rota;

            for(int i=0; i<filas_prioridade.size(); i++){
                if(filas_prioridade[i].size() == 0) continue;

                if(filas_prioridade[i][0].second < menor_custo){
                    menor_custo = filas_prioridade[i][0].second;
                    melhor_rota = i;
                }
            }

            // Agora descobrimos a melhor estação para inserir
            int melhor_estacao = filas_prioridade[melhor_rota][0].first;
            int custo_ao_inserir = filas_prioridade[melhor_rota][0].second;

            rotas[melhor_rota].push_back(melhor_estacao);
            necessidades[melhor_rota] += p.demandas[melhor_estacao - 1];

            // Remove a estação do array de restam_visitar
            restam_visitar.erase(remove(restam_visitar.begin(), restam_visitar.end(), melhor_estacao), restam_visitar.end());

            // Para a rota que acabou de inserir, atualiza a fila de prioridades
            for(auto it = filas_prioridade[melhor_rota].begin(); it != filas_prioridade[melhor_rota].end(); ){
                if(it->first == melhor_estacao || abs(necessidades[melhor_rota] + p.demandas[it->first - 1]) > p.capacidade_max){
                    it = filas_prioridade[melhor_rota].erase(it);
                }else{
                    it->second = p.matriz_custo[melhor_estacao][it->first];
                    it++;
                }
            }

            sort(filas_prioridade[melhor_rota].begin(), filas_prioridade[melhor_rota].end(),
                [](const pair<int, int>& a, const pair<int, int>& b) {
                    return a.second < b.second;
                });
            
            // Remove a estação escolhida das outras filas de prioridade
            for(int i=0; i<filas_prioridade.size(); i++){
                if(i == melhor_rota) continue;

                for(auto it = filas_prioridade[i].begin(); it != filas_prioridade[i].end(); ){
                    if(it->first == melhor_estacao){
                        it = filas_prioridade[i].erase(it);
                        break;
                    }else{
                        it++;
                    }
                }
            }
        }

        Solucao solucao;
        solucao.custo_total = 0;
        solucao.veiculos_usados = 0;
        for (int i=0; i<rotas.size(); i++){
            if(rotas[i].size() <= 1) break;

            solucao.rotas.push_back(Rota());
            Rota& r = solucao.rotas.back(); // pega referência para a rota recém-criada

            for (int j = 1; j < rotas[i].size(); j++) {
                r.InsertEnd(rotas[i][j]);
                solucao.custo_total += p.matriz_custo[rotas[i][j-1]][rotas[i][j]];
            }
            solucao.custo_total += p.matriz_custo[rotas[i][rotas[i].size() - 1]][0];
            solucao.veiculos_usados++;
        }
        solucao.veiculos_disponiveis = p.qnt_veiculos - solucao.veiculos_usados;
        return solucao;
    }

    void MelhorarSolucao(vector<Rota>& rotas){
        int ind_rota_origem, ind_rota_destino;
        int custo_remocao, custo_insercao;
        int estacao_a_mover;
        bool melhorou = true;
        int iter = 0;

        while(melhorou && iter < 500){
            melhorou = false;
            iter++;

            ind_rota_origem = -1;
            for(Rota& rota_origem : rotas){
                ind_rota_origem++;
    
                for(int ind_estacao_a_mover = 1; ind_estacao_a_mover < rota_origem.estacoes.size() -1; ind_estacao_a_mover++){
                    estacao_a_mover = rota_origem.estacoes[ind_estacao_a_mover];
                    ind_rota_destino = -1;
    
                    for(Rota& rota_destino : rotas){
                        ind_rota_destino++;
    
                        for(int i=1; i<rota_destino.estacoes.size() - 1; i++){
    
                            int estacao_destino = rota_destino.estacoes[i];
                            int anterior_destino = rota_destino.estacoes[i - 1];

                            if (estacao_a_mover == estacao_destino || estacao_a_mover == anterior_destino)
                                continue; // não insere na mesma posição ou adjacente
    
                            custo_remocao = - p.matriz_custo[rota_origem.estacoes[ind_estacao_a_mover - 1]][rota_origem.estacoes[ind_estacao_a_mover]]
                                            - p.matriz_custo[rota_origem.estacoes[ind_estacao_a_mover]][rota_origem.estacoes[ind_estacao_a_mover + 1]]
                                            + p.matriz_custo[rota_origem.estacoes[ind_estacao_a_mover - 1]][rota_origem.estacoes[ind_estacao_a_mover + 1]];
    
                            custo_insercao = + p.matriz_custo[anterior_destino][estacao_a_mover]
                                             + p.matriz_custo[estacao_a_mover][estacao_destino]
                                             - p.matriz_custo[anterior_destino][estacao_destino];
    
                            if((custo_insercao + custo_remocao) < 0){

                                if(ind_rota_destino == ind_rota_origem){
                                    Rota copia = rota_origem; // faz uma copia temporaria
                                    copia.RemoveAt(ind_estacao_a_mover);
                                    copia.InsertAt(i, estacao_a_mover);
                                    if(VerificaDemanda(copia)){
                                        rota_origem = copia;
                                        // melhorou = true;
                                        goto fim_iteracao;
                                    }
    
                                }else{
                                    Rota copia_origem = rota_origem;  
                                    Rota copia_destino = rota_destino;
    
                                    copia_origem.RemoveAt(ind_estacao_a_mover);
                                    copia_destino.InsertAt(i, estacao_a_mover);

                                    if(VerificaDemanda(copia_origem) && VerificaDemanda(copia_destino)){
                                        rota_origem = copia_origem;
                                        rota_destino = copia_destino;
                                        // melhorou = true;
                                        goto fim_iteracao;
                                    }
                                }
    
                            }
                        }
                    }
                }
            }

            fim_iteracao: ;
        }

        rotas.erase(remove_if(rotas.begin(), rotas.end(),
                              [](const Rota& r){ return r.estacoes.size() < 3; }),
                    rotas.end());
    }

    void PerturbacaoSwitch(vector<Rota>& rotas, int trocar_a_realizar){
        int qtd_rotas = rotas.size();
        int id_rota_origem, id_rota_destino;
        int pos1, pos2;

        while(trocar_a_realizar--){
            if(qtd_rotas > 1){
                id_rota_origem = rand() % qtd_rotas;
                id_rota_destino = rand() % qtd_rotas;
            }else{
                id_rota_origem = id_rota_destino = 0;
            }

            // O range deve ser [1, rota.size() - 1], para nao incluir o galpao do inicio e do fim
            pos1 = rand() % (rotas[id_rota_origem].estacoes.size()  -1);
            pos2 = rand() % (rotas[id_rota_destino].estacoes.size() -1);

            // Para evitar de ser índice 0, onde esta o galpao
            pos1++;
            pos2++;

            swap(rotas[id_rota_origem].estacoes[pos1], rotas[id_rota_destino].estacoes[pos2]);
        }
    }    

    // void PerturbacaoRelocate(
    //     vector<Rota>& rotas,
    //     int trocas_a_realizar,
    //     int qtd_el_trocados,
    //     bool reverso = false
    // ) {
    //     while (trocas_a_realizar-- && !rotas.empty()) {
    //         int qtd_rotas = rotas.size();
    //         int id_rota_origem = rand() % qtd_rotas;
    //         int id_rota_destino = rand() % qtd_rotas;

    //         Rota& rota_origem = rotas[id_rota_origem];
    //         Rota& rota_destino = rotas[id_rota_destino];

    //         // precisa de espaço pra remover
    //         if ((int)rota_origem.estacoes.size() <= qtd_el_trocados + 2) {
    //             continue;
    //         }

    //         // intervalo válido de retirada (não inclui depósitos)
    //         int id_max = (int)rota_origem.estacoes.size() - 1 - qtd_el_trocados;
    //         int indice_retirada_inicio = RandomEntre(1, id_max - 1);
    //         int indice_retirada_final = indice_retirada_inicio + qtd_el_trocados - 1;

    //         // índice de inserção válido (não depósitos)
    //         int limite_insercao = (int)rota_destino.estacoes.size() - 1;
    //         if (limite_insercao <= 1) continue;
    //         int indice_onde_colocar = RandomEntre(1, limite_insercao - 1);

    //         // pega o bloco
    //         vector<int> bloco;
    //         for (int i = indice_retirada_inicio; i <= indice_retirada_final; i++) {
    //             bloco.push_back(rota_origem.estacoes[i]);
    //         }

    //         if (reverso) {
    //             reverse(bloco.begin(), bloco.end());
    //         }

    //         // remove no origem (sempre do mesmo índice, pq a cada remoção a lista encurta)
    //         for (int i = indice_retirada_inicio; i <= indice_retirada_final; i++) {
    //             rota_origem.RemoveAt(indice_retirada_inicio);
    //         }

    //         // se origem == destino e a remoção foi antes da inserção, ajusta
    //         if (id_rota_origem == id_rota_destino &&
    //             indice_onde_colocar > indice_retirada_inicio) {
    //             indice_onde_colocar -= bloco.size();
    //         }

    //         // insere no destino
    //         for (int i = 0; i < (int)bloco.size(); i++) {
    //             rota_destino.InsertAt(indice_onde_colocar + i, bloco[i]);
    //         }

    //         // remove rota inútil (só depósitos)
    //         if ((int)rota_origem.estacoes.size() < 3) {
    //             rotas.erase(rotas.begin() + id_rota_origem);
    //         }
    //     }
    // }

    /**
     * @brief Perturbação que cria uma nova rota, movendo elementos de outras rotas para ela
     * Os alvos serao estacoes aleatorias, de outras rotas tambem aleatorias
     * ---
     * @param rotas Array de rotas que serao perturbadas
     * @param qtd_elementos Quantidade de elementos a serem movidos para a nova rota
     */
    void PerturbacaoNewRoute(vector<Rota>& rotas, int qtd_elementos){
        // Tal perturbacao nao eh possivel se ja estivermos usando todos os veiculos
        if(p.qnt_veiculos <= rotas.size())
            return;

        vector<int> elementos_retirados(qtd_elementos);
        qtd_elementos = min(qtd_elementos, (int)(p.qnt_estacoes - rotas.size())); // garante um teto maximo
        int aux = 0;

        while(qtd_elementos--){
            int id_rota_origem = rand() % rotas.size();
            int id_max = rotas[id_rota_origem].estacoes.size() - 2;

            if(id_max < 1) id_max = 1;

            int indice_retirada = RandomEntre(1, id_max);
            elementos_retirados[aux++] = rotas[id_rota_origem].estacoes[indice_retirada];
            rotas[id_rota_origem].RemoveAt(indice_retirada);

            // Se a rota ficar sem elementos, remove ela
            if(rotas[id_rota_origem].estacoes.size() < 3){
                rotas.erase(rotas.begin() + id_rota_origem);
            }
        }

        // Cria a nova rota
        Rota nova_rota;
        for(int i=0; i<aux; i++){
            nova_rota.InsertEnd(elementos_retirados[i]);
        }
        rotas.push_back(nova_rota);
    }

    void PerturbacaoHalfSwap(vector<Rota>& rotas, bool reverso = false){
        int rota_alvo;
        
        do{
            rota_alvo = rand() % rotas.size();
        }while(rotas[rota_alvo].estacoes.size() <= 3); // garante que a rota tenha ao menos 2 elementos para trocar
        
        int metade = 1 + (rotas[rota_alvo].estacoes.size() - 2) / 2;

        if(!reverso){
            int i = 1, j = metade ;
            
            while (i < metade) {
                swap(rotas[rota_alvo].estacoes[i], rotas[rota_alvo].estacoes[j]);
                i++;
                j++;
            }
        }else{
            int i = 1, j = rotas[rota_alvo].estacoes.size() - 2;

            while (i < j) {
                swap(rotas[rota_alvo].estacoes[i], rotas[rota_alvo].estacoes[j]);
                i++;
                j--;
            }
        }
    }

    void PerturbacaoHalfSwapRoutes(vector<Rota>& rotas){
        if(rotas.size() < 2) return; // precisa de ao menos 2 rotas

        int rota1, rota2;
        do{
            rota1 = rand() % rotas.size();
            rota2 = rand() % rotas.size();
        }while(rota1 == rota2 || rotas[rota1].estacoes.size() <= 3 || rotas[rota2].estacoes.size() <= 3);

        int metade1 = 1 + (rotas[rota1].estacoes.size() - 2) / 2;
        int metade2 = 1 + (rotas[rota2].estacoes.size() - 2) / 2;

        int i = 1, j = 1;

        while (i < metade1 && j < metade2) {
            swap(rotas[rota1].estacoes[i], rotas[rota2].estacoes[j]);
            i++;
            j++;
        }
    }


    void Perturbar(vector<Rota>& rotas, int opcao, int nivel_perturbacao){
        switch (opcao)
        {
            case 1:
                PerturbacaoSwitch(rotas, 3*nivel_perturbacao);
                break;
            case 2:
                PerturbacaoNewRoute(rotas, 2 * nivel_perturbacao);
                break;
            case 3:
                PerturbacaoHalfSwap(rotas);
                break;
            case 4:
                PerturbacaoHalfSwap(rotas, true);
                break;
            case 5:
                PerturbacaoHalfSwapRoutes(rotas);
                break;
            default:
                break;
        }
    }

    void ILS(vector<Rota>& rotas, int max_iteracoes = 2000, int max_sem_melhora = 200){
        srand(time(NULL));
        int iteracoes = 0, sem_melhora = 0;
        int melhor_custo = CustoTotal(rotas), custo_teste;

        while(iteracoes < max_iteracoes && sem_melhora < max_sem_melhora){
            int opcao_perturbacao = 1 + rand() % 5;
            int nivel_perturbacao = (sem_melhora / (max_sem_melhora / 5)) + 1;

            vector<Rota> rotas_copia = rotas;
            Perturbar(rotas_copia, opcao_perturbacao, nivel_perturbacao);
            MelhorarSolucao(rotas_copia);

            custo_teste = CustoTotal(rotas_copia);

            if (custo_teste < melhor_custo && VerificaDemandas(rotas_copia)){
                rotas = move(rotas_copia);
                melhor_custo = custo_teste;
                sem_melhora = 0;
                cout << "Melhora! Novo custo: " << melhor_custo << endl;
            }else{
                sem_melhora++;
            }

            iteracoes++;
        }
    }

#endif