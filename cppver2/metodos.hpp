#ifndef _METODOS_HPP_
#define _METODOS_HPP_

    #include <iostream>
    #include <stdlib.h>
    #include <vector>
    #include "structures.hpp"
    #include "utils.hpp"
    #include "threadpool.hpp"
    #include <time.h>
    #include <map>
    #include <functional>
    #include <random>
    #include <algorithm>

    #define MAXX_INT 2147483647

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
            int melhor_rota, melhor_custo = MAXX_INT, melhor_estacao;

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
    
    Solucao InsercaoMaisBarata()
    {
        vector<int> estacoes_restantes(p.qnt_estacoes);
        for (int i = 1; i <= p.qnt_estacoes; i++)
        {
            estacoes_restantes[i - 1] = i;
        }

        pair<int, int> vertices = MelhoresVertices();
        vector<vector<int>> rotas = {{0, vertices.first, vertices.second, 0}};

        cout << "chegou aqui" << endl;

        int caminhoes_restantes = p.qnt_veiculos;

        for (int estacao : estacoes_restantes)
        {
            int menor_custo = MAXX_INT; // Menor custo possivel para aquela
            int index_rota = 0;
            int index_rota_salva;
            vector<int> rota_salva;
            vector<vector<int>> rotas_copia = rotas;

            for (vector<int> rota : rotas_copia)
            {
                LinkedList vetorLL = transformarEmLinkedList(rota);
                for (int index_ll = 1; index_ll < vetorLL.rotaTam; index_ll++)
                {
                    // Verifica qual é o menor custo de inserção entre os indices da rota
                    int estacao_pos = vetorLL.stationAtIndex(index_ll);
                    int estacao_pre = vetorLL.stationAtIndex(index_ll - 1);

                    int custo = 0 + p.matriz_custo[estacao_pre][estacao] + p.matriz_custo[estacao][estacao_pos] - p.matriz_custo[estacao_pre][estacao_pos];

                    // Copia e transformação afim de testar demanda.
                    LinkedList linkedlist_novo_teste = vetorLL;
                    linkedlist_novo_teste.InsertAt(index_ll, estacao);
                    vector<int> vetorNovo_teste_reverso = transformarEmVector(linkedlist_novo_teste);   // Criação do vetor da rota, inicialmente sentido d1
                    vector<int> vetorNovo_teste = vetorNovo_teste_reverso;                              // Recebe uma copia do vetor no sentido normal

                    reverse(vetorNovo_teste_reverso.begin(), vetorNovo_teste_reverso.end());            // Reverte o vetor para ficar no sentido reverso, conforme esperado

                    // Verifica se com esta inserção há um custo menor que o anteriormente registrado
                    // E se a rota é valida no sentido d1...
                    if (custo < menor_custo && VerificaDemanda(vetorNovo_teste))
                    {
                        // Verifica se o custo é menor que o anterior registrado e se é uma rota valida
                        // Se sim, checa se o sentido d1 ou d2 é menor
                        menor_custo = linkedlist_novo_teste.custo_total_d1;
                        index_rota_salva = index_rota;
                        rota_salva = vetorNovo_teste;
                    }
                    else if (custo < menor_custo && VerificaDemanda(vetorNovo_teste_reverso)) // ... Ou no sentido d2
                    {
                        menor_custo = linkedlist_novo_teste.custo_total_d2;
                        index_rota_salva = index_rota;
                        rota_salva = vetorNovo_teste_reverso;
                    }
                }
            }
            if ((menor_custo != MAXX_INT && menor_custo <= (p.matriz_custo[0][estacao] + p.matriz_custo[estacao][0])) || (caminhoes_restantes == 0))
            { // Decide inserir em uma rota já existente

                rotas[index_rota_salva] = rota_salva;
            }
            else // Cria uma nova rota do tipo {0, X, 0}
            {
                rotas.push_back({0, estacao, 0});
                caminhoes_restantes--;
            }

            index_rota++;
        }

        // Montando objeto Solucao
        Solucao solucao;
        solucao.veiculos_disponiveis = caminhoes_restantes;
        solucao.veiculos_usados = p.qnt_veiculos - caminhoes_restantes;
        solucao.rotas = rotas;
        int custo_total = 0;
        for (vector<int> rota : rotas)
        {
            custo_total += CalculaCusto(rota);
        }
        solucao.custo_total = custo_total;

        return solucao;
    }

    Solucao IMB() {
        vector<vector<int>> rotas(p.qnt_veiculos, vector<int>(2, 0));
        for (int i = 0; i < p.qnt_veiculos; i++)
            rotas[i].reserve(p.qnt_estacoes / p.qnt_veiculos + 2);

        int caminhoes_usados = 1;
        int custo_total = 0;

        for (int i = 1; i < p.qnt_estacoes; i++) {
            int menor_custo = MAXX_INT;
            int melhor_rota = -1;
            int melhor_posicao = -1;

            for (int j = 0; j < caminhoes_usados; j++) {
                for (int k = 1; k < rotas[j].size(); k++) {
                    int custo = p.matriz_custo[rotas[j][k - 1]][i] + p.matriz_custo[i][rotas[j][k]] - p.matriz_custo[rotas[j][k - 1]][rotas[j][k]];
                    if (custo < menor_custo && InsertionTest(rotas[j], k, i)) {
                        menor_custo = custo;
                        melhor_rota = j;
                        melhor_posicao = k;
                    }
                }
            }

            if (melhor_rota == -1 || menor_custo > (p.matriz_custo[0][i] + p.matriz_custo[i][0])) {
                if (caminhoes_usados < p.qnt_veiculos) {
                    rotas[caminhoes_usados] = {0, i, 0};
                    custo_total += p.matriz_custo[0][i] + p.matriz_custo[i][0];
                    caminhoes_usados++;
                } else {
                    rotas[0].insert(rotas[0].begin() + 1, i);
                    custo_total += p.matriz_custo[0][i] + p.matriz_custo[i][rotas[0][2]];
                }
            } else {
                rotas[melhor_rota].insert(rotas[melhor_rota].begin() + melhor_posicao, i);
                custo_total += menor_custo;
            }
        }

        Solucao solucao;
        solucao.rotas.resize(caminhoes_usados);
        for (int i = 0; i < caminhoes_usados; i++)
            solucao.rotas[i] = rotas[i];

        solucao.custo_total = custo_total;
        solucao.veiculos_usados = solucao.rotas.size();
        solucao.veiculos_disponiveis = p.qnt_veiculos - solucao.veiculos_usados;
        return solucao;
    }


    void MelhorarSolucao(vector<vector<int>>& rotas, int custo_atual, int iter = 1){
        if(iter > 500) return;

        int est1_anterior, est1, est1_proximo;
        int est2_anterior, est2, est2_proximo;

        for(int id_rota_origem = 0; id_rota_origem < rotas.size(); id_rota_origem++){
            if(rotas[id_rota_origem].size() <= 2) continue; // ignora rotas pequenas

            for(int id_estacao_origem = 1; id_estacao_origem < rotas[id_rota_origem].size() - 1; id_estacao_origem++){
                est1_anterior = rotas[id_rota_origem][id_estacao_origem - 1];
                est1 = rotas[id_rota_origem][id_estacao_origem];
                est1_proximo = rotas[id_rota_origem][id_estacao_origem + 1];

                for(int id_rota_destino = 0; id_rota_destino < rotas.size(); id_rota_destino++){
                    if(rotas[id_rota_destino].size() <= 2) continue;

                    for(int id_estacao_destino = 1; id_estacao_destino < rotas[id_rota_destino].size() - 1; id_estacao_destino++){
                        if(id_rota_origem == id_rota_destino && (id_estacao_origem == id_estacao_destino || id_estacao_origem == id_estacao_destino - 1))
                            continue;

                        est2_anterior = rotas[id_rota_destino][id_estacao_destino - 1];
                        est2 = rotas[id_rota_destino][id_estacao_destino];
                        est2_proximo = rotas[id_rota_destino][id_estacao_destino + 1];

                        int custo_teste = custo_atual;

                        custo_teste += p.matriz_custo[est1_anterior][est2];
                        custo_teste += p.matriz_custo[est2][est1_proximo];
                        custo_teste += p.matriz_custo[est2_anterior][est1];
                        custo_teste += p.matriz_custo[est1][est2_proximo];

                        custo_teste -= p.matriz_custo[est1_anterior][est1];
                        custo_teste -= p.matriz_custo[est1][est1_proximo];
                        custo_teste -= p.matriz_custo[est2_anterior][est2];
                        custo_teste -= p.matriz_custo[est2][est2_proximo];

                        if(custo_teste < custo_atual){
                            swap(rotas[id_rota_origem][id_estacao_origem], rotas[id_rota_destino][id_estacao_destino]);
                            MelhorarSolucao(rotas, custo_teste, iter + 1);
                            return;
                        }
                    }
                }
            }
        }
    }

    vector<int> VND_Swap(const vector<int> &rota){
        vector<int> rota_ret = rota;   // copia a rota antiga
        int melhor_custo = CalculaCusto(rota_ret);
        int custo_swap = 0;
        
        for(int i = 1; i < rota_ret.size() - 2; i++){
            for(int j = i + 1; j < rota_ret.size() - 1; j++){
                vector<int> rota_teste = rota_ret;

                swap(rota_teste[i], rota_teste[j]);

                custo_swap = CalculaCusto(rota_teste);

                if(custo_swap < melhor_custo && VerificaDemanda(rota_teste)){
                    rota_ret = rota_teste;
                    melhor_custo = custo_swap;
                }
            }
        }
        
        return rota_ret;
    }

    vector<int> VND_ReInsertion(const vector<int> &rota){
        vector<int> rota_ret = rota;
        int melhor_custo = CalculaCusto(rota);
        int custo_reinsertion = 0;
        
        for(int i = 1; i < rota_ret.size() - 1; i++){
            for(int j = i + 1; j < rota_ret.size() - 1; j++){
                vector<int> rota_teste = rota_ret;                
                
                rota_teste.erase(rota_teste.begin() + i);
                rota_teste.insert(rota_teste.begin() + j-1, rota_ret[i]);

                custo_reinsertion = CalculaCusto(rota_teste);

                if(custo_reinsertion < melhor_custo && VerificaDemanda(rota_teste)){
                    rota_ret = rota_teste;
                    melhor_custo = custo_reinsertion;
                }
            }
        }
        
        return rota_ret;
    }

    vector<int> VND_Inversion(const vector<int> &rota){
        vector<int> rota_ret = rota;
        int melhor_custo = CalculaCusto(rota_ret);
        int custo_inversion = 0;
        
        for(int i = 1; i < rota_ret.size() - 1; i++){
            for(int j = i + 1; j < rota_ret.size() - 1; j++){
                vector<int> rota_teste = rota_ret;

                // inverter o intervalo [i, j] na nova rota
                reverse(rota_teste.begin() + i, rota_teste.begin() + j);
                
                // recalcular o custo da rota_teste
                custo_inversion = CalculaCusto(rota_teste);

                if(custo_inversion < melhor_custo && VerificaDemanda(rota_teste)){
                    rota_ret = rota_teste;
                    melhor_custo = custo_inversion;
                }
            }
        }

        return rota_ret;
    }

    void VND(vector<int> &rota){

        int melhor_custo = CalculaCusto(rota);

        vector<function<vector<int>(vector<int>)>> estruturas_VND = {
            VND_Swap,
            VND_ReInsertion,
            VND_Inversion
        };

        int k = 0; // estrutura swap primeiro
        int N = estruturas_VND.size();
        while(k < N){
            vector<int> nova_rota = estruturas_VND[k](rota);
            int novo_custo = CalculaCusto(nova_rota);

            if(novo_custo < melhor_custo){
                rota = nova_rota;
                melhor_custo = novo_custo;
                k = 0;
            }else{
                k++;
            }
        }

        return;
    }


    int VNDIntra(vector<int> &rota){

        int melhor_custo = CalculaCusto(rota);

        vector<function<vector<int>(vector<int>)>> estruturas_VND = {
            VND_Swap,
            VND_ReInsertion,
            VND_Inversion
        };

        int k = 0; // estrutura swap primeiro
        int N = estruturas_VND.size();
        while(k < N){
            vector<int> nova_rota = estruturas_VND[k](rota);
            int novo_custo = CalculaCusto(nova_rota);

            if(novo_custo < melhor_custo){
                rota = nova_rota;
                melhor_custo = novo_custo;
                k = 0;
            }else{
                k++;
            }
        }

        return melhor_custo;
    }

    void VND(vector<vector<int>> &rotas){
        int n_rotas = rotas.size();

        // distribuir as rotas nas threads
        for(int i = 0; i < n_rotas; i++){
            int custo_rota_i = CalculaCusto(rotas[i]);
            VND(rotas[i]);    
        }
        return;
    }

    int VNDInterSwap(vector<int> &r1, vector<int> &r2){
        if(r1.size() < 3 || r2.size() < 3) return CalculaCusto(r1) + CalculaCusto(r2);

        int melhor_custo = CalculaCusto(r1) + CalculaCusto(r2);
        int novo_custo, custo_antigo;
        
        // percorre a rota r1 (ignorando a garagem nas pontas)
        for(int i = 1; i < r1.size() - 1; i++){
            // percorre a rota r2 (ignorando a garagem nas pontas)
            for(int j = 1; j < r2.size() - 1; j++){
                // verifica o custo antes de criar uma cópia
                // novo_custo =  p.matriz_custo[r1[i-1]][r2[j]] + p.matriz_custo[r2[j]][r1[i+1]];
                // novo_custo += p.matriz_custo[r2[j-1]][r1[i]] + p.matriz_custo[r1[i]][r2[j+1]];
                // custo_antigo = p.matriz_custo[r1[i-1]][r1[i]] + p.matriz_custo[r1[i]][r1[i+1]];
                // custo_antigo += p.matriz_custo[r2[j-1]][r2[j]] + p.matriz_custo[r2[j]][r2[j+1]]; 
                // novo_custo = melhor_custo + novo_custo - custo_antigo;

                vector<int> copy_r1 = r1, copy_r2 = r2;
                swap(copy_r1[i], copy_r2[j]);
                
                novo_custo = CalculaCusto(copy_r1) + CalculaCusto(copy_r2);

                if(novo_custo < melhor_custo){
                    // se o custo for melhor, cria cópias e realiza, de fato, o swap
                    
                    // usa essas copias para verificar se as demandas desse swap são válidas
                    if(VerificaDemanda(copy_r1) && VerificaDemanda(copy_r2)){
                        // atualiza as rotas e o melhor custo para a melhor versão até o momento
                        // cout << "[Swap]: Custo anterior: " << melhor_custo << ", Novo custo aceito: " << novo_custo << endl;
                        melhor_custo = novo_custo;
                        r1 = copy_r1;
                        r2 = copy_r2;
                    }
                }
            }
        }

        return melhor_custo;
    }

    int VNDInterRelocate(vector<int> &r1, vector<int> &r2){
        if(r1.size() < 3 || r2.size() < 3) return CalculaCusto(r1) + CalculaCusto(r2);

        int melhor_custo = CalculaCusto(r1) + CalculaCusto(r2);
        int novo_custo, custo_antigo;
        
        // percorre a rota r1 (ignorando a garagem nas pontas)
        for(int i = 1; i < r1.size() - 1; i++){
            // percorre a rota r2 (ignorando a garagem nas pontas)
            for(int j = 1; j < r2.size() - 1; j++){

                // novo_custo =  p.matriz_custo[r2[j]][r1[i]] 
                //             + p.matriz_custo[r1[i]][r2[j+1]]
                //             + p.matriz_custo[r1[i-1]][r1[i+1]];
                // custo_antigo= p.matriz_custo[r2[j]][r2[j+1]]
                //             + p.matriz_custo[r1[i-1]][r1[i]]
                //             + p.matriz_custo[r1[i]][r1[i+1]];
                
                // novo_custo = melhor_custo + (novo_custo - custo_antigo);

                vector<int> copy_r1 = r1, copy_r2 = r2;
            
                copy_r2.insert(copy_r2.begin() + j, copy_r1[i]);
                copy_r1.erase(copy_r1.begin() + i);

                novo_custo = CalculaCusto(copy_r1) + CalculaCusto(copy_r2);
                
                if(novo_custo < melhor_custo){

                    if(VerificaDemanda(copy_r1) && VerificaDemanda(copy_r2)){
                        // cout << "[Realocate]: Custo anterior: " << melhor_custo << ", Novo custo aceito: " << novo_custo << endl;
                        melhor_custo = novo_custo;
                        r1 = copy_r1;
                        r2 = copy_r2;
                    }
                }
            }
        }

        return melhor_custo;
    }

    // muito propensa a deixar a rota não coesa
    int VNDInterMerge(vector<int> &r1, vector<int> &r2){
        if(r1.size() < 3 || r2.size() < 3) return CalculaCusto(r1) + CalculaCusto(r2);

        int custo_anterior = CalculaCusto(r1) + CalculaCusto(r2);
        int custo1 = custo_anterior, custo2 = custo_anterior;
        
        // primeiro, tenta fazer um merge da primeira com a segunda
        vector<int> merge1 = r1;
        // remove a estação final (garagem)
        merge1.erase(merge1.end());
        // cola a r2 (sem a estacao inicial) em merge1
        merge1.insert(merge1.end(), r2.begin() + 1, r2.end());
        // calcula o custo:
        custo1 = CalculaCusto(merge1);
        
        // agora, faz o mesmo só que ao contrário:
        vector<int> merge2 = r2;
        // remove a estação final (garagem)
        merge2.erase(merge2.end());
        // cola a r1 (sem a estacao inicial) em merge2
        merge2.insert(merge2.end(), r1.begin() + 1, r1.end());
        // calcula o custo:
        custo2 = CalculaCusto(merge2);

        // se algum deles tiver melhorado o custo e for melhor que o outro, 
        // substitui as rotas e retorna esse melhor valor
        if(custo1 <= custo2){
            if(custo1 < custo_anterior && VerificaDemanda(merge1)){
                r1 = merge1;
                r2 = {0, 0};
                return custo1;
            }
        }else{
            if(custo2 < custo_anterior && VerificaDemanda(merge2)){
                r2 = merge2;
                r1 = {0, 0};
                return custo2;
            }
        }

        // caso contrário, mantém as rotas normalmente e retorna o valor de custo antigo mesmo
        return custo_anterior;
    }

    int VNDInterCrossover(vector<int> &r1, vector<int> &r2){
        if(r1.size() < 3 || r2.size() < 3) return CalculaCusto(r1) + CalculaCusto(r2);

        int melhor_custo = CalculaCusto(r1) + CalculaCusto(r2);
        int novo_custo;
        vector<int> copy_r1, copy_r2;
        srand(time(NULL)); // seed para aleatorizar o tamanho do segmento
        
        int limite = r1.size() > r2.size() ? r2.size() - 3 : r1.size() - 3;

        // para cada segmento, tenta fazer um swap de segmentos:
        for(int i = 1; i < r1.size() - 1; i++){
            for(int j = 1; j < r2.size() - 1; j++){
                
                int max_tam = min({limite, (int)r1.size()-i-1, (int)r2.size()-j-1});
                if(max_tam < 2) break; // não dá pra formar segmento válido a partir daqui
                int tamanho_seg = 2 + rand() % (max_tam - 1);
                
                copy_r1 = r1;
                copy_r2 = r2;
                for(int k = 0; k < tamanho_seg; k++){
                    swap(copy_r1[i+k], copy_r2[j+k]);
                }

                novo_custo = CalculaCusto(copy_r1) + CalculaCusto(copy_r2);

                if(novo_custo < melhor_custo && VerificaDemanda(copy_r1) && VerificaDemanda(copy_r2)){
                    // cout << "[Crossover]: Custo anterior: " << melhor_custo << ", Novo custo aceito: " << novo_custo << endl;
                    melhor_custo = novo_custo;
                    r1 = copy_r1;
                    r2 = copy_r2;
                }
            }
        }

        return melhor_custo;
    }

    int VNDInter(vector<int> &r1, vector<int> &r2){
        int melhor_custo = CalculaCusto(r1) + CalculaCusto(r2);
        int novo_custo;

        vector<function<int(vector<int>&, vector<int>&)>> estruturasInterRota = {
            VNDInterSwap,
            VNDInterCrossover,
            VNDInterRelocate
        };

        int k = 0;
        int N = estruturasInterRota.size();
        int i = 0;
        while(k < N){
            novo_custo = estruturasInterRota[k](r1, r2);
            if(novo_custo < melhor_custo){
                melhor_custo = novo_custo;
                k = 0;
            }else{
                k++;
            } 

            // se em alguma das estruturas uma delas ficar vazia, retorna, pois só haverá uma rota, afinal 
            if(r1.size() < 3 || r2.size() < 3){
                return melhor_custo;
            }
            i++;
        }

        return melhor_custo;
    }

    // VND intra + inter-rota
    void VNDIntraInter(vector<vector<int>> &rotas){
        int custo_anterior, novo_custo;
        bool melhorou = true;
        
        while(melhorou){
            melhorou = false;
             
            // vnd intra-rota
            for(int i = 0; i < (int)rotas.size(); i++){
                custo_anterior = CalculaCusto(rotas[i]);
                novo_custo = VNDIntra(rotas[i]);
                
                if(novo_custo < custo_anterior){
                    cout << "Melhora na rota " << i << "! Diferenca: " << novo_custo - custo_anterior << endl;
                    melhorou = true;
                }
            }
            
            // vnd inter-rotas
            // vamos coletar índices a remover e só apagar depois
            for(int i = 0; i < (int)rotas.size() - 1; i++){
                for(int j = i + 1; j < (int)rotas.size(); j++){
                    custo_anterior = CalculaCusto(rotas[i]) + CalculaCusto(rotas[j]);
                    novo_custo = VNDInter(rotas[i], rotas[j]);
                    if(novo_custo < custo_anterior){
                        cout << "Melhora nas rotas " << i << " e " << j << " ! Diferenca: " << novo_custo - custo_anterior << endl;
                        melhorou = true;
                    }
                }
            }

           

            // opcional: mostrar custo total aproximado (soma das rotas)
            cout << "[VND] Custo total: " << CustoTotal(rotas) << " | rotas: " << rotas.size() << endl;
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

    void PerturbacaoScramble(vector<vector<int>>& rotas, int qtd_scrambles){
        while(qtd_scrambles > 0){
            
            // Escolhendo uma rota aleatoria
            int id_rota_escolhida;
            do{
                id_rota_escolhida = rand() % rotas.size();
            }while(rotas[id_rota_escolhida].size() <= 3);

            // Selecionando qual fatia daquela rota será permutada
            int index_inicio_fatia = 0;
            int index_fim_fatia = 0;

            
            while (index_inicio_fatia == index_fim_fatia)
            {
                index_inicio_fatia = 1 + rand() % (rotas[id_rota_escolhida].size() - 2);    // 1 .. n-2
                index_fim_fatia    = 1 + rand() % (rotas[id_rota_escolhida].size() - 2);    // 1 .. n-2
            }
           
            // Garantindo que os index_inicio é menor
            if (index_inicio_fatia > index_fim_fatia)
                swap(index_inicio_fatia, index_fim_fatia);
            
            // Fatia terá intervalo [index_inicio_fatia] (inclusivo) a [index_fim_fatia] (exclusivo)
            // Necessario ajustar

            // Scrambling
            // Gerador de numeros aleatorios
            std::random_device rd;
            std::mt19937 g(rd());

            shuffle(rotas[id_rota_escolhida].begin() + index_inicio_fatia,
                    rotas[id_rota_escolhida].begin() + index_fim_fatia, g);
           
            qtd_scrambles--;
        }
    }

    void PerturbacaoReverse(vector<vector<int>>& rotas, int qtd_reverses){
        vector<int> reversed_routes;
        if (qtd_reverses > rotas.size())    // Se o termo for maior que o numero de rotas, alguma delas vai ser revertida e posta de novo pro normal
            return;

        while(qtd_reverses > 0){

            int random_route_index = rand() % rotas.size();

            // Verificação se o termo já está no vetor
            if (find(reversed_routes.begin(), reversed_routes.end(), random_route_index) != reversed_routes.end()){
                continue;
            }

            while(rotas[random_route_index].size() <= 3){ // Rota só tem 1 termo efetivo, sortear dnv
                random_route_index = rand() % rotas.size();
            }

            // Reverter rota
            reverse(rotas[random_route_index].begin(), rotas[random_route_index].end());
            reversed_routes.push_back(random_route_index);

            qtd_reverses--;
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

        vector<int> elementos_retirados(qtd_elementos + 2);
        elementos_retirados[0] = 0; // depósito inicial
        elementos_retirados[qtd_elementos + 1] = 0; // depósito final
        int aux = 1;

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
        rotas.push_back(elementos_retirados);
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
             case 6:
                 PerturbacaoScramble(rotas, 2 * nivel_perturbacao);
                 break;
             case 7:
                 PerturbacaoReverse(rotas, nivel_perturbacao);
                 break;
            default:
                break;
        }
    }

    void ILS(vector<vector<int>>& rotas, int max_iteracoes = 10000, int max_sem_melhora = 1000){
        srand(time(NULL));
        int iteracoes = 0, sem_melhora = 0;
        int melhor_custo = CustoTotal(rotas), custo_teste;

        while(iteracoes < max_iteracoes && sem_melhora < max_sem_melhora){
            int opcao_perturbacao = 1 + rand() % 7;
            int nivel_perturbacao = (sem_melhora / (max_sem_melhora / 6)) + 2;

            vector<vector<int>> rotas_copia = rotas;
            Perturbar(rotas_copia, opcao_perturbacao, nivel_perturbacao);

            MelhorarSolucao(rotas_copia, CustoTotal(rotas_copia));
            VNDIntraInter(rotas_copia);

            custo_teste = CustoTotal(rotas_copia);

            if (custo_teste < melhor_custo && VerificaDemandas(rotas_copia)){
                rotas = move(rotas_copia);
                melhor_custo = custo_teste;
                Solucao sol = Solucao();
                sol.rotas = rotas;
                sol.custo_total = melhor_custo;
                sol.SalvarSolucao("ILS_melhorou.sol");
                sem_melhora = 0;
                cout << "Melhora! Novo custo: " << melhor_custo << endl;
            }else{
                sem_melhora++;
            }

            iteracoes++;
        }
    }

    struct ThreadData {
        int thread_id;
        vector<vector<int>>& rotas;
        int max_iteracoes;
        int max_sem_melhora;
        int &melhor_custo;
        ThreadPool &pool;

        ThreadData(int id,
                vector<vector<int>>& r,
                int maxIt,
                int maxNoImprove,
                int &custo,
                ThreadPool &p)
            : thread_id(id),
            rotas(r),
            max_iteracoes(maxIt),
            max_sem_melhora(maxNoImprove),
            melhor_custo(custo),
            pool(p) {}
    };

    void ThreadILS(void *data){
        ThreadData *threadData = (ThreadData *) data;
        srand(time(NULL) + threadData->thread_id); // seed diferente para cada thread
        cout << "[Thread " << threadData->thread_id << "] Iniciando ILS..." << endl;

        int iteracoes = 0, sem_melhora = 0, custo_teste;

        while(iteracoes < threadData->max_iteracoes && sem_melhora < threadData->max_sem_melhora){
            int opcao_perturbacao = 1 + rand() % 7;
            int nivel_perturbacao = (sem_melhora / (threadData->max_sem_melhora / 6)) + 2;

            threadData->pool.lock();
            vector<vector<int>> rotas_copia = threadData->rotas;
            threadData->pool.release();

            Perturbar(rotas_copia, opcao_perturbacao, nivel_perturbacao);
            cout << "perturbou" << endl;

            MelhorarSolucao(rotas_copia, CustoTotal(rotas_copia));
            cout << "melhorou" << endl;

            VNDIntraInter(rotas_copia);
            cout << "vnd intra inter" << endl;

            int custo_teste = CustoTotal(rotas_copia);

            threadData->pool.lock();
            if (custo_teste < threadData->melhor_custo && VerificaDemandas(rotas_copia)){
                threadData->rotas = move(rotas_copia);
                threadData->melhor_custo = custo_teste;
                sem_melhora = 0;
                cout << "[Thread " << threadData->thread_id << "] Melhora! Novo custo: " << threadData->melhor_custo << endl;
            }else{
                sem_melhora++;
            }
            threadData->pool.release();

            iteracoes++;
        }

        cout << "[Thread " << threadData->thread_id << "] Finalizando." << endl;
    }

#endif