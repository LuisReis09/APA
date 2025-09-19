#include <bits/stdc++.h>
#include <dirent.h>
#include "structures.hpp"
#include "metodos.hpp"
#include "utils.hpp"
#include <chrono>

using namespace std;

Problema p;

double calcularGap(int custo, int valor_otimo){
    if(valor_otimo <= 0) return -1.0; // n/a
    return ((double)(custo - valor_otimo) / valor_otimo) * 100.0;
}

vector<string> listarArquivos(const string &path){
    vector<string> arquivos;
    DIR *dir = opendir(path.c_str());
    if(!dir) return arquivos;

    struct dirent *entry;
    while((entry = readdir(dir)) != nullptr){
        string nome = entry->d_name;
        if(nome != "." && nome != "..")
            arquivos.push_back(path + "/" + nome);
    }
    closedir(dir);
    return arquivos;
}

int main() {
    srand(time(NULL));

    ofstream csv("./resultados.csv", ios::app);
    if(!csv.is_open()){
        cerr << "Nao foi possivel abrir resultados.csv" << endl;
        return 1;
    }

    // Cabeçalho caso o arquivo esteja vazio

    string path_dir = "../exemplos";
    vector<string> arquivos = listarArquivos(path_dir);

    for(const string &arquivo : arquivos){
        cout << "Processando arquivo: " << arquivo << endl;

        try {
            p = Problema(arquivo, true);
            int valor_otimo = (p.valor_otimo != -1) ? p.valor_otimo : 0;

            // ==== Vizinho Mais Proximo ====
            auto start = chrono::high_resolution_clock::now();
            Solucao s = VizinhoMaisProximo();
            auto end = chrono::high_resolution_clock::now();
            s.custo_total = CustoTotal(s.rotas);
            long long tempo = chrono::duration_cast<chrono::milliseconds>(end - start).count();
            double gap = (valor_otimo > 0) ? calcularGap(s.custo_total, valor_otimo) : -1.0;

            csv << "\"" << arquivo << "\"" << ",\"VizinhoMaisProximo\","
                << s.custo_total << "," << tempo << ","
                << ((valor_otimo>0)?to_string(valor_otimo):"\"null\"") << ","
                << ((gap>=0)?to_string(gap):"\"null\"") << "\n";

            // ==== VND Intra Inter ====
            start = chrono::high_resolution_clock::now();
            VNDIntraInter(s.rotas);
            end = chrono::high_resolution_clock::now();
            s.custo_total = CustoTotal(s.rotas);
            tempo = chrono::duration_cast<chrono::milliseconds>(end - start).count();
            gap = (valor_otimo > 0) ? calcularGap(s.custo_total, valor_otimo) : -1.0;

            csv << "\"" << arquivo << "\"" << ",\"VNDIntraInter\","
                << s.custo_total << "," << tempo << ","
                << ((valor_otimo>0)?to_string(valor_otimo):"\"null\"") << ","
                << ((gap>=0)?to_string(gap):"\"null\"") << "\n";

            // ==== ILS ====
            start = chrono::high_resolution_clock::now();
            ILS(s.rotas, 10000, 200);
            end = chrono::high_resolution_clock::now();
            s.custo_total = CustoTotal(s.rotas);
            tempo = chrono::duration_cast<chrono::milliseconds>(end - start).count();
            gap = (valor_otimo > 0) ? calcularGap(s.custo_total, valor_otimo) : -1.0;

            csv << "\"" << arquivo << "\"" << ",\"ILS\","
                << s.custo_total << "," << tempo << ","
                << ((valor_otimo>0)?to_string(valor_otimo):"\"null\"") << ","
                << ((gap>=0)?to_string(gap):"\"null\"") << "\n";

            cout << "Resultados para " << arquivo << " salvos em resultados.csv" << endl;

        } catch(const exception &e){
            cerr << "Erro ao processar " << arquivo << ": " << e.what() << endl;
        }
    }

    csv.close();
    cout << "Processamento concluído, resultados salvos em resultados.csv" << endl;
    return 0;
}
