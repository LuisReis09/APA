class Problema:
    qtd_veiculos:           int
    capacidade_max:         int
    demandas:               list[int]
    matriz_distancias:      list[list[int]]
    qtd_estacoes:           int
    veiculos_disponiveis:   int

class No:
    estacao:    int
    proximo:    'No' | None
    soma_demandas_d1: int
    soma_demandas_d2: int
    custo_d1: int
    custo_d2: int

class Rota:
    rota_i:                      No         # Exemplo: [0, 1, 5, 7, 2, 8, 0] ENCADEADA, pq vamos precisar fzr muitas remoções e inserções, que podem ser no meio da lista
    rota_f:                      No         # Último elemento da lista encadeada
    custo_total_d1:              int              
    custo_total_d2:              int
    direcao_atual:               int               # 1 ou 2, indicando a direção atual da rota

    """
        Se direcao_atual == 1:
            [0, 1, 2, ..., n-1], sendo n-1 o último elemento da lista `rota`
        Se direcao_atual == 2:
            [n-1, n-2, n-3, ..., 0], sendo n-1 o último elemento da lista `rota`
    """
    
class Solucao:
    rotas:                  list[Rota]
    custo_total:            int
    veiculos_usados:        int
    veiculos_disponiveis:   int
    