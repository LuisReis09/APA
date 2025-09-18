#include <bits/stdc++.h>
#include "structures.hpp"
#include "utils.hpp"
#include "metodos.hpp"
#include "httplib.h"
#include <chrono>

using namespace std;

Problema p = Problema();
static double gap_antigo_imb = -1, gap_antigo_vmp = -1;
static Solucao melhor_vizinho_mais_proximo = Solucao(), melhor_insercao_mais_barata = Solucao();

string get_form_field(const httplib::Request& req, const std::string& name) {
    auto it = req.files.find(name);
    if (it == req.files.end()) {
        throw std::runtime_error("Campo '" + name + "' não encontrado");
    }
    return it->second.content;
}

string MatrizToString(const vector<vector<int>>& matriz) {
    string result = "[";
    for (size_t i = 0; i < matriz.size(); i++) {
        result += "[";
        for (size_t j = 0; j < matriz[i].size(); j++) {
            result += to_string(matriz[i][j]);
            if (j < matriz[i].size() - 1) {
                result += ", ";
            }
        }
        result += "]";
        if (i < matriz.size() - 1) {
            result += ", ";
        }
    }
    result += "]";
    return result;
}

int main()
{
    httplib::Server server;
    gap_antigo_imb = gap_antigo_vmp = -1;

    /**
     * @brief
     * Recebe um arquivo de entrada do usuário.
     * O programa irá avaliar se a entrada do usuário está no formato esperado.
     * - Se tiver, guarda a instância do problema e retorna sucesso.
     * - Se não estiver, notifica a razão.
     *
     * @returns:
       - success: booleano indicando se a entrada foi válida
       - message: indicando a mensagem de sucesso ou a razão do erro.
    */
    server.Options("/carregarArquivo", [](const httplib::Request &, httplib::Response &res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 200; // precisa responder OK
    });

    server.Post("/carregarArquivo", [](const httplib::Request &req, httplib::Response &res){
        res.set_header("Access-Control-Allow-Origin", "*");

        try{
            p.init(get_form_field(req, "input"));
            cout << "Arquivo recebido e instanciado com sucesso!" << endl;

            res.status = 200;
            string json_response = R"({"success": true, "message": "Arquivo recebido e instanciado com sucesso!"})";
            res.set_content(json_response, "application/json");
        }catch (exception& e){
            cout << "Error: " << e.what() << endl;
            res.status = 400;
            string json_response = R"({"success": false, "message": ")" + string(e.what()) + R"("})";
            res.set_content(json_response, "application/json");
        } 
    });

    /**
     * @brief
     * Recebe do front o valor ótimo (ou null), o máximo de iterações e o máximo de iterações sem melhora
     * O back deve guardar essas informações, para utilizá-las nas funções de processamento.
     */
    server.Options("/configuracoes", [](const httplib::Request &, httplib::Response &res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 200; // precisa responder OK
    });
    server.Post("/configuracoes", [](const httplib::Request &req, httplib::Response &res){
        res.set_header("Access-Control-Allow-Origin", "*");

        try{
            cout << req.body;

            string valor_otimo_str = get_form_field(req, "valor_otimo");
            if(valor_otimo_str != "null")
                p.valor_otimo = stoi(valor_otimo_str);

            p.max_iteracoes = stoi(get_form_field(req, "max_iteracoes"));
            p.max_sem_melhora = stoi(get_form_field(req, "max_sem_melhora"));

            cout << p.valor_otimo << " " << p.max_iteracoes << " " << p.max_sem_melhora << endl;

            res.status = 200;
            string json_response = R"({"success": true, "message": "Configurações recebidas com sucesso!"})";
            res.set_content(json_response, "application/json");

        }catch (exception& e){
            cout << "Error: " << e.what() << endl;
            res.status = 400;
            string json_response = "{\"success\": false, \"message\": \"" + string(e.what()) + "\"}";
            res.set_content(json_response, "application/json");
        } 
    });

    /*
        * @brief
        * Processa os algoritmos gulosos (VMP e IMB), retornando o custo total, o tempo de execução, o gap e as rotas
        * O gap é calculado em relação ao valor ótimo, se fornecido, ou em relação à última solução encontrada
        * Se for a primeira solução, o gap é 0
        * O tempo de execução é em milissegundos
        * As rotas são retornadas em formato JSON
        * Exemplo de resposta:
        {
            "custo_vizinho_mais_proximo": 123,
            "tempo_vizinho_mais_proximo": 456,
            "gap_vizinho_mais_proximo": 7.89,
            "custo_insercao_mais_barata": 234,
            "tempo_insercao_mais_barata": 567,
            "gap_insercao_mais_barata": 8.90,
            "vizinho_mais_proximo": [[0, 1, 2, 0], [0, 3, 4, 0]],
            "insercao_mais_barata": [[0, 2, 1, 0], [0, 4, 3, 0]]
        }
    */
    server.Options("/processarGulosos", [](const httplib::Request &, httplib::Response &res){
        res.set_header("Access-Control-Allow-Origin", "*"); 
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 200; // precisa responder OK
    });
    server.Get("/processarGulosos", [](const httplib::Request &, httplib::Response &res){ 
        res.set_header("Access-Control-Allow-Origin", "*"); 

        cout << "Processando soluções gulosas..." << endl;

        auto start = chrono::high_resolution_clock::now();
        Solucao sol_vizinho_mais_proximo = VizinhoMaisProximo();
        auto end = chrono::high_resolution_clock::now();
        long long duration_vizinho_mais_proximo = chrono::duration_cast<chrono::milliseconds>(end - start).count();

        cout << "Tempo Vizinho Mais Próximo: " << duration_vizinho_mais_proximo << " ms" << endl;

        start = chrono::high_resolution_clock::now();
        Solucao sol_insercao_mais_barata = IMB();
        end = chrono::high_resolution_clock::now();
        long long duration_insercao_mais_barata = chrono::duration_cast<chrono::milliseconds>(end - start).count();

        cout << MatrizToString(sol_insercao_mais_barata.rotas) << endl;

        cout << "Tempo Inserção Mais Barata: " << duration_insercao_mais_barata << " ms" << endl;

        sol_vizinho_mais_proximo.custo_total = CustoTotal(sol_vizinho_mais_proximo.rotas);
        sol_insercao_mais_barata.custo_total = CustoTotal(sol_insercao_mais_barata.rotas);

        if(sol_vizinho_mais_proximo.custo_total < melhor_vizinho_mais_proximo.custo_total || melhor_vizinho_mais_proximo.custo_total == 0)
            melhor_vizinho_mais_proximo = sol_vizinho_mais_proximo;

        if(sol_insercao_mais_barata.custo_total < melhor_insercao_mais_barata.custo_total || melhor_insercao_mais_barata.custo_total == 0)
            melhor_insercao_mais_barata = sol_insercao_mais_barata;

        double gap_vmp , gap_imb = -1;
        if(p.valor_otimo != -1){
            gap_vmp = ((double)(sol_vizinho_mais_proximo.custo_total - p.valor_otimo) / p.valor_otimo) * 100.0;
            gap_imb = ((double)(sol_insercao_mais_barata.custo_total - p.valor_otimo) / p.valor_otimo) * 100.0;
        }else{
            // Retorna o quanto melhorou em relação à última vez
            if(gap_antigo_vmp != -1){
                gap_vmp = ((double)(gap_antigo_vmp - sol_vizinho_mais_proximo.custo_total) / gap_antigo_vmp) * 100.0;
                gap_imb = ((double)(gap_antigo_imb - sol_insercao_mais_barata.custo_total) / gap_antigo_imb) * 100.0;

                gap_antigo_vmp = sol_vizinho_mais_proximo.custo_total;
                gap_antigo_imb = sol_insercao_mais_barata.custo_total;
            }else{
                gap_vmp = gap_imb = 0;
                gap_antigo_vmp = sol_vizinho_mais_proximo.custo_total;
                gap_antigo_imb = sol_insercao_mais_barata.custo_total;
            }
        }

        string json_response = "{ \"vizinho_mais_proximo\": " + MatrizToString(sol_vizinho_mais_proximo.rotas) +
                               ", \"custo_vizinho_mais_proximo\": " + to_string(sol_vizinho_mais_proximo.custo_total) +
                               ", \"tempo_vizinho_mais_proximo\": " + to_string(duration_vizinho_mais_proximo) +
                               ", \"gap_vizinho_mais_proximo\": " + to_string(gap_vmp) +

                               ", \"insercao_mais_barata\": " + MatrizToString(sol_insercao_mais_barata.rotas) +
                               ", \"custo_insercao_mais_barata\": " + to_string(sol_insercao_mais_barata.custo_total) +
                               ", \"tempo_insercao_mais_barata\": " + to_string(duration_insercao_mais_barata) +
                               ", \"gap_insercao_mais_barata\": " + to_string(gap_imb) +
                               " }";
        res.set_content(json_response, "application/json");
    });

    server.Options("/melhorarGulosos", [](const httplib::Request &, httplib::Response &res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 200;
    });
    server.Get("/melhorarGulosos", [](const httplib::Request &, httplib::Response &res){ 
        res.set_header("Access-Control-Allow-Origin", "*"); 

        cout << "Melhorando soluções gulosas..." << endl;

        try{

            auto start = chrono::high_resolution_clock::now();
            MelhorarSolucao(melhor_vizinho_mais_proximo.rotas);
            auto end = chrono::high_resolution_clock::now();
    
            long long duration_vizinho_mais_proximo = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    
            start = chrono::high_resolution_clock::now();
            MelhorarSolucao(melhor_insercao_mais_barata.rotas);
            end = chrono::high_resolution_clock::now();
    
            long long duration_insercao_mais_barata = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    
            melhor_vizinho_mais_proximo.custo_total = CustoTotal(melhor_vizinho_mais_proximo.rotas);
            melhor_insercao_mais_barata.custo_total = CustoTotal(melhor_vizinho_mais_proximo.rotas);
    
            double gap_vmp , gap_imb = -1;
            if(p.valor_otimo != -1){
                gap_vmp = ((double)(melhor_vizinho_mais_proximo.custo_total - p.valor_otimo) / p.valor_otimo) * 100.0;
                gap_imb = ((double)(melhor_insercao_mais_barata.custo_total - p.valor_otimo) / p.valor_otimo) * 100.0;
            }else{
                // Retorna o quanto melhorou em relação à última vez
                if(gap_antigo_vmp != -1){
                    gap_vmp = ((double)(gap_antigo_vmp - melhor_vizinho_mais_proximo.custo_total) / gap_antigo_vmp) * 100.0;
                    gap_imb = ((double)(gap_antigo_imb - melhor_insercao_mais_barata.custo_total) / gap_antigo_imb) * 100.0;
    
                    gap_antigo_vmp = melhor_vizinho_mais_proximo.custo_total;
                    gap_antigo_imb = melhor_insercao_mais_barata.custo_total;
                }else{
                    gap_vmp = gap_imb = 0;
                    gap_antigo_vmp = melhor_vizinho_mais_proximo.custo_total;
                    gap_antigo_imb = melhor_insercao_mais_barata.custo_total;
                }
            }
    
            string json_response = "{ \"vizinho_mais_proximo\": " + MatrizToString(melhor_vizinho_mais_proximo.rotas) +
                                   ", \"custo_vizinho_mais_proximo\": " + to_string(melhor_vizinho_mais_proximo.custo_total) +
                                   ", \"tempo_vizinho_mais_proximo\": " + to_string(duration_vizinho_mais_proximo) +
                                   ", \"gap_vizinho_mais_proximo\": " + to_string(gap_vmp) +
                                   ", \"insercao_mais_barata\": " + MatrizToString(melhor_insercao_mais_barata.rotas) +
                                   ", \"custo_insercao_mais_barata\": " + to_string(melhor_insercao_mais_barata.custo_total) +
                                   ", \"tempo_insercao_mais_barata\": " + to_string(duration_insercao_mais_barata) +
                                   ", \"gap_insercao_mais_barata\": " + to_string(gap_imb) +
                                   " }";
            res.set_content(json_response, "application/json");
        }catch (exception& e){
            cout << "Error: " << e.what() << endl;
            res.status = 400;
            string json_response = "{\"success\": false, \"message\": \"" + string(e.what()) + "\"}";
            res.set_content(json_response, "application/json");
        }

    });

    server.Options("/aplicarVND", [](const httplib::Request &, httplib::Response &res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 200; // precisa responder OK
    });
    server.Get("/aplicarVND", [](const httplib::Request &, httplib::Response &res){ 
        res.set_header("Access-Control-Allow-Origin", "*"); 

        cout << "Aplicando VND nas soluções gulosas..." << endl;
        try{
            auto start = chrono::high_resolution_clock::now();
            VND(melhor_vizinho_mais_proximo.rotas);
            auto end = chrono::high_resolution_clock::now();
            
            long long duration_vnd_vmp = chrono::duration_cast<chrono::milliseconds>(end - start).count();
            
            cout << "Aplicou VND 1" << endl;
            
            start = chrono::high_resolution_clock::now();
            VND(melhor_insercao_mais_barata.rotas);
            end = chrono::high_resolution_clock::now();
            
            cout << "Aplicou VND 2" << endl;
            long long duration_vnd_imb = chrono::duration_cast<chrono::milliseconds>(end - start).count();

            melhor_insercao_mais_barata.custo_total = CustoTotal(melhor_insercao_mais_barata.rotas);
            melhor_vizinho_mais_proximo.custo_total = CustoTotal(melhor_vizinho_mais_proximo.rotas);

            double gap_vmp , gap_imb = -1;
            if(p.valor_otimo != -1){
                gap_vmp = ((double)(melhor_vizinho_mais_proximo.custo_total - p.valor_otimo) / p.valor_otimo) * 100.0;
                gap_imb = ((double)(melhor_insercao_mais_barata.custo_total - p.valor_otimo) / p.valor_otimo) * 100.0;
            }else{
                // Retorna o quanto melhorou em relação à última vez
                if(gap_antigo_vmp != -1){
                    gap_vmp = ((double)(gap_antigo_vmp - melhor_vizinho_mais_proximo.custo_total) / gap_antigo_vmp) * 100.0;
                    gap_imb = ((double)(gap_antigo_imb - melhor_insercao_mais_barata.custo_total) / gap_antigo_imb) * 100.0;

                    gap_antigo_vmp = melhor_vizinho_mais_proximo.custo_total;
                    gap_antigo_imb = melhor_insercao_mais_barata.custo_total;
                }else{
                    gap_vmp = gap_imb = 0;
                    gap_antigo_vmp = melhor_vizinho_mais_proximo.custo_total;
                    gap_antigo_imb = melhor_insercao_mais_barata.custo_total;
                }
            }

            string json_response = "{ \"insercao_mais_barata\": " + MatrizToString(melhor_insercao_mais_barata.rotas) +
                                   ", \"custo_insercao_mais_barata\": " + to_string(melhor_insercao_mais_barata.custo_total) +
                                   ", \"tempo_vnd_insercao_mais_barata\": " + to_string(duration_vnd_imb) +
                                   ", \"gap_vnd_insercao_mais_barata\": " + to_string(gap_imb) +
                                   ", \"vizinho_mais_proximo\": " + MatrizToString(melhor_vizinho_mais_proximo.rotas) +
                                   ", \"custo_vizinho_mais_proximo\": " + to_string(melhor_vizinho_mais_proximo.custo_total) +
                                   ", \"tempo_vnd_vizinho_mais_proximo\": " + to_string(duration_vnd_vmp) +
                                   ", \"gap_vnd_vizinho_mais_proximo\": " + to_string(gap_vmp) +
                                   " }";
            cout << json_response << endl;
            res.set_content(json_response, "application/json");
        }catch( exception& e){
            cout << "Error: " << e.what() << endl;
            res.status = 400;
            string json_response = "{\"success\": false, \"message\": \"" + string(e.what()) + "\"}";
            res.set_content(json_response, "application/json");
        }
    });

    
    server.Options("/aplicarILS", [](const httplib::Request &, httplib::Response &res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 200; // precisa responder OK
    });
    server.Get("/aplicarILS", [](const httplib::Request &, httplib::Response &res){ 
        res.set_header("Access-Control-Allow-Origin", "*"); 

        cout << "Aplicando ILS nas soluções gulosas..." << endl;

        try{
            auto start = chrono::high_resolution_clock::now();
            ILS(melhor_vizinho_mais_proximo.rotas);
            auto end = chrono::high_resolution_clock::now();
            
            long long duration_ils_vmp = chrono::duration_cast<chrono::milliseconds>(end - start).count();
            
            cout << "Aplicou ILS 1" << endl;
            
            start = chrono::high_resolution_clock::now();
            ILS(melhor_insercao_mais_barata.rotas);
            end = chrono::high_resolution_clock::now();
            
            cout << "Aplicou ILS 2" << endl;
            long long duration_ils_imb = chrono::duration_cast<chrono::milliseconds>(end - start).count();


            melhor_insercao_mais_barata.custo_total = CustoTotal(melhor_insercao_mais_barata.rotas);
            melhor_vizinho_mais_proximo.custo_total = CustoTotal(melhor_vizinho_mais_proximo.rotas);

            double gap_vmp , gap_ils = -1;
            if(p.valor_otimo != -1){
                gap_vmp = ((double)(melhor_vizinho_mais_proximo.custo_total - p.valor_otimo) / p.valor_otimo) * 100.0;
                gap_ils = ((double)(melhor_insercao_mais_barata.custo_total - p.valor_otimo) / p.valor_otimo) * 100.0;
            }else{
                // Retorna o quanto melhorou em relação à última vez
                if(gap_antigo_vmp != -1){
                    gap_vmp = ((double)(gap_antigo_vmp - melhor_vizinho_mais_proximo.custo_total) / gap_antigo_vmp) * 100.0;
                    gap_ils = ((double)(gap_antigo_imb - melhor_insercao_mais_barata.custo_total) / gap_antigo_imb) * 100.0;

                    gap_antigo_vmp = melhor_vizinho_mais_proximo.custo_total;
                    gap_antigo_imb = melhor_insercao_mais_barata.custo_total;
                }else{
                    gap_vmp = gap_ils = 0;
                    gap_antigo_vmp = melhor_vizinho_mais_proximo.custo_total;
                    gap_antigo_imb = melhor_insercao_mais_barata.custo_total;
                }
            }

            string json_response = "{ \"insercao_mais_barata\": " + MatrizToString(melhor_insercao_mais_barata.rotas) +
                                   ", \"custo_insercao_mais_barata\": " + to_string(melhor_insercao_mais_barata.custo_total) +
                                   ", \"tempo_ils_insercao_mais_barata\": " + to_string(duration_ils_imb) +
                                   ", \"gap_ils_insercao_mais_barata\": " + to_string(gap_ils) +
                                   ", \"vizinho_mais_proximo\": " + MatrizToString(melhor_vizinho_mais_proximo.rotas) +
                                   ", \"custo_vizinho_mais_proximo\": " + to_string(melhor_vizinho_mais_proximo.custo_total) +
                                   ", \"tempo_ils_vizinho_mais_proximo\": " + to_string(duration_ils_vmp) +
                                   ", \"gap_ils_vizinho_mais_proximo\": " + to_string(gap_ils) +
                                   " }";
            cout << json_response << endl;
            res.set_content(json_response, "application/json");
        }catch( exception& e){
            cout << "Error: " << e.what() << endl;
            res.status = 400;
            string json_response = "{\"success\": false, \"message\": \"" + string(e.what()) + "\"}";
            res.set_content(json_response, "application/json");
        }
    });

    cout << "Back-end is running on http://localhost:4000" << endl;
    server.listen("localhost", 4000);

    return 0;
}