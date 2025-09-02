import numpy as np
import matplotlib.pyplot as plt
import networkx as nx

def ImprimirGrafo(matriz, nodos_destaque=[]):
    m, n = matriz.shape
    G = nx.Graph()
    
    # adiciona arestas
    for i in range(m):
        for j in range(i+1, n):  # evita duplicação
            if matriz[i][j] != 0:
                G.add_edge(i, j, weight=matriz[i][j])
    
    # Desenha o grafo, destacando os nós especificados com uma cor diferente
    pos = nx.spring_layout(G, seed=42, k=2, scale=10)  # layout fixo
    node_colors = ["lightgreen" if i in nodos_destaque else "skyblue" for i in range(n)]
    nx.draw(G, pos, with_labels=True, node_size=800, node_color=node_colors, font_size=12, font_weight="bold")

    # desenha os pesos
    labels = nx.get_edge_attributes(G, "weight")
    nx.draw_networkx_edge_labels(G, pos, edge_labels=labels)
    plt.savefig("graph.png")


def MatrizAleatoria(m, n, max=100):
    matriz = np.zeros((m, n), dtype=int)
    for i in range(m):
        for j in range(i+1, n):
            matriz[i][j] = matriz[j][i] = np.random.randint(1, max + 1)
    return matriz

def LerDados(caminho_arquivo = "exemplo1.txt"):
    """
        Lê os dados do arquivo no formato:
            Primeira linha: número de estações (fora o galpão)
            Segunda linha: número de caminhões
            Terceira linha: capacidade máxima de cada caminhão
            Quarta linha: necessidades de cada estação (fora o galpão)
            Linhas seguintes: matriz de adjacência (distâncias entre as estações, incluindo o galpão)
    """

    with open(caminho_arquivo, 'r') as f:
        # Primeira linha: número de estações (fora o galpão)
        estacoes = int(f.readline().strip())

        # Segunda linha: número de caminhões
        caminhoes = int(f.readline().strip())

        # Terceira linha: capacidade máxima de cada caminhão
        capmax_caminhao = int(f.readline().strip())

        # Pula a linha em branco, se houver
        linha = f.readline().strip()

        # Quarta linha: necessidades de cada estação (fora o galpão)
        necessidades = list(map(int, f.readline().strip().split()))

        # Pula a linha em branco, se houver
        linha = f.readline().strip()

        # Linhas seguintes: matriz de adjacência (distâncias entre as estações, incluindo o galpão)
        matriz = np.zeros((estacoes + 1, estacoes + 1), dtype=int)
        for i in range(estacoes + 1):
            linha = list(map(int, f.readline().strip().split()))
            matriz[i] = linha

    if len(necessidades) == estacoes:
        necessidades = [0] + necessidades
            
    return estacoes, caminhoes, capmax_caminhao, necessidades, matriz
    
    


def TestaConjunto(matriz, conjunto: int):
    """
        Retorna a diferença entre as distâncias entre as estações e as distâncias ao galpão
    """

    dist_ao_galpao, dist_entre_estacoes = 0, 0
    i = 1
    last = 0

    while conjunto:
        if conjunto & 1:
            dist_ao_galpao += matriz[0][i]
            
            if last != 0:
                last = i
            else:
                dist_entre_estacoes += matriz[last][i]
                last = i

        i += 1
        conjunto >>= 1

    return dist_entre_estacoes - dist_ao_galpao
        
