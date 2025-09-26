#ifndef _VND1_HPP_
#define _VND1_HPP_

    #include <iostream>
    #include <stdlib.h>
    #include <vector>
    #include <algorithm>
    #include "utils.hpp"
    #include "structures.hpp"

    int VND_Intra_Swap(vector<int>& rota, int custo_anterior){
        int custo_swap, custo_atual;
        int n = rota.size();

        for(int i = 1; i < n-2; i++){
            for(int j = i+1; j < n-1; j++){

                if(j == i+1){
                    custo_atual = p.matriz_custo[rota[i-1]][rota[i]] + p.matriz_custo[rota[i]][rota[j]] + p.matriz_custo[rota[j]][rota[j+1]];
                    custo_swap  = p.matriz_custo[rota[i-1]][rota[j]] + p.matriz_custo[rota[j]][rota[i]] + p.matriz_custo[rota[i]][rota[j+1]];
                }else{
                    custo_atual = p.matriz_custo[rota[i-1]][rota[i]] + p.matriz_custo[rota[i]][rota[i+1]] + p.matriz_custo[rota[j-1]][rota[j]] + p.matriz_custo[rota[j]][rota[j+1]];
                    custo_swap  = p.matriz_custo[rota[i-1]][rota[j]] + p.matriz_custo[rota[j]][rota[i+1]] + p.matriz_custo[rota[j-1]][rota[i]] + p.matriz_custo[rota[i]][rota[j+1]];
                }

                // Se o custo compensar:
                if(custo_swap < custo_atual){
                    // Precisamos verificar se essa troca eh possivel segundo as demandas
                    if(VerificaSwapIntraVND(rota, i, j)){
                        swap(rota[i], rota[j]);
                        custo_anterior -= custo_atual;
                        custo_anterior += custo_swap;
                    }
                }
            }
        }

        return custo_anterior;
    }

    int VND_Intra_ShiftN(vector<int>& rota, int custo_anterior){
        int nMax = min((int) rota.size() - 2, 4);
        int custo_teste, custo_atual = CustoRota(rota);

        for(int i = 1; i < rota.size() - 2; i++){
            int max_seg = min(nMax, (int) rota.size() - i - 1);
            if(max_seg < 1) continue;
            int tam_seg = 1 + rand() % max_seg;

            for(int j = i+tam_seg; j < rota.size(); j++){

                custo_teste = VerificaIntraShiftn(rota, i, j, tam_seg);
                
                if(custo_teste != -1 && custo_teste < custo_atual){
                    vector<int> bloco(rota.begin() + i, rota.begin() + i + tam_seg);
                    // Fastar os elementos, preenchendo o espaco da "retirada" do bloco
                    for(int aux = i; aux < j; aux++){
                        rota[aux] = rota[aux + tam_seg];
                    }

                    // Inserir os elementos, preenchendo o espaco liberado
                    for(int aux = j, aux2 = 0; aux2 < tam_seg; aux++, aux2++){
                        rota[aux] = bloco[aux2];
                    }
                    
                    custo_anterior -= custo_atual;
                    custo_anterior += custo_teste;
                    custo_atual = custo_teste;
                }
            }
        }

        return custo_anterior;
    }

    int VND_Intra_Reinsertion(vector<int>& rota, int custo_anterior){
        int custo_reinsertion, custo_atual;

        for(int i = 1; i < rota.size() - 1; i++){
            for(int j = 1; j < rota.size() - 1; j++){
                if(i == j || j == i+1) continue;

                // O que seria adicionado
                custo_reinsertion = p.matriz_custo[rota[j-1]][rota[i]] + p.matriz_custo[rota[i]][rota[j]] + p.matriz_custo[rota[i-1]][rota[i+1]];

                // O que seria removido
                custo_atual = p.matriz_custo[rota[i-1]][rota[i]] + p.matriz_custo[rota[i]][rota[i+1]] + p.matriz_custo[rota[j-1]][rota[j]];

                // Se o custo compensar
                if(custo_reinsertion < custo_atual){
                    // Se for possivel atender as demandas
                    if(ReinsertionTest(rota, i, j)){
                        // Se for inserindo depois de onde esta
                        if(i < j){
                            int estacao = rota[i];
                            for(int aux = i; aux < j; aux++){
                                rota[aux] = rota[aux+1];
                            }
                            rota[j] = estacao;
                        }
                        else{
                            int estacao = rota[i];
                            for(int aux = i; aux > j; aux--){
                                rota[aux] = rota[aux-1];
                            }
                            rota[j] = estacao;
                        }

                        custo_anterior += custo_reinsertion - custo_atual;
                    }
                }
            }
        }

        return custo_anterior;
    }

    int VND_Intra_Inversion(vector<int> &rota, int custo_anterior){
        int custo_inversion, custo_atual;

        custo_atual = CustoRota(rota);

        for(int i = 1; i < rota.size() - 2; i++){
            for(int j = i+1; j < rota.size() - 1; j++){
                custo_inversion = TentaInverter(rota, i, j);

                // Se o custo veio -1, significa que nao pode devido as demandas
                if(custo_inversion != -1){

                    // Se o custo compensar, invertemos
                    if(custo_inversion < custo_atual){
                        reverse(rota.begin() + i, rota.begin() + j + 1);
                        custo_anterior -= custo_atual;
                        custo_anterior += custo_inversion;
                        custo_atual = custo_inversion;
                    }
                }
            }
        }

        return custo_anterior;
    }

    int VND_Intra(vector<int>& rota, int custo_anterior){
        int novo_custo;
        int k = 1;

        // Teremos 4 estruturas de vizinhanca
        while(k < 4){
            switch(k){
                case 1: novo_custo = VND_Intra_Swap(rota, custo_anterior); break;
                case 2: novo_custo = VND_Intra_Inversion(rota, custo_anterior); break;
                case 3: novo_custo = VND_Intra_Reinsertion(rota, custo_anterior); break;
                case 4: novo_custo = VND_Intra_ShiftN(rota, custo_anterior); break;
            }

            if(novo_custo < custo_anterior){
                custo_anterior = novo_custo;
                k = 1;
            }
            else{
                k++;
            }
        }

        return custo_anterior;
    }

    int VND_Inter_Swap(vector<int> &r1, vector<int> &r2, int custo_anterior){
        int custo_atual, custo_swap;

        for(int i = 1; i < r1.size() - 1; i++){
            for(int j = 1; j < r2.size() - 1; j++){

                // O que se tem atualmente
                custo_atual = p.matriz_custo[r1[i-1]][r1[i]];
                custo_atual += p.matriz_custo[r1[i]][r1[i+1]];
                custo_atual += p.matriz_custo[r2[j-1]][r2[j]];
                custo_atual += p.matriz_custo[r2[j]][r2[j+1]];

                // O que se pretende verificar
                custo_swap = p.matriz_custo[r1[i-1]][r2[j]];
                custo_swap += p.matriz_custo[r2[j]][r1[i+1]];
                custo_swap += p.matriz_custo[r2[j-1]][r1[i]];
                custo_swap += p.matriz_custo[r1[i]][r2[j+1]];

                if(custo_swap < custo_atual){
                    if(VerificaSwapInterVND(r1, i, r2[j]) && VerificaSwapInterVND(r2, j, r1[i])){
                        swap(r1[i], r2[j]);
                        custo_anterior -= custo_atual;
                        custo_anterior += custo_swap;
                    }
                }
            }
        }

        return custo_anterior;
    }

    int VND_Inter_Relocate(vector<int> &r1, vector<int> &r2, int custo_anterior){
        int custo_novo, custo_atual;

        for(int i = 1; i < r1.size() - 1; i++){
            for(int j = 1; j < r2.size(); j++){

                // O que temos no momento
                custo_atual = p.matriz_custo[r1[i-1]][r1[i]];
                custo_atual += p.matriz_custo[r1[i]][r1[i+1]];
                custo_atual += p.matriz_custo[r2[j-1]][r2[j]];

                // O que teriamos ao reinserir
                custo_novo = p.matriz_custo[r1[i-1]][r1[i+1]];
                custo_novo += p.matriz_custo[r2[j-1]][r1[i]];
                custo_novo += p.matriz_custo[r1[i]][r2[j]];

                if(custo_novo < custo_atual){
                    if(RemovalTest(r1, i) && InsertionTest(r2, j, r1[i])){
                        r2.insert(r2.begin() + j, r1[i]);
                        r1.erase(r1.begin() + i);

                        custo_anterior -= custo_atual;
                        custo_anterior += custo_novo;
                    }
                }
            }
        }

        return custo_anterior;
    }

    int VND_Inter_Swap2x2(vector<int> &r1, vector<int> &r2, int custo_anterior){
        int custo_novo, custo_atual;

        for(int i = 1; i < r1.size() - 2; i++){
            for(int j = 1; j < r2.size() - 2; j++){

                // O que temos:
                custo_atual = p.matriz_custo[r1[i-1]][r1[i]] + p.matriz_custo[r1[i+1]][r1[i+2]] + p.matriz_custo[r2[j-1]][r2[j]] + p.matriz_custo[r2[j+1]][r2[j+2]];

                // O que teriamos ao dar swap:
                custo_novo =  p.matriz_custo[r1[i-1]][r2[j]] + p.matriz_custo[r2[j+1]][r1[i+2]] + p.matriz_custo[r2[j-1]][r1[i]] + p.matriz_custo[r1[i+1]][r2[j+2]];

                if(custo_novo < custo_atual){
                    swap(r1[i], r2[j]);
                    swap(r1[i+1], r2[j+1]);

                    if(VerificaDemanda(r1) && VerificaDemanda(r2)){
                        custo_anterior -= custo_atual;
                        custo_anterior += custo_novo;
                    }
                    else{
                        // Desfaz o swap
                        swap(r1[i], r2[j]);
                        swap(r1[i+1], r2[j+1]);
                    }
                }
            }
        }

        return custo_anterior;
    }

    int VND_Inter_NOpt1(vector<int> &r1, vector<int> &r2, int custo_anterior){
        int custo_novo, custo_atual;
        int max_seg, tam_seg;

        custo_atual = CustoRota(r1) + CustoRota(r2);

        for(int i = 1; i < r1.size() - 2; i++){
            max_seg = r1.size() - i - 1;
            tam_seg = RandomEntre(2, max_seg);

            for(int j = 1; j < r2.size() - 1; j++){
                custo_novo = VerificaInterNOpt(r1, r2, j, i, tam_seg);

                if(custo_novo != -1 && custo_novo < custo_atual){
                    r2.insert(r2.begin() + j, r1.begin() + i, r1.begin() + i + tam_seg);
                    r1.erase(r1.begin() + i, r1.begin() + i + tam_seg);

                    custo_anterior -= custo_atual;
                    custo_anterior += custo_novo;
                    custo_atual = custo_novo;
                }
            }
        }

        return custo_anterior;
    }

    int VND_Inter(vector<int>& rota1, vector<int>& rota2, int custo_anterior){
        int novo_custo, k = 1;

        // Teremos 4 estruturas de vizinhanca
        while(k < 5){

            switch(k){
                case 1: novo_custo = VND_Inter_Swap(rota1, rota2, custo_anterior); break;
                case 2: novo_custo = VND_Inter_Relocate(rota1, rota2, custo_anterior); break;
                case 3: novo_custo = VND_Inter_Swap2x2(rota1, rota2, custo_anterior); break;
                case 4: novo_custo = VND_Inter_NOpt1(rota1, rota2, custo_anterior); break;
            }

            if(novo_custo < custo_anterior){
                k = 1;
                custo_anterior = novo_custo;
            }
            else{
                k++;
            }

        }

        return custo_anterior;
    }

    int VND1(vector<vector<int>> &rotas, int custo_inicial = -1){
        bool melhorou = true;
        int novo_custo;
        int custo_anterior = custo_inicial == -1 ? CustoTotal(rotas) : custo_inicial;

        while(melhorou){
            melhorou = false;

            // VND INTRA-ROTA
            for(int i = 0; i < rotas.size(); i++){
                novo_custo = VND_Intra(rotas[i], custo_anterior);

                if(novo_custo < custo_anterior){
                    melhorou = true;
                    custo_anterior = novo_custo;
                }

                // cout << "Intra aplicado a " << i << endl;
            }

            VerificaSolucao(rotas, true);

            // VND INTER-ROTAS
            for(int i = 0; i < rotas.size(); i++){
                for(int j = i+1; j < rotas.size(); j++){
                    novo_custo = VND_Inter(rotas[i], rotas[j], custo_anterior);

                    if(novo_custo < custo_anterior){
                        melhorou = true;
                        custo_anterior = novo_custo;
                    }

                    // cout << "Inter aplicado a " << i << " e " << j << endl;
                }
            }
        }

        return custo_anterior;
    }

#endif