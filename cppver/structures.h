#ifndef STRUCTURES_H
#define STRUCTURES_H

class Problema{
    public:
        int qnt_veiculos;
        int capacidade_max;
        int* demandas;      // Array de inteiros
        int qnt_estacoes;
        int veiculos_disponiveis;
};

class No{
    public:
        int estacao;
        No* proximo;
        int soma_demandas_d1;
        int soma_demandas_d2;
        int custo_d1, custo_d2;
};

class Rota{
    public:
        No* rota_i;      // Exemplo: [0, 1, 5, 7, 2, 8, 0] ENCADEADA, pq vamos precisar fzr muitas remoções e inserções, que podem ser no meio da lista
        No* rota_f;      // Último elemento da lista encadeada
        int custo_total_d1, custo_total_d2;
        int direcao_atual;
};

class Solucao{
    int custo_total;
    int veiculos_usados;
    int veiculos_disponiveis;
    Rota* rotas;
};

#endif