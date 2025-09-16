#include <bits/stdc++.h>
#include "httplib.h"
#include "structures.hpp"
#include "utils.hpp"
#include "metodos.hpp"

using namespace std;
Problema p = Problema();

int main()
{
    httplib::Server server;

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
    server.Post("/carregarArquivo", [](const httplib::Request &req, httplib::Response &res)
                {
        res.set_header("Access-Control-Allow-Origin", "*");
        try{

            cout << req.body << endl;
            cout << req.get_param_value("input");

            p.init(req.get_param_value("input"));

            res.status = 200;
            string json_response = R"({"success": true, "message": "Arquivo recebido e instanciado com sucesso!"})";
            res.set_content(json_response, "application/json");
        }catch (exception& e){
            cout << "Error: " << e.what() << endl;
            res.status = 400;
            string json_response = R"({"success": false, "message": ")" + string(e.what()) + R"("})";
            res.set_content(json_response, "application/json");
        } });

    /**
     * @brief
     * Recebe do front o valor ótimo (ou null), o máximo de iterações e o máximo de iterações sem melhora
     * O back deve guardar essas informações, para utilizá-las nas funções de processamento.
     */
    server.Post("/configuracoes", [](const httplib::Request &req, httplib::Response &res){
        res.set_header("Access-Control-Allow-Origin", "*");

        try{
            cout << req.body;
            cout << req.get_param_value("valor_otimo");
            cout << req.get_param_value("max_iteracoes");
            cout << req.get_param_value("max_sem_melhora");

            if(req.get_param_value("valor_otimo") != "null")
                p.valor_otimo = stoi(req.get_param_value("valor_otimo"));

            p.max_iteracoes = stoi(req.get_param_value("max_iteracoes"));
            p.max_sem_melhora = stoi(req.get_param_value("max_sem_melhora"));

            res.status = 200;
            string json_response = R"({"success": true, "message": "Configurações recebidas com sucesso!"})";
            res.set_content(json_response, "application/json");

        }catch (exception& e){
            cout << "Error: " << e.what() << endl;
            res.status = 400;
            string json_response = R"({"error": "bad request!"})";
            res.set_content(json_response, "application/json");
        } 
    });

   
    server.Get("/processarGulosos", [](const httplib::Request &, httplib::Response &res)
               { res.set_header("Access-Control-Allow-Origin", "*"); });

    
    server.Get("/melhorarGulosos", [](const httplib::Request &, httplib::Response &res)
               { res.set_header("Access-Control-Allow-Origin", "*"); });

   
    server.Get("/aplicarVND", [](const httplib::Request &, httplib::Response &res)
               { res.set_header("Access-Control-Allow-Origin", "*"); });

    
    server.Post("/aplicarILS", [](const httplib::Request &, httplib::Response &res)
                { res.set_header("Access-Control-Allow-Origin", "*"); });

    cout << "Back-end is running on http://localhost:4000" << endl;
    server.listen("localhost", 4000);

    return 0;
}