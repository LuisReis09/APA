#ifndef _VNDHERICK_HPP_
#define _VNDHERICK_HPP_

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

    /**
     * @brief Para cada combinação de termos em uma rota, as troca e checa se a nova combinação é mais barata
     * ---
     * @param rota Rota a ser analisada e modificada
     *
     * @return Retorna uma nova rota modificada
     */
    vector<int> VND_Swap(const vector<int> &rota)
    {
        vector<int> rota_ret = rota; // copia a rota antiga
        int melhor_custo = CalculaCusto(rota_ret);
        int custo_swap = 0;

        for (int i = 1; i < rota_ret.size() - 2; i++)
        {
            for (int j = i + 1; j < rota_ret.size() - 1; j++)
            {
                // Para cada combinação de termos em uma dada rota
                vector<int> rota_teste = rota_ret;

                // Os troca
                swap(rota_teste[i], rota_teste[j]);

                // Calcula o custo
                custo_swap = CalculaCusto(rota_teste);

                // Se for menor que o custo atual e for uma rota valida, substitui a rota
                if (custo_swap < melhor_custo && VerificaDemanda(rota_teste))
                {
                    rota_ret = rota_teste;
                    melhor_custo = custo_swap;
                }
            }
        }
        // Retorna rota modificada
        return rota_ret;
    }

    /**
     * @brief Algoritmo parte do VND. Para todas as posições em uma rota, checa se mover um termo de posição é benefico ao custo
     *
     * @param rota Rota a ser modificada
     * ---
     * @return Retorna a rota modificada
     */
    vector<int> VND_ReInsertion(const vector<int> &rota)
    {
        vector<int> rota_ret = rota;
        int melhor_custo = CalculaCusto(rota);
        int custo_reinsertion = 0;

        for (int i = 1; i < rota_ret.size() - 1; i++)
        {
            for (int j = i + 1; j < rota_ret.size() - 1; j++)
            {
                // Para todas as combinações de posição nesta rota
                vector<int> rota_teste = rota_ret;

                // Apaga o termo de sua posição original e a re-insere em outra
                rota_teste.erase(rota_teste.begin() + i);
                rota_teste.insert(rota_teste.begin() + j - 1, rota_ret[i]);

                // Calcula o custo dessa operação
                custo_reinsertion = CalculaCusto(rota_teste);

                // Se for menor que o atual e a rota for válida, substitui a original
                if (custo_reinsertion < melhor_custo && VerificaDemanda(rota_teste))
                {
                    rota_ret = rota_teste;
                    melhor_custo = custo_reinsertion;
                }
            }
        }
        // Retorna rota modificada
        return rota_ret;
    }

    vector<int> VND_Shift_N(const vector<int> &rota)
    {
        vector<int> rota_ret = rota;
        int melhor_custo = CalculaCusto(rota);

        int nMax = rota.size() - 2;
        if (nMax > 4)
            nMax = 4; // limite no tamanho do bloco

        for (int i = 1; i < rota_ret.size() - 1; i++)
        {
            for (int j = i + 1; j < rota_ret.size() - 1; j++)
            {
                int max_seg = min(nMax, (int)rota_ret.size() - 2 - i);
                if (max_seg < 1)
                    continue;

                int segmento = 1 + rand() % max_seg;

                vector<int> bloco(rota_ret.begin() + i, rota_ret.begin() + i + segmento);

                vector<int> rota_teste = rota_ret;
                rota_teste.erase(rota_teste.begin() + i, rota_teste.begin() + i + segmento);

                int pos = j - segmento;
                if (pos < 1)
                    pos = 1;
                if (pos > (int)rota_teste.size() - 1)
                    pos = rota_teste.size() - 1;

                rota_teste.insert(rota_teste.begin() + pos, bloco.begin(), bloco.end());

                int custo_shift = CalculaCusto(rota_teste);
                if (custo_shift < melhor_custo && VerificaDemanda(rota_teste))
                {
                    rota_ret = rota_teste;
                    melhor_custo = custo_shift;
                }
            }
        }
        return rota_ret;
    }

    /**
     * @brief Algoritmo parte do VND. Para todos os subintervalos da rota, os reverte e vê se é benefico ao custo, se sim, aplica a mudança
     *
     * @param rota Rota a ser modificada
     * ---
     * @return Retorna a rota modificada
     */
    vector<int> VND_Inversion(const vector<int> &rota)
    {
        vector<int> rota_ret = rota;
        int melhor_custo = CalculaCusto(rota_ret);
        int custo_inversion = 0;

        for (int i = 1; i < rota_ret.size() - 1; i++)
        {
            for (int j = i + 1; j < rota_ret.size() - 1; j++)
            {
                // Para toda sub-fatia da rota...

                vector<int> rota_teste = rota_ret;

                // inverter o intervalo [i, j] na nova rota
                reverse(rota_teste.begin() + i, rota_teste.begin() + j);

                // recalcular o custo da rota_teste
                custo_inversion = CalculaCusto(rota_teste);

                // Se for menor que o atual e a rota for válida, substitui a original
                if (custo_inversion < melhor_custo && VerificaDemanda(rota_teste))
                {
                    rota_ret = rota_teste;
                    melhor_custo = custo_inversion;
                }
            }
        }

        return rota_ret;
    }

    /**
     * @brief Algoritmo VND. Aplica as funções VND_Swap, VND_ReInsertion e VND_Inversion em ordem para melhorar custo de uma rota, termina apenas quando as três forem bem sucedidas em melhorar o custo.
     *
     * @param rota Rota a ser modificada e ter seu custo melhorado
     * ---
     */
    void VND(vector<int> &rota)
    {
        // Custo vigente da rota
        int melhor_custo = CalculaCusto(rota);

        // Funções a serem aplicadas
        vector<function<vector<int>(vector<int>)>> estruturas_VND = {
            VND_Shift_N,
            VND_Swap,
            VND_Inversion,
            VND_ReInsertion,
        };

        int k = 0; // estrutura swap primeiro
        int N = estruturas_VND.size();
        while (k < N)
        {
            // Aplica a função atual a rota
            vector<int> nova_rota = estruturas_VND[k](rota);
            int novo_custo = CalculaCusto(nova_rota);

            // Se o novo custo for menor que o atual, reseta o contador de funções para melhorar ainda mais a rota
            if (novo_custo < melhor_custo)
            {
                rota = nova_rota;
                melhor_custo = novo_custo;
                k = 0;
            }
            else // Se não for, passa para a proxima função
            {
                k++;
            }
        }

        return;
    }

    /**
     * @brief Algoritmo VND, mas que retorna o custo melhorado da rota.
     *
     * @param rota Rota a ser modificada
     * ---
     * @return Retorna o custo melhorado da rota
     */
    int VNDIntra(vector<int> &rota)
    {
        // Custo vigente da rota
        int melhor_custo = CalculaCusto(rota);

        // Funções a serem aplicadas
        vector<function<vector<int>(vector<int>)>> estruturas_VND = {
            VND_Swap,
            VND_Shift_N,
            VND_ReInsertion,
            VND_Inversion,
        };

        int k = 0; // estrutura swap primeiro
        int N = estruturas_VND.size();
        while (k < N)
        {
            // Aplica a função atual a rota
            vector<int> nova_rota = estruturas_VND[k](rota);
            int novo_custo = CalculaCusto(nova_rota);

            // Se o novo custo for menor que o atual, reseta o contador de funções para melhorar ainda mais a rota
            if (novo_custo < melhor_custo)
            {
                rota = nova_rota;
                melhor_custo = novo_custo;
                k = 0;
            }
            else // Se não for, passa para a proxima função
            {
                k++;
            }
        }

        return melhor_custo;
    }

    /**
     * @brief Algoritmo VND que aplicará o VND individualmente para cada rota da solução.
     *
     * @param rota Rotas a serem modificadas.
     * ---
     */
    void VND(vector<vector<int>> &rotas)
    {
        int n_rotas = rotas.size();

        // distribuir as rotas nas threads
        for (int i = 0; i < n_rotas; i++)
        {
            int custo_rota_i = CalculaCusto(rotas[i]);
            VND(rotas[i]);
        }
        return;
    }

    /**
     * @brief Algoritmo VND Inter-rotas que troca dois termos entre rotas e verifica se o custo é melhor.
     *
     * @param r1 Primeira rota analisada.
     * @param r2 Segunda rota analisada.
     * ---
     * @return Retorna o custo melhorado da rota
     */
    int VNDInterSwap(vector<int> &r1, vector<int> &r2)
    {
        if (r1.size() < 3 || r2.size() < 3)
            return CalculaCusto(r1) + CalculaCusto(r2);

        int melhor_custo = CalculaCusto(r1) + CalculaCusto(r2);
        int novo_custo;

        // percorre a rota r1 (ignorando a garagem nas pontas)
        for (int i = 1; i < r1.size() - 1; i++)
        {
            // percorre a rota r2 (ignorando a garagem nas pontas)
            for (int j = 1; j < r2.size() - 1; j++)
            {

                vector<int> copy_r1 = r1, copy_r2 = r2;
                swap(copy_r1[i], copy_r2[j]);

                novo_custo = CalculaCusto(copy_r1) + CalculaCusto(copy_r2);

                if (novo_custo < melhor_custo)
                {
                    // se o custo for melhor, cria cópias e realiza, de fato, o swap

                    // usa essas copias para verificar se as demandas desse swap são válidas
                    if (VerificaDemanda(copy_r1) && VerificaDemanda(copy_r2))
                    {
                        // atualiza as rotas e o melhor custo para a melhor versão até o momento
                        melhor_custo = novo_custo;
                        r1 = copy_r1;
                        r2 = copy_r2;
                    }
                }
            }
        }

        return melhor_custo;
    }

    /**
     * @brief Algoritmo VND Inter-rotas que pega uma fatia de uma rota r1 e a insere em uma rota r2, então verificando se é benefico ao custo
     *
     * @param r1 Primeira rota analisada.
     * @param r2 Segunda rota analisada.
     * ---
     * @return Retorna o custo melhorado da rota
     */
    int VNDInterRelocate(vector<int> &r1, vector<int> &r2)
    {
        if (r1.size() < 3 || r2.size() < 3)
            return CalculaCusto(r1) + CalculaCusto(r2);

        // Custo combinado entre rotas
        int melhor_custo = CalculaCusto(r1) + CalculaCusto(r2);
        int novo_custo;

        // percorre a rota r1 (ignorando a garagem nas pontas)
        for (int i = 1; i < r1.size() - 1; i++)
        {
            // percorre a rota r2 (ignorando a garagem nas pontas)
            for (int j = 1; j < r2.size() - 1; j++)
            {
                vector<int> copy_r1 = r1, copy_r2 = r2;

                // Insere a fatia de r1 em r2
                // E apaga esta mesma fatia de r1
                copy_r2.insert(copy_r2.begin() + j, copy_r1[i]);
                copy_r1.erase(copy_r1.begin() + i);

                novo_custo = CalculaCusto(copy_r1) + CalculaCusto(copy_r2);

                // Se o novo custo for menor...
                if (novo_custo < melhor_custo)
                {
                    // Checa se ambas rotas são viaveis, se sim, atualiza a rota real
                    if (VerificaDemanda(copy_r1) && VerificaDemanda(copy_r2))
                    {
                        melhor_custo = novo_custo;
                        r1 = copy_r1;
                        r2 = copy_r2;
                    }
                }
            }
        }

        return melhor_custo;
    }

    /**
     * @brief Algoritmo VND Inter-rotas que troca duas fatias das rotas r1 e r2 as troca entrei, verificando se o custo é menor.
     *
     * @param r1 Primeira rota analisada.
     * @param r2 Segunda rota analisada.
     * ---
     * @return Retorna o custo melhorado da rota
     */
    int VNDInterCrossover(vector<int> &r1, vector<int> &r2)
    {
        // Ignora rotas vazias
        if (r1.size() < 3 || r2.size() < 3)
            return CalculaCusto(r1) + CalculaCusto(r2);

        // Custo combinado das rotas
        int melhor_custo = CalculaCusto(r1) + CalculaCusto(r2);
        int novo_custo;
        vector<int> copy_r1, copy_r2;
        srand(time(NULL)); // seed para aleatorizar o tamanho do segmento

        int limite = r1.size() > r2.size() ? r2.size() - 3 : r1.size() - 3;

        // para cada rota, tenta fazer um swap de segmentos:
        for (int i = 1; i < r1.size() - 1; i++)
        {
            for (int j = 1; j < r2.size() - 1; j++)
            {

                // calcula tamanho máximo da fatia
                int max_tam = min({limite, (int)r1.size() - i - 1, (int)r2.size() - j - 1});
                if (max_tam < 2)
                    break; // não dá pra formar segmento válido a partir daqui
                int tamanho_seg = 2 + rand() % (max_tam - 1);

                copy_r1 = r1;
                copy_r2 = r2;
                // Troca termo a termo das fatias achadas
                for (int k = 0; k < tamanho_seg; k++)
                {
                    swap(copy_r1[i + k], copy_r2[j + k]);
                }

                novo_custo = CalculaCusto(copy_r1) + CalculaCusto(copy_r2);

                // Se o custo obtido for menor que o atual e ambas as novas rotas foram válidas, altera
                if (novo_custo < melhor_custo && VerificaDemanda(copy_r1) && VerificaDemanda(copy_r2))
                {
                    melhor_custo = novo_custo;
                    r1 = copy_r1;
                    r2 = copy_r2;
                }
            }
        }

        return melhor_custo;
    }

    /**
     * @brief Algoritmo VND aplicado entre duas rotas. Re-insere uma fatia de 1 a n elementos de r1 em r2
     *
     * ---
     * @param r1 Primeira rota a ser modificada
     * @param r2 Segunda rota a ser modificada
     * ---
     *
     * @return Retorna o custo melhorado da solução após o VND entre essas duas rotas
     */
    int VNDInterNOpt1(vector<int> &r1, vector<int> &r2)
    {
        // cout << "[VNDInterNOpt1]: " << endl;

        // se alguma rota não tiver pelo menos duas estacoes, pula
        if (r1.size() < 4 || r2.size() < 4)
        {
            return CalculaCusto(r1) + CalculaCusto(r2);
        }

        // cout << "Tamanho de r1: " << r1.size() << endl;
        // cout << "Tamanho de r2: " << r2.size() << endl;

        // Custo combinado das rotas
        int melhor_custo = CalculaCusto(r1) + CalculaCusto(r2);
        int novo_custo;
        vector<int> copy_r1, copy_r2;

        int max_seg = r1.size() - 3; // no máximo até antes do depósito
        if (max_seg < 4)
            return melhor_custo; // não tem segmento válido

        int tamanho_seg = 2 + rand() % (max_seg - 3);

        // Para toda combinação de segmentos entre r1 e r2
        for (int i = 1; i + tamanho_seg < r1.size() - 1; i++)
        {
            for (int j = 1; j < r2.size() - 1; j++)
            {
                copy_r1 = r1;
                copy_r2 = r2;

                if (i + tamanho_seg >= copy_r1.size() - 1)
                {
                    continue;
                }

                // cout << "i = " << i << ", j = " << j << ", tam_seg = " << tamanho_seg << endl;
                copy_r2.insert(copy_r2.begin() + j, copy_r1.begin() + i, copy_r1.begin() + i + tamanho_seg);
                copy_r1.erase(copy_r1.begin() + i, copy_r1.begin() + i + tamanho_seg);

                novo_custo = CalculaCusto(copy_r1) + CalculaCusto(copy_r2);

                // Se o custo obtido for menor que o atual e ambas as novas rotas foram válidas, altera
                if (novo_custo < melhor_custo && VerificaDemanda(copy_r1) && VerificaDemanda(copy_r2))
                {
                    melhor_custo = novo_custo;
                    r1 = copy_r1;
                    r2 = copy_r2;
                }
            }
        }

        // cout << "OK" << endl;
        return melhor_custo;
    }

    /**
     * @brief Algoritmo parte do VND. Cria uma perturbação ao trocar duas duplas entre rotas.
     *
     * ---
     * @param r1 Primeira rota a ser modificada
     * @param r2 Segunda rota a ser modificada
     * ---
     *
     * @return Retorna o custo melhorado da solução após o VND entre essas duas rotas
     */
    int VNDInterSwap2x2(vector<int> &r1, vector<int> &r2)
    {
        // cout << "swap2x2: " << endl;
        if (r1.size() < 3 || r2.size() < 3)
        {
            return CalculaCusto(r1) + CalculaCusto(r2);
        }

        // cout << "Tamanho de r1: " << r1.size() << endl;
        // cout << "Tamanho de r2: " << r2.size() << endl;

        // Custo combinado das rotas
        int melhor_custo = CalculaCusto(r1) + CalculaCusto(r2);
        int novo_custo;
        vector<int> copy_r1, copy_r2;

        // para toda combinação de termos de r1 e r2...
        for (int i = 1; i < r1.size() - 2; i++)
        {
            for (int j = 1; j < r2.size() - 2; j++)
            {
                copy_r1 = r1;
                copy_r2 = r2;

                // cout << "i = " << i << ", j = " << j << endl;
                // Troca duas duplas entre as duas rotas
                swap(copy_r1[i], copy_r2[j]);
                swap(copy_r1[i + 1], copy_r2[j + 1]);

                // Calcula o novo custo
                novo_custo = CalculaCusto(copy_r1) + CalculaCusto(copy_r2);

                // Se o custo obtido for menor que o atual e ambas as novas rotas foram válidas, altera
                if (novo_custo < melhor_custo && VerificaDemanda(copy_r1) && VerificaDemanda(copy_r2))
                {
                    melhor_custo = novo_custo;
                    r1 = copy_r1;
                    r2 = copy_r2;
                }
            }
        }
        // cout << "OK" << endl;
        return melhor_custo;
    }

    /**
     * @brief Algoritmo VND aplicado entre duas rotas. Aplica as funções VNDInterSwap, VNDInterRelocate e VNDInterCrossover em ordem para melhorar custo de uma rota, termina apenas quando as três forem bem sucedidas em melhorar o custo.
     * Usado em conjunto com VND intra rotas apresentado anteriormente
     * @param r1 Primeira rota a ser modificada
     * @param r2 Segunda rota a ser modificada
     * ---
     *
     * @return Retorna o custo melhorado da solução após o VND entre essas duas rotas
     */
    int VNDInter(vector<int> &r1, vector<int> &r2)
    {
        // Custo combinado atual das duas rotas
        int melhor_custo = CalculaCusto(r1) + CalculaCusto(r2);
        int novo_custo;

        // Funções a serem aplicadas
        vector<function<int(vector<int> &, vector<int> &)>> estruturasInterRota = {
            VNDInterSwap,
            VNDInterRelocate,
            VNDInterSwap2x2,
            VNDInterCrossover,
            VNDInterNOpt1,
        };

        int k = 0;
        int N = estruturasInterRota.size();
        while (k < N)
        {
            novo_custo = estruturasInterRota[k](r1, r2);
            if (novo_custo < melhor_custo)
            {
                melhor_custo = novo_custo;
                k = 0;
            }
            else
            {
                k++;
            }

            // se em alguma das estruturas uma delas ficar vazia, retorna, pois só haverá uma rota, afinal
            if (r1.size() < 3 || r2.size() < 3)
            {
                return melhor_custo;
            }
        }

        return melhor_custo;
    }

    /**
     * @brief Algoritmo VND que aplica primeiro o VND Intra-rota e depois o VND Inter-rotas.
     * @param rotas Vetor de vetor de inteiros. Rotas de uma dada solução
     * ---
     */
    void VNDIntraInter(vector<vector<int>> &rotas)
    {
        int custo_anterior, novo_custo;
        bool melhorou = true;

        while (melhorou) // Enquanto houver melhora, continue
        {
            melhorou = false;

            // VND INTRA-ROTA
            for (int i = 0; i < (int)rotas.size(); i++)
            { // Rota a rota, aplica o VND intrarota nelas
                custo_anterior = CalculaCusto(rotas[i]);
                novo_custo = VNDIntra(rotas[i]); // Esta função já altera as rotas em si

                // Se o custo for melhor, seta que melhora é true
                if (novo_custo < custo_anterior)
                {
                    // cout << "Melhora na rota " << i << "! Diferenca: " << novo_custo - custo_anterior << endl;
                    melhorou = true;
                }
            }

            // vnd inter-rotas
            // vamos coletar índices a remover e só apagar depois
            for (int i = 0; i < (int)rotas.size() - 1; i++)
            {
                for (int j = i + 1; j < (int)rotas.size(); j++)
                {
                    // Para cada combinação de rotas, aplica o VND Inter-rotas
                    custo_anterior = CalculaCusto(rotas[i]) + CalculaCusto(rotas[j]);
                    novo_custo = VNDInter(rotas[i], rotas[j]); // Esta função já altera as rotas em si

                    if (novo_custo < custo_anterior)
                    {
                        // cout << "Melhora nas rotas " << i << " e " << j << " ! Diferenca: " << novo_custo - custo_anterior << endl;
                        melhorou = true;
                    }
                }
            }

            // opcional: mostrar custo total aproximado (soma das rotas)
            // cout << "[VND] Custo total: " << CustoTotal(rotas) << " | rotas: " << rotas.size() << endl;
        }
    }

#endif