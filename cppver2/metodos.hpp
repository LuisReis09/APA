#ifndef _METODOS_HPP_
#define _METODOS_HPP_

    #include <iostream>
    #include <stdlib.h>
    #include <vector>
    #include "structures.hpp"
    #include "utils.hpp"
    #include <time.h>
    #include <map>
    #include <functional>
    #include <random>
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
    
    Solucao InsercaoMaisBarata()
    {
        vector<int> estacoes_restantes(p.qnt_estacoes);
        for (int i = 1; i <= p.qnt_estacoes; i++)
        {
            estacoes_restantes[i - 1] = i;
        }

        pair<int, int> vertices = MelhoresVertices();
        vector<vector<int>> rotas = {{0, vertices.first, vertices.second, 0}};

        int caminhoes_restantes = p.qnt_veiculos;

        for (int estacao : estacoes_restantes)
        {
            int menor_custo = INT_MAX; // Menor custo possivel para aquela
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
                    LinkedList vetorNovo_teste = vetorLL;
                    linkedlist_novo_teste.InsertAt(index_ll, estacao);
                    vector<int> vetorNovo_teste = transformarEmVector(linkedlist_novo_teste);
                    vector<int> vetorNovo_teste_reverso = reverse(vetorNovo_teste.begin(), vetorNovo_teste.end());

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
            if ((menor_custo != INT_MAX && menor_custo <= (p.matriz_custo[0][estacao] + p.matriz_custo[estacao][0])) || (caminhoes_restantes == 0))
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

    void MelhorarSolucao(vector<vector<int>>& rotas){
        int custo_remocao, custo_insercao;
        int estacao_origem, estacao_destino;
        bool melhorou = false;
        
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
                                    melhorou = true;
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
                                    melhorou = true;
                                    rotas[id_rota_origem].erase(rotas[id_rota_origem].begin() + id_estacao_origem);
                                    rotas[id_rota_destino].insert(rotas[id_rota_destino].begin() + id_estacao_destino, estacao_origem);
                                }
                            }

                        }
                    }
                }
            }
        }

        if(melhorou)
            MelhorarSolucao(rotas);
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

    void VND(vector<vector<int>> &rotas){
        int n_rotas = rotas.size();

        // distribuir as rotas nas threads
        for(int i = 0; i < n_rotas; i++){
            int custo_rota_i = CalculaCusto(rotas[i]);
            VND(rotas[i]);    
        }
        return;
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

            if(sem_melhora % 50)
                MelhorarSolucao(rotas_copia);
            else
                VND(rotas_copia);

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