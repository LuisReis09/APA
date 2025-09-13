from utils import LerDados, MelhoresVertices, VerificaSolucao,  BuscaGulosa2
import copy
import random

# ---------- Leitura dos dados ----------
estacoes, caminhoes, capmax_caminhao, necessidades, matriz = LerDados("../exemplos/instancia1.txt")
print("DADOS DE ENTRADA")
print("Estações:", estacoes)
print("Caminhões:", caminhoes)
print("Capacidade máxima por caminhão:", capmax_caminhao)
print("Necessidades:", necessidades)
print(len(necessidades))
print()


# ---------- Inicialização ----------
# v1, v2 = MelhoresVertices(matriz, necessidades, capmax_caminhao)

# rotas = [[(0, v1), (v1, v2), (v2, 0)]]
# caminhoes_restantes = caminhoes - 1
# estacoes_restantes = [estacao+1 for estacao in range(estacoes)]
# estacoes_restantes.remove(v1)
# estacoes_restantes.remove(v2)

# Sem conjunto inicial
rotas = []
caminhoes_restantes = caminhoes
estacoes_restantes = [estacao+1 for estacao in range(estacoes)]  # +1 pq 0 é o depósito

# ---------- Funções auxiliares ----------
# calcula = verifica, nos arquivos cpp
def calcula_demanda(rota):
    """Calcula a demanda acumulada da rota, retornando False se ultrapassar a capacidade."""
    demanda_total = 0
    for aresta in rota:
        if aresta[0] != 0:  # Ignora a garagem
            demanda_total += necessidades[aresta[0]]
            if abs(demanda_total) > capmax_caminhao:
                return False
    return True

def calcula_demandas(rotas):
    for rota in rotas:
        demanda_total = 0
        for estacao in rota[1:-1]:  # Ignora depósitos
            demanda_total += necessidades[estacao]
            if abs(demanda_total) > capmax_caminhao:
                return False
            
    return True

def SomaCusto(rotas):
    """Calcula o custo total da rota."""
    custo_total = 0
    for rota in rotas:
        for i in range(1, len(rota)):
            custo_total += matriz[rota[i-1]][rota[i]]
    return custo_total

def verifica_demanda(rota, ind_aresta, estacao):
    """Verifica se a demanda da estação pode ser inserida na rota sem ultrapassar a capacidade."""
    nova_rota = rota[:ind_aresta] + [(rota[ind_aresta][0], estacao), (estacao, rota[ind_aresta][1])] + rota[ind_aresta+1:]
    demanda_total = 0
    for aresta in nova_rota:
        if aresta[0] != 0:  # Ignora a garagem
            demanda_total += necessidades[aresta[0]]
            if abs(demanda_total) > capmax_caminhao:
                return False

    return True

def verifica_rota(rota):
    """ Verifica se a demanda total da rota não ultrapassa a capacidade máxima do caminhão. """
    demanda_total = 0
    for estacao in rota[1:-1]:
        demanda_total += necessidades[estacao]
        if abs(demanda_total) > capmax_caminhao:
            return False

    return True


def calcula_custo_insercao(rota, ind, estacao):
    """Calcula o custo de inserir uma estação em uma posição da rota."""
    origem, destino = rota[ind]
    return matriz[origem][estacao] + matriz[estacao][destino] - matriz[origem][destino]


# ---------- Algoritmo principal ----------
# ---------- Inserção mais barata ---------
def execute(estacoes_restantes=estacoes_restantes, rotas=rotas, caminhoes_restantes=caminhoes_restantes):
    for estacao in estacoes_restantes:
        menor_custo = float("inf")
        melhor_config = None  # (ind_rota, rota_final, custo)

        for ind_rota, rota in enumerate(rotas):
            # Verifica a rota normal e a invertida
            for rota_testada in [rota, [(y, x) for (x, y) in reversed(rota)]]:
                for ind, arestas in enumerate(rota_testada):

                    # SUBSTITUIR POR VERSAO OTIMIZADA
                    custo = calcula_custo_insercao(rota_testada, ind, estacao)

                    if custo < menor_custo:
                        if verifica_demanda(rota_testada, ind, estacao):
                            menor_custo = custo
                            melhor_config = (ind_rota, rota_testada, ind, arestas)

        # Decide onde inserir ou criar nova rota
        if (melhor_config and menor_custo <= (matriz[0][estacao] + matriz[estacao][0])) or caminhoes_restantes == 0:
            ind_rota, rota_final, ind_aresta, arestas = melhor_config
            nova_rota = rota_final[:ind_aresta] + [(arestas[0], estacao), (estacao, arestas[1])] + rota_final[ind_aresta+1:]
            rotas[ind_rota] = nova_rota
        else:
            rotas.append([(0, estacao), (estacao, 0)])
            caminhoes_restantes -= 1

    return rotas