def CalculaMelhorConjunto(matriz, qtd_caminhoes):
    """
        Retorna o conjunto de índices (lista) que maximiza a diferença entre as distâncias entre as estações e minimiza as distâncias ao galpão
        Usa uma abordagem de geração de subconjuntos com a mesma quantidade de bits 1
        A lista conterá qtd_caminhoes elementos, que serão as primeiras estações (índices) a serem visitadas por cada caminhão
    """

    # Retiramos o galpão da contagem
    qtd_conjunto = matriz.shape[0] - 1

    diff_atual, diff_teste = -float("inf"), 0
    melhor_conjunto, mask_teste = 0, (1 << (qtd_caminhoes)) - 1

    while mask_teste < (1 << qtd_conjunto):
        diff_teste = TestaConjunto(matriz, mask_teste)

        if diff_teste > diff_atual:
            diff_atual = diff_teste
            melhor_conjunto = mask_teste

        # Gera o próximo conjunto de bits com a mesma quantidade de bits 1, utilizando o algoritmo de Gosper:

        # Passo 1: Isola o bit mais à esquerda que estiver ligado (1). Ex.: 0100110 -> 0000010
        c = mask_teste & -mask_teste

        # Passo 2: Soma o valor isolado ao conjunto atual. Ex.: 0100110 + 0000010 -> 0101000
        r = mask_teste + c

        # Passo 3: reorganiza os bits à direita do bit isolado, mantendo a quantidade de bits 1. Ex.: 0101000 -> 0011000
        mask_teste = (((r ^ mask_teste) >> 2) // c) | r
    
    indices, i = [], 1
    while melhor_conjunto:
        if melhor_conjunto & 1:
            indices.append(i)
        i += 1
        melhor_conjunto >>= 1

    return indices

def MelhoresVertices(matriz, necessidades, capmax_caminhao):
    """
        Retorna quais são os 2 pontos, que, junto com o galpão, formam os melhores vértices do triângulo inicial pra Inserção Barata.
        Para que eles sejam bons, a distância entre os 3 tem que ser a maior possível
    """

    qtd_estacoes = matriz.shape[0] - 1
    melhores_vertices = None
    maior_distancia = 0

    for i in range(qtd_estacoes):
        for j in range(i+1, qtd_estacoes):
            # Verifica se as necessidades dos dois vértices são compatíveis com a capacidade do caminhão
            if abs(necessidades[i] + necessidades[j]) > capmax_caminhao:
                continue

            # direcao 1: galpao -> i -> j -> galpao
            distancia1 = matriz[0][i+1] + matriz[i+1][j+1] + matriz[j+1][0]

            # direcao 2: galpao -> j -> i -> galpao
            distancia2 = matriz[0][j+1] + matriz[j+1][i+1] + matriz[i+1][0]

            if distancia1 > distancia2 and distancia1 > maior_distancia:
                maior_distancia = distancia1
                melhores_vertices = (i+1, j+1)
            elif distancia2 > maior_distancia:
                maior_distancia = distancia2
                melhores_vertices = (j+1, i+1)

    return melhores_vertices[0], melhores_vertices[1]



# def BuscaGulosa(matriz, capmax_caminhao, necessidades, caminhoes):
#     """
#         Implementa o algoritmo de vizinho mais próximo para cada conjunto
#         Retorna a lista de rotas (listas de índices) e o custo total
#     """

#     rotas = []
#     necessidades_rotas = []
#     custo_total = 0
#     restam_visitar = [estacao for estacao in range(1, matriz.shape[0])]

#     for _ in range(caminhoes):
#         rotas.append([0])
#         necessidades_rotas.append(0)

    
#     # Cada rota guarda sua fila de prioridade de inserções possíveis
#     # Caso haja conflito de interesses, a inserção mais barata global é escolhida

#     while restam_visitar:
#         fila_prioridade = {}

#         # Monta a fila de prioridades para cada rota
#         for i, rota in enumerate(rotas):
#             fila_prioridade[i] = {}

#             # Para a estação ser possível de visitar, as somas das necessidades tem que pertencer ao range [-capmax_caminhao, capmax_caminhao]
#             for estacao in restam_visitar:
#                 if abs(necessidades_rotas[i] + necessidades[estacao]) <= capmax_caminhao:
#                     fila_prioridade[i][estacao] = matriz[rota[-1]][estacao]

#             # Ordena a fila de prioridades pelo custo de inserção (menor primeiro)
#             fila_prioridade[i] = dict(sorted(fila_prioridade[i].items(), key=lambda x: x[1]))

#         # Filtra as rotas que ainda possuem estações possíveis de visitar
#         rotas_a_atualizar = [i for i in range(len(rotas)) if fila_prioridade[i]]

#         if not rotas_a_atualizar:   # Se nenhuma rota consegue inserir mais
#             break

#         while rotas_a_atualizar and restam_visitar:

#             # Definimos a melhor rota em dois critérios: 
#                 # 1. menor custo de inserção
#                 # 2. mais perto de carga 0 (menor valor absoluto da soma das necessidades), para evitar que fique sem opções de inserção
#             melhor_rota = min(
#                 rotas_a_atualizar,
#                 key=lambda x: (
#                     next(iter(fila_prioridade[x].values())),
#                     abs(necessidades_rotas[x] + next(iter(fila_prioridade[x].keys())))  # mais perto de 0
#                 )
#             )

#             # Seleciona a estação de menor custo
#             estacao_escolhida = next(iter(fila_prioridade[melhor_rota]))
#             custo_escolhido = fila_prioridade[melhor_rota][estacao_escolhida]

#             rotas[melhor_rota].append(estacao_escolhida)
#             necessidades_rotas[melhor_rota] += necessidades[estacao_escolhida]

#             custo_total += custo_escolhido

#             restam_visitar.remove(estacao_escolhida)

#             # Para a rota que acabou de inserir, é necessário reajustar sua fila de prioridades, considerando a nova estação final
#             fila_prioridade[melhor_rota] = {
#                 estacao: matriz[estacao_escolhida][estacao]
#                 for estacao in restam_visitar
#                 if abs(necessidades_rotas[melhor_rota] + necessidades[estacao]) <= capmax_caminhao
#             }
#             fila_prioridade[melhor_rota] = dict(sorted(fila_prioridade[melhor_rota].items(), key=lambda x: x[1]))

#             # Remove estação escolhida de todas as filas
#             for i in rotas_a_atualizar:
#                 fila_prioridade[i].pop(estacao_escolhida, None)

#             rotas_a_atualizar = [i for i in rotas_a_atualizar if fila_prioridade[i]]

#     # Finalmente, cada rota deve retornar ao galpão
#     for i, rota in enumerate(rotas):
#         custo_total += matriz[rota[-1]][0]
#         rota.append(0)

#     return rotas, custo_total

import heapq

def BuscaGulosa2(matriz, capmax_caminhao, necessidades, caminhoes):
    """
        Implementa o algoritmo de vizinho mais próximo para cada conjunto
        Retorna a lista de rotas (listas de índices) e o custo total
    """

    rotas = []
    necessidades_rotas = []
    custo_total = 0
    restam_visitar = [estacao for estacao in range(1, matriz.shape[0])]

    for _ in range(caminhoes):
        rotas.append([0])
        necessidades_rotas.append(0)

    
    # Cada rota guarda sua fila de prioridade de inserções possíveis
    # Caso haja conflito de interesses, a inserção mais barata global é escolhida

    fila_prioridade = {}

    # Monta a fila de prioridades para cada rota
    fila = {}

    # Para a estação ser possível de visitar, as somas das necessidades tem que pertencer ao range [-capmax_caminhao, capmax_caminhao]
    for estacao in restam_visitar:
        fila[estacao] = matriz[0][estacao]

    # Ordena a fila de prioridades pelo custo de inserção (menor primeiro)
    fila = dict(sorted(fila.items(), key=lambda x: x[1]))

    fila_prioridade = {i: fila.copy() for i in range(len(rotas))}
    rotas_a_atualizar = [i for i in range(len(rotas))]

    while restam_visitar:

        # Definimos a melhor rota em dois critérios: 
            # 1. menor custo de inserção
            # 2. mais perto de carga 0 (menor valor absoluto da soma das necessidades), para evitar que fique sem opções de inserção
        melhor_rota = min(
            rotas_a_atualizar,
            key=lambda x: (
                next(iter(fila_prioridade[x].values())),
                abs(necessidades_rotas[x] + next(iter(fila_prioridade[x].keys())))  # mais perto de 0
            )
        )

        # Seleciona a estação de menor custo
        estacao_escolhida = next(iter(fila_prioridade[melhor_rota]))
        custo_escolhido = fila_prioridade[melhor_rota][estacao_escolhida]

        rotas[melhor_rota].append(estacao_escolhida)
        necessidades_rotas[melhor_rota] += necessidades[estacao_escolhida]

        custo_total += custo_escolhido

        restam_visitar.remove(estacao_escolhida)

        # Para a rota que acabou de inserir, é necessário reajustar sua fila de prioridades, considerando a nova estação final
        fila_prioridade[melhor_rota] = {
            estacao: matriz[estacao_escolhida][estacao]
            for estacao in restam_visitar
            if abs(necessidades_rotas[melhor_rota] + necessidades[estacao]) <= capmax_caminhao
        }
        fila_prioridade[melhor_rota] = dict(sorted(fila_prioridade[melhor_rota].items(), key=lambda x: x[1]))

        # Remove estação escolhida de todas as filas
        for i in rotas_a_atualizar:
            fila_prioridade[i].pop(estacao_escolhida, None)

        rotas_a_atualizar = [i for i in rotas_a_atualizar if fila_prioridade[i]]

    # Finalmente, cada rota deve retornar ao galpão
    for i, rota in enumerate(rotas):
        custo_total += matriz[rota[-1]][0]
        rota.append(0)

    return rotas, custo_total


def VerificaSolucao(matriz: list[list[int]], necessidades: list[int], cap_max: int, rotas: list[list[int]], show_warnings=False):
    """
        Verifica se as rotas passadas são possíveis
            - Nenhuma estação visitada mais de uma vez
            - Nenhuma estação deixada sem visita
            - Capacidade máxima respeitada
            - Retorna o custo total das rotas
    """

    visitados = np.zeros(matriz.shape[0] - 1, dtype = int)
    custo_total = 0

    for rota in rotas:
        soma_carga = 0

        if rota != [0, 0]:
            for i in range(1, len(rota)):
                soma_carga += necessidades[rota[i]]

                if abs(soma_carga) > cap_max:
                    if show_warnings:
                        print("Capacidade máxima excedida na rota:", rota)
                    return False
                
                if rota[i] != 0:
                    if visitados[rota[i] - 1]:
                        if show_warnings:
                            print("Estação visitada mais de uma vez:", rota[i], "na rota:", rota)
                        return False
                    else:
                        visitados[rota[i] - 1] =  1

                custo_total += matriz[rota[i-1]][rota[i]]

    if not visitados.all():
        if show_warnings:
            print("Nem todas as estações foram visitadas. Estações não visitadas:", np.where(visitados == 0)[0] + 1)
        return False

    
    return custo_total


def GerarExemplo(qtd_caminhoes, qtd_estacoes, capmax_caminhao, max_distancia=100):
    """
        Gera um exemplo aleatório e o salva em um arquivo "exemploX.txt", onde X é o próximo número disponível
    """
    
    max_necessidade = capmax_caminhao // 2
    necessidades = [0] + [np.random.randint(-max_necessidade, max_necessidade + 1) for _ in range(qtd_estacoes)]
    matriz = MatrizAleatoria(qtd_estacoes + 1, qtd_estacoes + 1, max_distancia)

    # Encontra o próximo número disponível para o nome do arquivo
    numero_arquivo = 1
    while True:
        nome_arquivo = f"exemplo{numero_arquivo}.txt"
        try:
            with open(nome_arquivo, 'r'):
                numero_arquivo += 1
        except FileNotFoundError:
            break

    with open(nome_arquivo, 'w') as f:
        f.write(f"{qtd_estacoes}\n")
        f.write(f"{qtd_caminhoes}\n")
        f.write(f"{capmax_caminhao}\n\n")
        f.write(" ".join(map(str, necessidades)) + "\n\n")
        for linha in matriz:
            f.write(" ".join(map(str, linha)) + "\n")

    print(f"Arquivo '{nome_arquivo}' gerado com sucesso.")