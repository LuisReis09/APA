#ifndef _VND1_HPP_
#define _VND1_HPP_

    #include <iostream>
    #include <stdlib.h>
    #include <vector>
    #include <algorithm>
    #include "utils.hpp"
    #include "structures.hpp"

    using namespace std;

    bool VND_Intra_Swap(vector<int>& rota){
        int custo_swap, custo_atual;
        int n = rota.size();
        bool achou_melhora = false;

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
                        achou_melhora = true;
                    }
                }
            }
        }

        return achou_melhora;
    }

    bool VND_Intra_ShiftN(vector<int>& rota) {
        int nMax = min((int) rota.size() - 2, 5);
        
        int melhor_custo = CustoRota(rota);
        int melhor_i = -1, melhor_j, melhor_tamseg;

        for (int i = 1; i < (int)rota.size() - 2; i++) {
            int max_seg = min(nMax, (int) rota.size() - i - 1);
            if (max_seg < 2) continue;

            int tam_seg = RandomEntre(2, max_seg);

            for (int j = i + tam_seg + 1; j < (int)rota.size() - 1; j++) {

                int custo_teste = VerificaIntraShiftn(rota, i, j, tam_seg);

                if (custo_teste != -1) {
                    if(custo_teste < melhor_custo){
                        melhor_custo = custo_teste;
                        melhor_i = i;
                        melhor_j = j;
                        melhor_tamseg = tam_seg;
                    }
                }
            }
        }

        if (melhor_i != -1) {
            rota.insert(rota.begin() + melhor_j, rota.begin() + melhor_i, rota.begin() + melhor_i + melhor_tamseg);
            rota.erase(rota.begin() + melhor_i, rota.begin() + melhor_i + melhor_tamseg);
            return true;
        }

        return false;
    }

    bool VND_Intra_Reinsertion_BestChoice(vector<int>& rota){
        int melhor_posicao, melhor_custo;
        int custo_remocao, custo_insercao, delta;
        bool achou_melhora = false;

        for(int i = 1; i < rota.size() - 1; i++){
            melhor_custo = MAXX_INT;
            melhor_posicao = -1;

            for(int j = 1; j < rota.size(); j++){
                if(i == j || j == i+1) continue;

                // Custo da remocao
                custo_remocao = p.matriz_custo[rota[i-1]][rota[i+1]] - p.matriz_custo[rota[i-1]][rota[i]] - p.matriz_custo[rota[i]][rota[i+1]];

                // Custo da Insercao
                custo_insercao = p.matriz_custo[rota[j-1]][rota[i]] + p.matriz_custo[rota[i]][rota[j]] - p.matriz_custo[rota[j-1]][rota[j]];

                delta = custo_remocao + custo_insercao;

                // Se o custo compensar e for possivel de atender às demandas
                if(delta < 0 && ReinsertionTest(rota, i, j)){
                    
                    if(delta < melhor_custo){
                        melhor_custo = delta;
                        melhor_posicao = j;
                    }
                }
            }

            if(melhor_posicao != -1){
                int estacao = rota[i];
                rota.erase(rota.begin() + i);
                if(i < melhor_posicao)
                    rota.insert(rota.begin() + melhor_posicao - 1, estacao);
                else
                    rota.insert(rota.begin() + melhor_posicao, estacao);

                achou_melhora = true;
            }
        }

        return achou_melhora;
    }

    bool VND_Intra_Reinsertion_FirstChoice(vector<int>& rota){
        int custo_remocao, custo_insercao, delta;
        int custo_backup = CustoRota(rota);
        bool achou_melhora = false;

        for(int i = 1; i < rota.size() - 1; i++){

            for(int j = 1; j < rota.size(); j++){
                if(i == j || j == i+1) continue;

                // Custo da remocao
                custo_remocao = p.matriz_custo[rota[i-1]][rota[i+1]] - p.matriz_custo[rota[i-1]][rota[i]] - p.matriz_custo[rota[i]][rota[i+1]];

                // Custo da Insercao
                custo_insercao = p.matriz_custo[rota[j-1]][rota[i]] + p.matriz_custo[rota[i]][rota[j]] - p.matriz_custo[rota[j-1]][rota[j]];

                delta = custo_remocao + custo_insercao;

                // Se o custo compensar e for possivel de atender às demandas
                if(delta < 0 && ReinsertionTest(rota, i, j)){
                    int estacao = rota[i];
                    achou_melhora = true;

                    rota.erase(rota.begin() + i);

                    if(i < j)
                        rota.insert(rota.begin() + j - 1, estacao);
                    else
                        rota.insert(rota.begin() + j, estacao);

                }
            }
        }

        return achou_melhora;
    }

    bool VND_Intra_Inversion(vector<int> &rota){
        int custo_inversion, custo_atual;
        bool achou_melhora = false;

        custo_atual = CustoRota(rota);

        for(int i = 1; i < rota.size() - 2; i++){
            for(int j = i+1; j < rota.size() - 1; j++){
                custo_inversion = TentaInverter(rota, i, j);

                // Se o custo veio -1, significa que nao pode devido as demandas
                if(custo_inversion != -1){

                    // Se o custo compensar, invertemos
                    if(custo_inversion < custo_atual){
                        reverse(rota.begin() + i, rota.begin() + j + 1);
                        custo_atual = custo_inversion;

                        achou_melhora = true;
                    }
                }
            }
        }

        return achou_melhora;
    }

    bool VND_Intra(vector<int>& rota){
        bool melhorou;
        bool intra_melhorou = false;
        int k = 1;

        // Teremos 5 estruturas de vizinhanca
        while(k < 6){

            switch(k){
                case 1: melhorou = VND_Intra_Swap(rota); break;
                case 2: melhorou = VND_Intra_Inversion(rota); break;
                case 3: melhorou = VND_Intra_ShiftN(rota); break;
                case 4: melhorou = VND_Intra_Reinsertion_BestChoice(rota); break;
                case 5: melhorou = VND_Intra_Reinsertion_FirstChoice(rota); break;
            }

            // cout << "Intra aplicou " << k << ": " << (melhorou? "melhorou" : "nao melhorou") << endl;
            // cout << "Custo: " << CustoRota(rota) << endl;

            if(melhorou){
                k = 1;
                intra_melhorou = true;
            }
            else{
                k++;
            }
        }

        return intra_melhorou;
    }

    bool VND_Inter_Swap(vector<int> &r1, vector<int> &r2){
        int custo_atual, custo_swap;
        bool achou_melhora = false;

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
                        achou_melhora = true;
                    }
                }
            }
        }

        return achou_melhora;
    }

    bool VND_Inter_Relocate(vector<int> &r1, vector<int> &r2){
        int custo_novo, custo_atual;
        bool achou_melhora = false;

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
                        achou_melhora = true;
                    }
                }
            }
        }

        return achou_melhora;
    }

    bool VND_Inter_Swap2x2(vector<int> &r1, vector<int> &r2){
        int custo_novo, custo_atual;
        bool achou_melhora = false;

        for(int i = 1; i < r1.size() - 2; i++){
            for(int j = 1; j < r2.size() - 2; j++){

                // O que temos:
                custo_atual = p.matriz_custo[r1[i-1]][r1[i]] + p.matriz_custo[r1[i+1]][r1[i+2]] + p.matriz_custo[r2[j-1]][r2[j]] + p.matriz_custo[r2[j+1]][r2[j+2]];

                // O que teriamos ao dar swap:
                custo_novo =  p.matriz_custo[r1[i-1]][r2[j]] + p.matriz_custo[r2[j+1]][r1[i+2]] + p.matriz_custo[r2[j-1]][r1[i]] + p.matriz_custo[r1[i+1]][r2[j+2]];

                if(custo_novo < custo_atual){
                    swap(r1[i], r2[j]);
                    swap(r1[i+1], r2[j+1]);

                    if(!VerificaDemanda(r1) || !VerificaDemanda(r2)){
                        // Desfaz o swap
                        swap(r1[i], r2[j]);
                        swap(r1[i+1], r2[j+1]);
                        continue;
                    }

                    achou_melhora = true;
                }
            }
        }

        return achou_melhora;
    }

    bool VND_Inter_NOpt1(vector<int> &r1, vector<int> &r2){
        int custo_novo, custo_atual;
        int max_seg, tam_seg;
        bool achou_melhora = false;

        custo_atual = CustoRota(r1) + CustoRota(r2);

        for(int i = 1; i < r1.size() - 2; i++){
            max_seg = r1.size() - i - 1;
            tam_seg = RandomEntre(2, max_seg);

            for(int j = 1; j < r2.size() - 1; j++){
                custo_novo = VerificaInterNOpt(r1, r2, j, i, tam_seg);

                if(custo_novo != -1 && custo_novo < custo_atual){
                    r2.insert(r2.begin() + j, r1.begin() + i, r1.begin() + i + tam_seg);
                    r1.erase(r1.begin() + i, r1.begin() + i + tam_seg);

                    custo_atual = custo_novo;
                    achou_melhora = true;
                }
            }
        }

        return achou_melhora;
    }

    bool VND_Inter(vector<int>& rota1, vector<int>& rota2){
        int k = 1;
        bool melhorou;
        bool inter_melhorou = false;

        // Teremos 4 estruturas de vizinhanca
        while(k < 5){

            switch(k){
                case 1: melhorou = VND_Inter_Swap(rota1, rota2); break;
                case 2: melhorou = VND_Inter_Relocate(rota1, rota2); break;
                case 3: melhorou = VND_Inter_Swap2x2(rota1, rota2); break;
                case 4: melhorou = VND_Inter_NOpt1(rota1, rota2); break;
            }

            // cout << "Inter aplicou metodo " << k << endl;

            if(melhorou){
                k = 1;
                inter_melhorou = true;
            }
            else{
                k++;
            }
        }

        return inter_melhorou;
    }

    int VND1(vector<vector<int>> &rotas){
        bool melhorou = true;
        int novo_custo;

        while(melhorou){

            // VND INTRA-ROTA
            for(int i = 0; i < rotas.size(); i++){
                melhorou = VND_Intra(rotas[i]);

                // cout << "Intra aplicado a " << i << endl;
            }

            // VND INTER-ROTAS
            for(int i = 0; i < rotas.size(); i++){
                for(int j = i+1; j < rotas.size(); j++){
                    melhorou |= VND_Inter(rotas[i], rotas[j]);

                    // cout << "Inter aplicado a " << i << " e " << j << endl;
                }
            }

            RetiraRotasVazias(rotas);
        }

        return CustoTotal(rotas);
    }

#endif