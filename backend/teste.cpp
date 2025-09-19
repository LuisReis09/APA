#include <iostream>
#include <fstream>
#include "structures.hpp"
#include "utils.hpp"
#include "metodos.hpp"

#define MAX 1000000

using namespace std;
Problema p;

int main()
{
    int menor_vmp, menor_imb, menor_vnd, menor_ils;
    int soma_vmp, soma_imb, soma_vnd, soma_ils;
    int media_vmp, media_imb, media_vnd, media_ils;

    for (int inst = 1; inst <= 3; inst)
    {

        string path = "./exemplos/instancia" + to_string(inst) + ".txt";
        p = Problema(path, true);
        soma_vmp = soma_imb = soma_vnd = soma_ils = 0;
        menor_vmp = menor_imb = menor_vnd = menor_ils = MAX;

        for (int i = 1; i <= 15; i++)
        {
            Solucao sol = VizinhoMaisProximo();
            sol.custo_total = CustoTotal(sol.rotas);
            soma_vmp += sol.custo_total;
            if (sol.custo_total < menor_vmp)
            {
                menor_vmp = sol.custo_total;
            }

            cout << "vmp feito" << endl;
            sol.SalvarSolucao("./solucoes/instancia" + to_string(inst) + "/vmp/" + to_string(i) + ".txt");

            Solucao sol_imb = IMB();
            sol_imb.custo_total = CustoTotal(sol_imb.rotas);
            soma_imb += sol_imb.custo_total;
            if (sol_imb.custo_total < menor_imb)
            {
                menor_imb = sol_imb.custo_total;
            }

            cout << "imb feito" << endl;
            sol.SalvarSolucao("./solucoes/instancia" + to_string(inst) + "/imb/" + to_string(i) + ".txt");

            VND2(sol.rotas);
            sol.custo_total = CustoTotal(sol.rotas);
            soma_vnd += sol.custo_total;
            if (sol.custo_total < menor_vnd)
            {
                menor_vnd = sol.custo_total;
            }

            cout << "vnd feito" << endl;
            sol.SalvarSolucao("./solucoes/instancia" + to_string(inst) + "/vnd/" + to_string(i) + ".txt");

            ILS_Backend(sol.rotas, 2000, 200);
            sol.custo_total = CustoTotal(sol.rotas);
            soma_ils += sol.custo_total;
            if (sol.custo_total < menor_ils)
            {
                menor_ils = sol.custo_total;
            }

            cout << "ils feito" << endl;
            sol.SalvarSolucao("./solucoes/instancia" + to_string(inst) + "/ils/" + to_string(i) + ".txt");
        }

        media_vmp = soma_vmp / 15;
        media_imb = soma_imb / 15;
        media_vnd = soma_vnd / 15;
        media_ils = soma_ils / 15;

        cout << "\n=== INSTANCIA " << inst << " ===" << endl;
        cout << "Media Vizinho Mais Proximo: " << media_vmp << endl;
        cout << "Menor Valor Vizinho Mais Proximo: " << menor_vmp << endl;
        cout << "Media Insercao Mais Barata: " << media_imb << endl;
        cout << "Menor Valor Insercao Mais Barata: " << menor_imb << endl;
        cout << "Media VND: " << media_vnd << endl;
        cout << "Menor Valor VND: " << menor_vnd << endl;
        cout << "Media ILS: " << media_ils << endl;
        cout << "Menor Valor ILS: " << menor_ils << endl;
    }

    return 0;
}