def arestas_para_rotas(rotas_arestas):
    rotas_ret = []
    for rota in rotas_arestas:
        rota_atual = [0]
        last = 0
        rota.sort()
        for _ in range(len(rota) - 1):
            for aresta in rota:
                if aresta[0] == last:
                    rota_atual.append(aresta[1])
                    last = aresta[1]
                    break
        rotas_ret.append(rota_atual + [0])

    return rotas_ret

def melhorar_solucao(rotas):
    """
    Tenta reposicionar melhor as estações nas rotas para reduzir custo total.
    Mantém depósitos nas pontas e aplica inserção mais barata.
    """

    for ind_rota_origem, rota in enumerate(rotas):
        for estacao in rota[1:-1]:  # ignora depósitos nas pontas

            for ind_rota_destino, rota_destino in enumerate(rotas):
                # posições de inserção: ignorando primeiro e último
                for pos in range(1, len(rota_destino) - 1):

                    if estacao == 0:  # nunca mover depósitos
                        continue

                    # cópias para teste
                    nova_rota_origem = rota[:]

                    if ind_rota_origem == ind_rota_destino:
                        nova_rota_origem.remove(estacao)
                        nova_rota_destino = nova_rota_origem
                    else:
                        # mover para outra rota
                        nova_rota_origem.remove(estacao)
                        nova_rota_destino = rota_destino[:]

                    # insere a estação na posição válida
                    nova_rota_destino.insert(pos, estacao)

                    # checa viabilidade
                    if not verifica_rota(nova_rota_destino) or not verifica_rota(nova_rota_origem):
                        continue

                    # calcula custos
                    if ind_rota_origem == ind_rota_destino:
                        custo_atual = SomaCusto([rota])
                        custo_novo = SomaCusto([nova_rota_destino])
                    else:
                        custo_atual = SomaCusto([rota, rota_destino])
                        custo_novo = SomaCusto([nova_rota_origem, nova_rota_destino])

                    # aplica mudança se melhora
                    if custo_novo < custo_atual:
                        rotas[ind_rota_destino] = nova_rota_destino
                        if ind_rota_origem != ind_rota_destino:
                            rotas[ind_rota_origem] = nova_rota_origem
                        return melhorar_solucao(rotas)

    return rotas

# def melhorar_solucao(rotas):
#     """
#         Tenta posicionar melhor as estações nas rotas para reduzir o custo total.
#         Reaplicando o algoritmo de inserção mais barata nas rotas.
#     """

#     for ind_rota_origem, rota in enumerate(rotas):
#         for estacao in rota[1:-1]:  # ignora depósitos (0 no início e fim)

#             for ind_rota, rota_testada in enumerate(rotas):

#                 for pos in range(1, len(rota_testada)):  # posições de inserção

#                     if estacao == 0:
#                         continue


#                     # remove a estação da origem
#                     nova_rota_origem = rota[:]
#                     nova_rota_origem.remove(estacao)

#                     if not verifica_rota(nova_rota_origem):
#                         continue

#                     rota_inserida = nova_rota_origem[:] if rota_testada == rota else rota_testada[:]

#                     # versão inserida
#                     rota_inserida.insert(pos, estacao)

#                     if ind_rota_origem == ind_rota:
#                         custo_atual = SomaCusto([rota])
#                         custo_novo = SomaCusto([rota_inserida])
#                     else:
#                         custo_atual = SomaCusto([rota, rota_testada])
#                         custo_novo = SomaCusto([nova_rota_origem, rota_inserida])

#                     if custo_novo < custo_atual and verifica_rota(rota_inserida):
#                         if ind_rota_origem == ind_rota:
#                             rotas[ind_rota] = rota_inserida
#                         else:
#                             rotas[ind_rota] = rota_inserida
#                             if nova_rota_origem == [0, 0]:
#                                 del rotas[ind_rota_origem]
#                             else:
#                                 rotas[ind_rota_origem] = nova_rota_origem

