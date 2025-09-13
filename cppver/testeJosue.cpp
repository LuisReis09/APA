#include "metodos.hpp"
#include "utils.hpp"
#include "threadpool.hpp"
#include "structures.hpp"
#include <iostream>
using namespace std;

int main()
{
    Problema p = LerDados("../exemplos/exemplo3.txt");
    SetProblema(p);

    Solucao sol = VizinhoMaisProximo();

    // Solução inicial
    printf("SOLUÇÃO INICIAL\n\t>> Custo total: %d\n\t>> Veiculos usados: %d\n\t", sol.custo_total, sol.veiculos_usados);
    // Print das rotas
    for (Rota rota : sol.rotas)
    {
        printf(">> ");
        No *aux = rota.rota_i;
        while (aux->proximo)
        {
            printf(" %d - ", aux->estacao);
            aux = aux->proximo;
        }
        printf("%d\n\t", aux->estacao);
    }

    sol.rotas = MelhorarSolucao(sol.rotas);

    // Solução inicial
    printf("SOLUÇÃO PÓS-MELHORA\n\t>> Custo total: %d\n\t>> Veiculos usados: %d\n\t", sol.custo_total, sol.veiculos_usados);
    // Print das rotas
    for (Rota rota : sol.rotas)
    {
        printf(">> ");
        No *aux = rota.rota_i;
        while (aux->proximo)
        {
            printf(" %d - ", aux->estacao);
            aux = aux->proximo;
        }
        printf("%d\n\t", aux->estacao);
    }
}