from utils import MatrizAleatoria, ImprimirGrafo, CalculaMelhorConjunto, BuscaGulosa, LerDados, GerarExemplo, BuscaGulosa2, VerificaSolucao
import time


# matriz = MatrizAleatoria(m, n)
# caminhoes = 2
# capmax_caminhao = 10
# necessidades = [0, 3, -2, 4, -3, 5, -4, 2, -1, 6] 

estacoes, caminhoes, capmax_caminhao, necessidades, matriz = LerDados("exemplo3.txt")


print("Matriz de adjacência gerada:\n", matriz)
ImprimirGrafo(matriz)

# conjuntos = CalculaMelhorConjunto(matriz, caminhoes)
# ImprimirGrafo(matriz, conjuntos)

tempo_inicial = time.time()
rotas, custo_total = BuscaGulosa(matriz, capmax_caminhao, necessidades, caminhoes=caminhoes)
tempo_final = time.time()
print("Rotas encontradas:", rotas)
print("Custo total:", custo_total)
print("Tempo de execução (s):", tempo_final - tempo_inicial)

tempo_inicial = time.time()
rotas, custo_total = BuscaGulosa2(matriz, capmax_caminhao, necessidades, caminhoes)
tempo_final = time.time()

for i, rota in enumerate(rotas):
    if rota is [0, 0]:
        del rotas[i]

print("Rotas encontradas (2):", rotas)
print("Custo total (2):", VerificaSolucao(matriz, necessidades, capmax_caminhao, rotas))
print("Tempo de execução (s) (2):", tempo_final - tempo_inicial)