#                         return melhorar_solucao(rotas)

#     return rotas

def perturbacao_switch(rotas, trocas_a_realizar):
    """
    Troca estações entre rotas ou dentro da mesma rota.
    Altera rotas por referência.
    """
    qtd_rotas = len(rotas)
    if qtd_rotas == 0:
        return rotas

    while trocas_a_realizar > 0:
        if qtd_rotas > 1:
            rota_origem, rota_destino = random.sample(range(qtd_rotas), 2)
        else:
            rota_origem = rota_destino = 0

        if len(rotas[rota_origem]) < 3 or len(rotas[rota_destino]) < 3:
            trocas_a_realizar -= 1
            continue

        pos1 = random.randint(1, len(rotas[rota_origem]) - 2)
        pos2 = random.randint(1, len(rotas[rota_destino]) - 2)

        rotas[rota_origem][pos1], rotas[rota_destino][pos2] = \
            rotas[rota_destino][pos2], rotas[rota_origem][pos1]

        trocas_a_realizar -= 1

    return rotas


def perturbacao_relocate(rotas, trocas_a_realizar, qtd_elementos_trocados, reverso=False):
    """
    Remove um bloco de elementos de uma rota e insere em outra.
    Altera rotas por referência.
    """
    while trocas_a_realizar > 0 and rotas:
        rota_origem = random.randint(0, len(rotas) - 1)
        rota_destino = random.randint(0, len(rotas) - 1)

        if len(rotas[rota_origem]) <= qtd_elementos_trocados + 1:
            trocas_a_realizar -= 1
            continue

        max_index = len(rotas[rota_origem]) - 1 - qtd_elementos_trocados
        indice_retirada = random.randint(1, max_index)

        elementos = rotas[rota_origem][indice_retirada:indice_retirada + qtd_elementos_trocados]
        del rotas[rota_origem][indice_retirada:indice_retirada + qtd_elementos_trocados]

        if reverso:
            elementos.reverse()

        indice_insercao = random.randint(1, len(rotas[rota_destino]) - 1)
        rotas[rota_destino][indice_insercao:indice_insercao] = elementos

        if len(rotas[rota_origem]) <= 2:
            rotas.pop(rota_origem)

        trocas_a_realizar -= 1

    return rotas


def perturbacao_newroute(rotas, qtd_elementos):
    """
    Retira elementos de rotas válidas e cria uma nova rota.
    Altera rotas por referência.
    """
    elementos_retirados = []

    while qtd_elementos > 0:
        rotas_validas = [i for i, r in enumerate(rotas) if len(r) > 2]
        if not rotas_validas:
            break

        rota_origem = random.choice(rotas_validas)
        indice_retirada = random.randint(1, len(rotas[rota_origem]) - 2)

        elementos_retirados.append(rotas[rota_origem].pop(indice_retirada))

        if len(rotas[rota_origem]) <= 2:
            rotas.pop(rota_origem)

        qtd_elementos -= 1

    if elementos_retirados:
        rotas.append([0] + elementos_retirados + [0])

    return rotas

def perturbacao_inversao(rotas):
    """
        Escolhe aleatoriamente uma rota pra ser invertida.
        Altera rotas por referência.
    """

    rota_escolhida = random.randint(0, len(rotas) - 1)
    rotas[rota_escolhida] = rotas[rota_escolhida][::-1]
    return rotas


def pertubacao(rotas, opcao, grau_perturbacao=1):
    """
    Aplica perturbações para escapar de ótimos locais.
    """
    match opcao:
        case 1:
            # Opção 1: switch de até (2 * grau_perturbacao) estações entre rotas diferentes
            return perturbacao_switch(rotas, 3*grau_perturbacao)
        case 2:
            # Opção 2: relocate de blocos de (grau_perturbacao) estações entre rotas diferentes
            return perturbacao_relocate(rotas, grau_perturbacao, grau_perturbacao)
        case 3:
            # Opção 3: relocate de blocos de (grau_perturbacao) estações entre rotas diferentes, invertendo a ordem
            return perturbacao_relocate(rotas, grau_perturbacao, grau_perturbacao, reverso=True)
        case 4:
            # Opção 4: cria uma nova rota com até (2 * grau_perturbacao) estações retiradas de rotas existentes
            return perturbacao_newroute(rotas, 2 * grau_perturbacao)
        case 5:
            # Opção 5: inverte uma rota inteira
            return perturbacao_inversao(rotas)
        case _:
            return rotas

