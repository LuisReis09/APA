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

    
    Solucao VizinhoMaisProximo(){
        vector<vector<int>> rotas(p.qnt_veiculos, vector<int>(1, 0)); // inicia todas as rotas com o depósito
        int custo_total = 0;
        int qtd_iteracoes = p.qnt_estacoes;

        /*
            Para cada estacao, guarda:
            [
                1: [1: 0, 2: 10, 3: 15 ...] // custos para cada estacao
                2: [1: 5, 2: 0, 3: 20 ...]  // custos para cada estacao
                ...
            ]
        */
        typedef vector<vector<pair<int, int>>> FilaPrioridade;
        FilaPrioridade fila_prioridade(p.qnt_estacoes + 1);

        for(int i=0; i<(p.qnt_estacoes+1); i++){
            vector<pair<int, int>> custos(p.qnt_estacoes);
            for(int j=0; j<p.qnt_estacoes; j++){
                custos[j] = make_pair(j+1, p.matriz_custo[i][j+1]);
            }

            sort(custos.begin(), custos.end(),
                [](const pair<int, int>& a, const pair<int, int>& b) {
                    return a.second < b.second;
                });

            fila_prioridade[i] = custos;
        }

        while(qtd_iteracoes--){
            int melhor_rota, melhor_custo = INT_MAX, melhor_estacao;

            for(int i=0; i<rotas.size(); i++){
                int ultimo_elemento = rotas[i].back();

                for(int j=0; j<fila_prioridade[ultimo_elemento].size(); j++){
                    int estacao = fila_prioridade[ultimo_elemento][j].first;
                    int custo = fila_prioridade[ultimo_elemento][j].second;

                    bool pode_inserir = TestaRota(rotas[i], estacao);
                    if(pode_inserir){
                        if(custo < melhor_custo){
                            melhor_custo = custo;
                            melhor_rota = i;
                            melhor_estacao = estacao;
                        }
                        break; // nao adianta olhar as outras, pq serao mais caras
                    }
                }
            }


            // Agora que temos a melhor rota e a melhor estação, inserimos
            rotas[melhor_rota].push_back(melhor_estacao);
            custo_total += melhor_custo;

            // Remove a estação escolhida das filas de prioridades
            for(int i=0; i<fila_prioridade.size(); i++){
                for(int j=0; j < fila_prioridade[i].size(); j++){
                    if(fila_prioridade[i][j].first == melhor_estacao){
                        fila_prioridade[i].erase(fila_prioridade[i].begin() + j);
                        break;
                    }
                }
            }
        }

        Solucao solucao;
        for(int i=0; i<rotas.size(); i++){
            if(rotas[i].size() <= 1) break;
            
            custo_total += p.matriz_custo[rotas[i].back()][0];
            rotas[i].push_back(0); // volta para o depósito
            solucao.rotas.push_back(rotas[i]);
        }
        solucao.custo_total = custo_total;
        solucao.veiculos_usados = solucao.rotas.size();
        solucao.veiculos_disponiveis = p.qnt_veiculos - solucao.veiculos_usados;
        return solucao;
    }

    void MelhorarSolucao(vector<vector<int>>& rotas){
        int custo_remocao, custo_insercao;
        int estacao_origem, estacao_destino;

        
        for(int id_rota_origem=0; id_rota_origem < rotas.size(); id_rota_origem++){
            for(int id_estacao_origem = 1; id_estacao_origem < (rotas[id_rota_origem].size() - 1); id_estacao_origem++){
                estacao_origem = rotas[id_rota_origem][id_estacao_origem];
                
                
                for(int id_rota_destino=0; id_rota_destino < rotas.size(); id_rota_destino++){
                    for(int id_estacao_destino = 1; id_estacao_destino < rotas[id_rota_destino].size(); id_estacao_destino++){
                        estacao_destino = rotas[id_rota_destino][id_estacao_destino];

                        
                        if(id_rota_destino == id_rota_origem){
                            if(estacao_origem == estacao_destino || estacao_origem == rotas[id_rota_destino][id_estacao_destino - 1]){
                                continue;
                            }
                        }
                        
                        
                        custo_remocao = -p.matriz_custo[rotas[id_rota_origem][id_estacao_origem - 1]][estacao_origem];
                        custo_remocao -= p.matriz_custo[estacao_origem][rotas[id_rota_origem][id_estacao_origem + 1]];
                        custo_remocao += p.matriz_custo[rotas[id_rota_origem][id_estacao_origem - 1]][rotas[id_rota_origem][id_estacao_origem + 1]];
                        
                        
                        
                        custo_insercao =  -p.matriz_custo[rotas[id_rota_destino][id_estacao_destino - 1]][estacao_destino];
                        custo_insercao +=  p.matriz_custo[rotas[id_rota_destino][id_estacao_destino - 1]][estacao_origem];
                        custo_insercao +=  p.matriz_custo[estacao_origem][estacao_destino];

                        int delta = custo_remocao + custo_insercao;
                        if(delta < 0){

                            if(id_rota_origem == id_rota_destino){
                                if(ReinsertionTest(rotas[id_rota_origem], id_estacao_origem, id_estacao_destino)){
                                    // Reposiciona o elemento estacao_origem 

                                    rotas[id_rota_origem].erase(rotas[id_rota_origem].begin() + id_estacao_origem);
                                    if(id_estacao_origem < id_estacao_destino){
                                        rotas[id_rota_origem].insert(rotas[id_rota_origem].begin() + id_estacao_destino - 1, estacao_origem);
                                    }else{
                                        rotas[id_rota_origem].insert(rotas[id_rota_origem].begin() + id_estacao_destino, estacao_origem);
                                    }
                                }
                            }else{
                                if(RemovalTest(rotas[id_rota_origem], id_estacao_origem) && InsertionTest(rotas[id_rota_destino], id_estacao_destino, estacao_origem)){
                                    // Retira o elemento estacao_origem da rota_origem, colocando-o na rota_destino

                                    rotas[id_rota_origem].erase(rotas[id_rota_origem].begin() + id_estacao_origem);
                                    rotas[id_rota_destino].insert(rotas[id_rota_destino].begin() + id_estacao_destino, estacao_origem);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void PerturbacaoSwitch(vector<vector<int>>& rotas, int trocar_a_realizar){
        int qtd_rotas = rotas.size();
        int id_rota_origem, id_rota_destino;
        int pos1, pos2;

        id_rota_origem = id_rota_destino = 0;
        while(trocar_a_realizar--){
            if(qtd_rotas > 1){
                id_rota_origem = rand() % qtd_rotas;
                id_rota_destino = rand() % qtd_rotas;
            }

            // O range deve ser [1, rota.size() - 1], para nao incluir o galpao do inicio e do fim
            pos1 = rand() % (rotas[id_rota_origem].size()  -2);
            pos2 = rand() % (rotas[id_rota_destino].size() -2);

            // Para evitar de ser índice 0, onde esta o galpao
            pos1++;
            pos2++;

            swap(rotas[id_rota_origem][pos1], rotas[id_rota_destino][pos2]);
        }
    }    

    void PerturbacaoRelocate(
        vector<Rota>& rotas,
        int trocas_a_realizar,
        int qtd_el_trocados,
        bool reverso = false
    ) {
        while (trocas_a_realizar--) {
            int qtd_rotas = rotas.size();
            int id_rota_origem = rand() % qtd_rotas;
            int id_rota_destino = rand() % qtd_rotas;

            Rota& rota_origem = rotas[id_rota_origem];
            Rota& rota_destino = rotas[id_rota_destino];

            // precisa de espaço pra remover
            if ((int)rota_origem.estacoes.size() <= qtd_el_trocados + 2) {
                continue;
            }

            // intervalo válido de retirada (não inclui depósitos)
            int id_max = (int)rota_origem.estacoes.size() - 1 - qtd_el_trocados;
            int indice_retirada_inicio = RandomEntre(1, id_max - 1);
            int indice_retirada_final = indice_retirada_inicio + qtd_el_trocados - 1;

            // índice de inserção válido (não depósitos)
            int limite_insercao = (int)rota_destino.estacoes.size() - 1;
            if (limite_insercao <= 1) continue;
            int indice_onde_colocar = RandomEntre(1, limite_insercao - 1);

            // pega o bloco
            vector<int> bloco;
            for (int i = indice_retirada_inicio; i <= indice_retirada_final; i++) {
                bloco.push_back(rota_origem.estacoes[i]);
            }

            if (reverso) {
                reverse(bloco.begin(), bloco.end());
            }

            // remove no origem (sempre do mesmo índice, pq a cada remoção a lista encurta)
            for (int i = indice_retirada_inicio; i <= indice_retirada_final; i++) {
                rota_origem.RemoveAt(indice_retirada_inicio);
            }

            // se origem == destino e a remoção foi antes da inserção, ajusta
            if (id_rota_origem == id_rota_destino &&
                indice_onde_colocar > indice_retirada_inicio) {
                indice_onde_colocar -= bloco.size();
            }

            // insere no destino
            for (int i = 0; i < (int)bloco.size(); i++) {
                rota_destino.InsertAt(indice_onde_colocar + i, bloco[i]);
            }

            // remove rota inútil (só depósitos)
            if ((int)rota_origem.estacoes.size() < 3) {
                rotas.erase(rotas.begin() + id_rota_origem);
            }
        }
    }

    /**
     * @brief Perturbação que cria uma nova rota, movendo elementos de outras rotas para ela
     * Os alvos serao estacoes aleatorias, de outras rotas tambem aleatorias
     * ---
     * @param rotas Array de rotas que serao perturbadas
     * @param qtd_elementos Quantidade de elementos a serem movidos para a nova rota
     */
    void PerturbacaoNewRoute(vector<vector<int>>& rotas, int qtd_elementos){
        // Tal perturbacao nao eh possivel se ja estivermos usando todos os veiculos
        if(p.qnt_veiculos <= rotas.size())
            return;

        vector<int> elementos_retirados(qtd_elementos);
        int aux = 0;

        while(qtd_elementos--){
            int id_rota_origem = rand() % rotas.size();
            if (rotas[id_rota_origem].size() < 3){
                qtd_elementos++; // tenta de novo
                continue;
            }
            int id_max = rotas[id_rota_origem].size() - 2;

            if(id_max < 1) id_max = 1;

            int indice_retirada = RandomEntre(1, id_max);
            elementos_retirados[aux++] = rotas[id_rota_origem][indice_retirada];
            rotas[id_rota_origem].erase(rotas[id_rota_origem].begin() + indice_retirada);

            // Se a rota ficar sem elementos, remove ela
            if(rotas[id_rota_origem].size() < 3){
                rotas.erase(rotas.begin() + id_rota_origem);
            }
        }

        // Cria a nova rota
        vector<int> nova_rota;
        nova_rota.push_back(0); // depósito inicial
        for(int i=0; i<aux; i++){
            nova_rota.push_back(elementos_retirados[i]);
        }
        nova_rota.push_back(0); // depósito final
        rotas.push_back(nova_rota);
    }

    void PerturbacaoHalfSwap(vector<vector<int>>& rotas, bool reverso = false){
        int rota_alvo;
        
        do{
            rota_alvo = rand() % rotas.size();
        }while(rotas[rota_alvo].size() <= 3); // garante que a rota tenha ao menos 2 elementos para trocar
        
        int metade = 1 + (rotas[rota_alvo].size() - 2) / 2;

        if(!reverso){
            int i = 1, j = metade ;
            
            while (i < metade) {
                swap(rotas[rota_alvo][i], rotas[rota_alvo][j]);
                i++;
                j++;
            }
        }else{
            int i = 1, j = rotas[rota_alvo].size() - 2;

            while (i < j) {
                swap(rotas[rota_alvo][i], rotas[rota_alvo][j]);
                i++;
                j--;
            }
        }
    }

    void PerturbacaoHalfSwapRoutes(vector<vector<int>>& rotas){
        if(rotas.size() < 2) return; // precisa de ao menos 2 rotas

        int rota1, rota2;
        do{
            rota1 = rand() % rotas.size();
            rota2 = rand() % rotas.size();
        }while(rota1 == rota2 || rotas[rota1].size() <= 3 || rotas[rota2].size() <= 3);

        int metade1 = 1 + (rotas[rota1].size() - 2) / 2;
        int metade2 = 1 + (rotas[rota2].size() - 2) / 2;

        int i = 1, j = 1;

        while (i < metade1 && j < metade2) {
            swap(rotas[rota1][i], rotas[rota2][j]);
            i++;
            j++;
        }
    }


    void Perturbar(vector<vector<int>>& rotas, int opcao, int nivel_perturbacao){
        switch (opcao)
        {
            case 1:
                PerturbacaoSwitch(rotas, 1 < nivel_perturbacao);
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
            // case 6:
            //     PerturbacaoRelocate(rotas, 2 * nivel_perturbacao, 1, false);
            //     break;
            // case 7:
            //     PerturbacaoRelocate(rotas, 2 * nivel_perturbacao, 1, true);
            //     break;
            default:
                break;
        }
    }

    void ILS(vector<vector<int>>& rotas, int max_iteracoes = 10000, int max_sem_melhora = 1000){
        srand(time(NULL));
        int iteracoes = 0, sem_melhora = 0;
        int melhor_custo = CustoTotal(rotas), custo_teste;

        while(iteracoes < max_iteracoes && sem_melhora < max_sem_melhora){
            int opcao_perturbacao = 1 + rand() % 5;
            int nivel_perturbacao = (sem_melhora / (max_sem_melhora / 7)) + 1;

            vector<vector<int>> rotas_copia = rotas;
            Perturbar(rotas_copia, opcao_perturbacao, nivel_perturbacao);
            MelhorarSolucao(rotas_copia);

            custo_teste = CustoTotal(rotas_copia);
            // cout << " - Custo teste: " << custo_teste << endl;

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