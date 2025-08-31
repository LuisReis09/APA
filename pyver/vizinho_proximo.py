from utils import LerDados, VerificaSolucao
import numpy as np

# Para fazer a conversao dos valores q vem de LerDados:
def converter_numpy(valor):
        if isinstance(valor, (np.integer, np.int64, np.int32)):
            return int(valor)
        elif isinstance(valor, (np.floating, np.float64, np.float32)):
            return float(valor)
        elif isinstance(valor, np.ndarray):
            return valor.tolist()
        elif hasattr(valor, 'tolist'):
            return valor.tolist()
        else:
            return valor


def guloso_por_vizinho_proximo(arquivo: str = "exemplo1.txt")-> tuple:
    n_estacoes, m_caminhoes, capacidade_p_caminhao, demandas_estacoes, matriz_custo = LerDados(arquivo)  

    n_estacoes = converter_numpy(n_estacoes)
    m_caminhoes = converter_numpy(m_caminhoes)
    capacidade_p_caminhao = converter_numpy(capacidade_p_caminhao)
    demandas_estacoes = converter_numpy(demandas_estacoes)
    matriz_custo = converter_numpy(matriz_custo)

    rotas = [[0] for _ in range(m_caminhoes)]
    custos = [0 for _ in range(m_caminhoes)]
    cargas_iniciais = [] # para analisar se a rota e esse valor foram bem iniciados
    custo_total = 0
    nao_visitados = [i+1 for i in range(n_estacoes)]
    caminhoes_livres = m_caminhoes
    solucao_bem_sucedida = True
    

    def melhor_vizinho_possivel(origem: int, ocupacao: int):
        # if nao_visitados == []:
        #     return None, None, False

        capacidade = capacidade_p_caminhao - ocupacao

        # considera o primeiro nao visitado como melhor atual
        estacao_mais_barata = 0
        melhor_custo = float('inf')
        # verifica se eh possivel ir pra ele com a capacidade atual do caminhao
        eh_possivel = False

        print(nao_visitados)
        # para cada um dos outros nao visitados (depois do primeiro)
        for i in range(len(nao_visitados)):
            custo_atual = matriz_custo[origem][nao_visitados[i]]
            demanda = demandas_estacoes[nao_visitados[i]]
            # e se for possivel, dada a disponibilidade de vagas atual
            
            if 0 <= demanda <= capacidade:
                # se o custo for menor para esse do que pro anterior menor custo
                if custo_atual < melhor_custo:
                    print(f"{nao_visitados[i]}: custo{demandas_estacoes[nao_visitados[i]]}, capacidade:{capacidade}, ocupacao:{ocupacao}")
                    # atualiza o novo melhor nao visitado
                    estacao_mais_barata = i
                    melhor_custo = custo_atual
                    eh_possivel = True

        demanda_atual = demandas_estacoes[nao_visitados[estacao_mais_barata]]
        estacao = nao_visitados[estacao_mais_barata]
        print(f"{"[ROTA CONTINUA]" if eh_possivel else "[FIM DA ROTA]"} de {origem} para {estacao} (demanda X capacidade: {demanda_atual} X {capacidade}), custo: {melhor_custo}")
        # retorna um proximo menos custoso e se é possivel prosseguir
        return estacao_mais_barata, melhor_custo, eh_possivel
    

    while nao_visitados != [] and caminhoes_livres > 0:
        # pega o indice da melhor opcao pra sair da garagem com capacidade maxima 
        # (ignora o eh_possivel, pois os nós não exedem a capacidade inicial dos caminhoes)
        id_proximo, custo, eh_possivel = melhor_vizinho_possivel(0, 0)
        if not eh_possivel:
            break
        proximo = nao_visitados[id_proximo] # define o nó que será adicionado na rota

        # define a carga com a qual o caminhao atual vai sair da garagem
        demanda_atual = demandas_estacoes[proximo]
        carga_atual = (capacidade_p_caminhao - demanda_atual) // 2
        # adiciona essa nova carga inicial na lista
        cargas_iniciais.append(carga_atual)

        # adiciona esse proximo nó na rota do caminhao atual
        caminhao_atual = m_caminhoes - caminhoes_livres
        rotas[caminhao_atual].append(proximo)
        custos[caminhao_atual] += custo
        carga_atual += demandas_estacoes[proximo]
        
        # uma vez que foi visitado, remove esse nó da lista de não visitados
        nao_visitados.remove(proximo) 

        # continua na rota, buscando o proximo melhor nó 
        while True:
            id_proximo, custo, eh_possivel = melhor_vizinho_possivel(proximo, carga_atual)

            # se nao encontrar esse proximo melhor nó possivel, retorna para 
            # a garagem e deixa que o proximo caminhao (se houver), conclua 
            # a distribuição
            if not eh_possivel:
                # adiciona a garagem à rota do caminhao, indicando seu retorno
                rotas[caminhao_atual].append(0)
                # acrescenta o custo da volta
                custos[caminhao_atual] += matriz_custo[proximo][0]
                # dispensa esse caminhão
                caminhoes_livres -= 1
                # segue para o próximo
                break

            # caso haja rota possível

            # atualiza o valor de proximo
            proximo = nao_visitados[id_proximo]

            # nova carga do caminhao
            carga_atual += demandas_estacoes[proximo]

            # atualiza a rota e os custos
            rotas[caminhao_atual].append(proximo)
            custos[caminhao_atual] += custo

            # remove esse nó visitado da lista
            nao_visitados.remove(proximo)
        
        custo_total += custos[caminhao_atual]

    # se os caminhoes se esgotaram e não foram visitados todos os nós
    if caminhoes_livres == 0 and nao_visitados != []:
        solucao_bem_sucedida = False
        print("[!] Não foi encontrada uma solução adequada ou o algoritmo possui problemas em sua estrutura!")       
    
    # para o restante dos caminhoes que restarem, se restarem
    while caminhoes_livres > 0:
        rotas[m_caminhoes - caminhoes_livres].append(0)
        caminhoes_livres -= 1

    return solucao_bem_sucedida, custo_total, rotas, custos, cargas_iniciais

resultados = guloso_por_vizinho_proximo()
print(resultados)
# VerificaSolucao