import numpy as np
import pandas as pd
treino = np.zeros((5, 7), dtype=int)  # Matriz para registrar o treino das perturbações

def ILS(rotas, max_iteracoes, max_sem_melhora):
    # Também deve receber a instância do Problema

    iteracoes = 0
    sem_melhora = 0

    """
        Uma boa fórmula de calcular o grau pode se log2(qtd_estacoes) ou algo do tipo.

        Se o grau maximo de perturbação for 5, então:
        Se sem_melhora <= 20% de max_sem_melhora:           grau de perturbação = 1
        Se 20% < sem_melhora <= 40% de max_sem_melhora:     grau de perturbação = 2
        Se 40% < sem_melhora <= 60% de max_sem_melhora:     grau de perturbação = 3
        Se 60% < sem_melhora <= 80% de max_sem_melhora:     grau de perturbação = 4
        Se 80% < sem_melhora <= 100% de max_sem_melhora:    grau de perturbação = 5
    """
    while iteracoes < max_iteracoes and sem_melhora < max_sem_melhora:

        grau_perturbacao = (sem_melhora // (max_sem_melhora // 7)) + 1
        opcao_perturbacao = random.randint(1, 5)  # Escolhe uma das 5 opções de perturbação

        solucao_perturbada = pertubacao(copy.deepcopy(rotas), opcao_perturbacao, grau_perturbacao)
        solucao_perturbada = melhorar_solucao(solucao_perturbada)
        custo_atual = SomaCusto(rotas)
        custo_novo  = SomaCusto(solucao_perturbada)
        if custo_novo < custo_atual and calcula_demandas(solucao_perturbada):
            rotas = solucao_perturbada
            sem_melhora = 0
            print(f"Melhoria geral encontrada: {custo_atual} -> {custo_novo}")
            treino[opcao_perturbacao-1][grau_perturbacao-1] += 1

        else:
            sem_melhora += 1

        iteracoes += 1

    return rotas
    

print("Iniciando busca gulosa...")
rotas_guloso, custo = BuscaGulosa2(matriz, capmax_caminhao, necessidades, caminhoes)
print("Custo inicial:", VerificaSolucao(matriz, necessidades, capmax_caminhao, rotas_guloso))
rotas_guloso = [rota for rota in rotas_guloso if len(rota) > 2]  # Remove rotas vazias
rotas_guloso = melhorar_solucao(rotas_guloso)
print("Rotas melhora-guloso:", rotas_guloso)
print("Custo melhora-guloso:", VerificaSolucao(matriz, necessidades, capmax_caminhao, rotas_guloso))
# rotas_guloso = ILS(rotas_guloso, 5000, 50)
# print("Rotas finais guloso:", rotas_guloso)
# custo_final = VerificaSolucao(matriz, necessidades, capmax_caminhao, rotas_guloso)
# print("Custo final guloso:", custo_final)

rotas_finais = execute()
rotas_finais = arestas_para_rotas(rotas_finais)
print("\nRotas IMB:", rotas_finais)
custo_total = VerificaSolucao(matriz, necessidades, capmax_caminhao, rotas_finais)
print("Custo IMB:", custo_total, "\n")


rotas_finais = [rota for rota in rotas_finais if len(rota) > 2]  # Remove rotas vazias
rotas_finais = melhorar_solucao(rotas_finais)
print("Rotas IMB-melhorado:", rotas_finais)
custo_total = VerificaSolucao(matriz, necessidades, capmax_caminhao, rotas_finais, show_warnings=True)
print("Custo IMB-melhorado:", custo_total)

rotas_finais = ILS(rotas_finais, 5000, 50)
print("Rotas finais IMB:", rotas_finais)
custo_total = VerificaSolucao(matriz, necessidades, capmax_caminhao, rotas_finais)
print("Custo total IMB:", custo_total)

print("\nTreino ILS (opção de perturbação x grau de perturbação):")
df_treino = pd.DataFrame(
    treino,
    index=["Switch", "Relocate", "Relocate Inverso", "Nova Rota", "Inversão"],
    columns=[1, 2, 3, 4, 5, 6, 7]
)
print(df_treino)