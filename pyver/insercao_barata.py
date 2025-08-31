from structures import Rota
from utils import LerDados

# Importa as variaveis em questão
estacoes, caminhoes, capmax_caminhao, necessidades, matriz = LerDados("exemplo1.txt")

def MelhoresVerticesAdaptado(matriz, exclude = []):
    """
        Retorna quais são os 2 pontos, que, junto com o galpão, formam os melhores vértices do triângulo inicial pra Inserção Barata.
        Para que eles sejam bons, a distância entre os 3 tem que ser a maior possível
    """

    qtd_estacoes = matriz.shape[0] - 1
    melhores_vertices = None
    maior_distancia = 0

    for i in range(qtd_estacoes):
        for j in range(i+1, qtd_estacoes):
            if j or i in exclude:   # Termo excluido / Que já foi explorado, não contar
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

def insercaoMaisBarata():
    """
    PARÂMETROS: Nenhum
    RETORNO: Array de objetos Rota
    """

    # Passo 1: Encontrando vértices do triangulo inicial
    # Procurando os dois maiores valores da matriz

    # Caso especial: Apenas 1 vértice no grafo inteiro
    #  --> Matriz tem aparência [[null, X]
    #                            [Y, null]]
    if estacoes == 1:
        return custo_total + matriz[0][1] + matriz[1][0]

    rotas = []
    estacoes_exploradas = []

    while estacoes_exploradas != estacoes: # Enquanto houver estações a serem exploradas...

        v1, v2 = MelhoresVerticesAdaptado(matriz, estacoes_exploradas)  # Acha os dois vertices mais distantes dado uma lista de estações excluidas

        estacoes_exploradas.append(v1, v2)
        estacoes_exploradas.sort()

        custo_total = matriz[0][v1] + matriz[v2][0] + matriz[v1][v2]
        
        rota_achada = Rota()
        rota_achada.init_insercaoBarata(v1, v2, matriz[0][v1] + matriz[v2][0] + matriz[v1][v2], necessidades[v1] + necessidades[v2])

        # Estações não examinadas ainda:
        nao_visitadas = [station for station in range(1, estacoes + 1) if station != v1 or station != v2]

        # Passo 2: Inserção termo a termo da melhor rota
        for station in nao_visitadas:
            # Checa se a demanda daquela estação está no intervalo correto e permissível de existência
            demanda_iteracao = rota_achada.soma_demandas + necessidades[station]
            if demanda_iteracao < (-1)*capmax_caminhao or demanda_iteracao > capmax_caminhao:   # Excede capacidade, insatisfatível
                continue

            # Checagem entre qual aresta deve ser posta
            duplas = rota_achada.returnAllDoubles()
            custos_insertion = [None for _ in duplas]
            # Terá forma [0, 0, 0, ..., 0]

            # Calculo do custo ganho ou perdido 
            index_dupla = 0
            for dupla in duplas:
                custos_insertion[index_dupla] = matriz[dupla[0]][station] + matriz[station][dupla[1]] - matriz[dupla[0]][dupla[1]]
                index_dupla += 1
            
            # Achando a dupla com menos valor
            index_aresta = custos_insertion[0] 
            for i in range(len(custos_insertion)):
                if custos_insertion[i] < index_aresta:
                    index_aresta = i
            
            # Passo 3: Adicionando à rota
            rota_achada.custo_total += custos_insertion[index_aresta]
            rota_achada.soma_demandas += necessidades[station]
            rota_achada.inserirEmDupla(duplas[index_aresta], station)

            estacoes_exploradas.append(station)

            rotas.append(rota_achada)

    return rotas
            
teste = insercaoMaisBarata()

print(f"===== Inserção barata Josué ver.")
for rota in teste:
    print(f"\t>> Rota: {rota.rota}\n\t>> Demanda acumulada: {rota.custo_total}\n\t>> Custo total: {rota.custo_total}\n\n")