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
    
    // percorrer as três instancias
    for(int i = 2; i <= 3; i++){
        stringstream ss("");
        ss << "exemplos/instancia" << i << ".txt"; 
        string fileName = ss.str();

        p = Problema(fileName, true);

        for(int teste = 1; teste <= 10; teste++){
            ss.str("");
            ss << "solucoes/instancia"<< i << "/VMP_" << teste << ".txt";
            string testeVMP = ss.str();

            ss.str("");
            ss << "solucoes/instancia"<< i << "/IMB_" << teste << ".txt";
            string testeIMB = ss.str();
            
            ss.str("");
            ss << "solucoes/instancia"<< i << "/VND_" << teste << ".txt";
            string testeVND = ss.str();
            
            ss.str("");
            ss << "solucoes/instancia"<< i << "/ILS_" << teste << ".txt";
            string testeILS = ss.str();
            
            Solucao s = IMB();
            s.custo_total = CustoTotal(s.rotas);
            s.SalvarSolucao(testeIMB);
            cout << "Solucao IMB_" << i << ", iter [" << teste << "] salva!" << endl;
            
            s = VizinhoMaisProximo();
            s.custo_total = CustoTotal(s.rotas);
            s.SalvarSolucao(testeVMP);
            cout << "Solucao VMP_" << i << ", iter [" << teste << "] salva!" << endl;
            
            VNDIntraInter(s.rotas);
            s.custo_total = CustoTotal(s.rotas);
            s.SalvarSolucao(testeVND);
            cout << "Solucao VND_" << i << ", iter [" << teste << "] salva!" << endl;
            
            // ILS(s.rotas);
            // s.custo_total = CustoTotal(s.rotas);
            // s.SalvarSolucao(testeILS);
            // cout << "Solucao ILS_" << i << ", iter [" << teste << "] salva!" << endl;
        }
    }    
    return 0;
}