#include <iostream>
#include <sstream>
#include "structures.hpp"
#include "metodos.hpp"
#include "utils.hpp"
#include <time.h>
// #include "threadpool.hpp"

using namespace std;

Problema p;

int main(){
    // ThreadPool pool(5);
    srand(time(NULL)); // seed para aleatorizar o tamanho do segmento
    std::ofstream arquivo("resultados.csv", std::ios::app); // abre no modo 
    if(!arquivo.is_open()){
                std::cerr << "Erro ao abrir arquivo!" << std::endl;
                return 1;
            }

    // percorrer as três instancias
    for(int i = 1; i <= 3; i++){
        stringstream ss("");
        ss << "../exemplos/instancia" << i << ".txt"; 
        string fileName = ss.str();

        p = Problema(fileName, true);

        stringstream pathIMB("");
        pathIMB << "../exemplos/instancia" << i << ".txt";
        string pathNameIMB = pathIMB.str();
            std::ifstream arquivo_texto(pathNameIMB);

        int custo_solucao;
        float tempo_exec;
        string linha;

        if(std::getline(arquivo_texto, linha)){
            std::stringstream tt(linha);
            tt >> custo_solucao;
        }
        std::getline(arquivo_texto, linha);
        if(std::getline(arquivo_texto, linha)){
            std::stringstream tt(linha);
            tt >> tempo_exec;
        }
        arquivo_texto.close();


        // Coloca apenas a primeira repetição daquele algoritmo visto que é um algoritmo não-randomico
        arquivo << "\"exemplos/instancia" << i << ".txt\",\"Inserção Mais Barata\"," << custo_solucao << ',' << std::fixed << std::setprecision(2) << "\"NA\"" << ",\"NA\"" << '\n';

        // Inserindo o VMP 
        stringstream pathVMP("");
        pathVMP << "exemplos/instancia" << i << ".txt";
        string pathNameVMP = pathVMP.str();
        std::ifstream arquivo_VMP(pathNameVMP);


        if(std::getline(arquivo_VMP, linha)){
            std::stringstream tt(linha);
            tt >> custo_solucao;
        }
        std::getline(arquivo_VMP, linha);
        if(std::getline(arquivo_VMP, linha)){
            std::stringstream tt(linha);
            tt >> tempo_exec;
        }
        arquivo_VMP.close();

        arquivo << "VMB_" << i << "_1" << ".txt,Vizinho Mais Proximo," << custo_solucao << ',' << std::fixed << std::setprecision(2) << "NA" << "NA" << '\n';

        // Inserindo as 10 execuções do VND
    for (int j = 1 ; j <= 10 ; j++){
        stringstream pathVND("");
        pathVND << "exemplos/instancia" << i << ".txt";
        string pathNameVND = pathVND.str();
            std::ifstream arquivo_VND(pathNameVND);

        if(std::getline(arquivo_VND, linha)){
            std::stringstream tt(linha);
            tt >> custo_solucao;
        }
        std::getline(arquivo_VND, linha);
        if(std::getline(arquivo_VND, linha)){
            std::stringstream tt(linha);
            tt >> tempo_exec;
        }
        arquivo_VND.close();

        // Coloca apenas a primeira repetição daquele algoritmo visto que é um algoritmo não-randomico
        arquivo << "VND_" << i << "_" << j << ".txt,VND," << custo_solucao << ',' << std::fixed << std::setprecision(2) << "NA" << "NA" << '\n';

        }

        // Inserindo as 10 linhas do ILS
        for (int k = 1 ; k <= 10 ; k++){
        stringstream pathILS("");
        pathILS << "exemplos/instancia" << i << ".txt";
        string pathNameILS = pathILS.str();
            std::ifstream arquivo_ILS(pathNameILS);

        if(std::getline(arquivo_ILS, linha)){
            std::stringstream tt(linha);
            tt >> custo_solucao;
        }
        std::getline(arquivo_ILS, linha);
        if(std::getline(arquivo_ILS, linha)){
            std::stringstream tt(linha);
            tt >> tempo_exec;
        }
        arquivo_ILS.close();


        // Coloca apenas a primeira repetição daquele algoritmo visto que é um algoritmo não-randomico
        arquivo << "ILS_" << i << "_" << k << ".txt,VND," << custo_solucao << ',' << std::fixed << std::setprecision(2) << "NA" << "NA" << '\n';
    }

    } 
    
    return 0;
}