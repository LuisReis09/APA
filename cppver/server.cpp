#include <bits/stdc++.h>
#include "httplib.h"

using namespace std;

int main(){
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
    server.Post("/carregarArquivo", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        try{
            // passar req.body para a função de leitura

            res.status = 200;
            string json_response = R"({"success": true, "message": "Arquivo recebido e instanciado com sucesso!"})";
            res.set_content(json_response, "application/json");
        }catch (exception& e){
            // Se faltou parametro, avisa
            cout << "Error: " << e.what() << endl;
            res.status = 400;
            string json_response = R"({"error": "bad request!"})";
            res.set_content(json_response, "application/json");
        }
    });

    /**
     * @brief
     * Recebe do front o valor ótimo (ou null), o máximo de iterações e o máximo de iterações sem melhora
     * O back deve guardar essas informações, para utilizá-las nas funções de processamento.
    */
    server.Post("/configuracoes", [](const httplib::Request& req, httplib::Response& res){
        res.set_header("Access-Control-Allow-Origin", "*");

        try{

        }catch (exception& e){
            cout << "Error: " << e.what() << endl;
            res.status = 400;
            string json_response = R"({"error": "bad request!"})";
            res.set_content(json_response, "application/json");
        }
    });


    /**
     * @brief
     * Essa função é chamada quando o usuário aperta o botão de "processar"
     * A partir disso, o back usará a instância do problema pra rodar os seguintes algoritmos gulosos:
        - Vizinho mais próximo
        - Inserção mais barata
     
     *
     * @returns Exemplo:
     * {
     *      success: false,
     *      message: "falha ao ..."
     * }
     * 
     * {
     *      success: true,
     *      vizinho_mais_proximo: [
     *          {id: 1, path: [[0, 1, 7, 0]], cost: 115.8, load: 42 },
     *          {id: 2, path: [[0, 3, 5, 0]], cost: 115.8, load: 30 },
     *          ...
     *      ],
     *     tempo_vizinho_mais_proximo: 123.45, // em ms
     *      insercao_mais_barata: [
     *          {id: 1, path: [[0, 1, 7, 0]], cost: 115.8, load: 42 },
     *          {id: 2, path: [[0, 3, 5, 0]], cost: 115.8, load: 30 },
     *          ...
     *      ],
     *     tempo_insercao_mais_barata: 123.45, // em ms
     * }
     *
     */
    server.Get("/processarGulosos", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
    });

    /**
     * @brief
     * Essa função é chamada quando o usuário seleciona a opção "melhorar rotas"
     * A função de melhorar rotas funcionará como uma busca local para as saídas dos algoritmos vizinho mais próximo e inserção mais barata.
     * Ela consiste de tentar realocações na mesma ou em outras rotas, que melhore o custo, sem impossibilitar a sequência de cargas.
     * 
     * @returns:
     * {
     *      success: false,
     *      message: "falha ao ..."
     * }
     * 
     * {
     *      success: true,
     *      vizinho_mais_proximo: [
     *          {id: 1, path: [[0, 1, 7, 0]], cost: 115.8, load: 42 },
     *          {id: 2, path: [[0, 3, 5, 0]], cost: 115.8, load: 30 },
     *          ...
     *      ],
     *      tempo_vizinho_mais_proximo: 123.45, // em ms
     *      insercao_mais_barata: [
     *          {id: 1, path: [[0, 1, 7, 0]], cost: 115.8, load: 42 },
     *          {id: 2, path: [[0, 3, 5, 0]], cost: 115.8, load: 30 },
     *          ...
     *      ],
     *      tempo_insercao_mais_barata: 123.45, // em ms
     * }
     */
    server.Get("/melhorarGulosos", [](const httplib::Request&, httplib::Response& res){
        res.set_header("Access-Control-Allow-Origin", "*");
    });


    /**
     * @brief
     * Essa função é chamada quando o usuário seleciona a opção "VND"
     * A função de VND aplicará movimentos de vizinhança para encontrar melhores soluções para o que já temos.
     * 
     * 
     * @returns:
     * {
     *      success: false,
     *      message: "falha ao ..."
     * }
     * 
     * {
     *      success: true,
     *      vizinho_mais_proximo: [
     *          {id: 1, path: [[0, 1, 7, 0]], cost: 115.8, load: 42 },
     *          {id: 2, path: [[0, 3, 5, 0]], cost: 115.8, load: 30 },
     *          ...
     *      ],
     *      tempo_vizinho_mais_proximo: 123.45, // em ms
     *      insercao_mais_barata: [
     *          {id: 1, path: [[0, 1, 7, 0]], cost: 115.8, load: 42 },
     *          {id: 2, path: [[0, 3, 5, 0]], cost: 115.8, load: 30 },
     *          ...
     *      ],
     *     tempo_insercao_mais_barata: 123.45, // em ms
     * }
     */
    server.Get("/aplicarVND", [](const httplib::Request&, httplib::Response& res){
        res.set_header("Access-Control-Allow-Origin", "*");
    });

    /**
     * @brief
     * Essa função é chamada quando o usuário seleciona a opção "ILS"
     * A função de ILS aplicará perturbações para encontrar melhores soluções para o que já temos.
     * 
     * 
     * @returns:
     * {
     *      success: false,
     *      message: "falha ao ..."
     * }
     * 
     * {
     *      success: true,
     *      vizinho_mais_proximo: [
     *          {id: 1, path: [[0, 1, 7, 0]], cost: 115.8, load: 42 },
     *          {id: 2, path: [[0, 3, 5, 0]], cost: 115.8, load: 30 },
     *          ...
     *      ],
     *      tempo_vizinho_mais_proximo: 123.45, // em ms
     *      insercao_mais_barata: [
     *          {id: 1, path: [[0, 1, 7, 0]], cost: 115.8, load: 42 },
     *          {id: 2, path: [[0, 3, 5, 0]], cost: 115.8, load: 30 },
     *          ...
     *      ],
     *     tempo_insercao_mais_barata: 123.45, // em ms
     * }
     */
    server.Post("/aplicarILS", [](const httplib::Request&, httplib::Response& res){
        res.set_header("Access-Control-Allow-Origin", "*");
    });


    cout << "Back-end is running on http://localhost:4000" << endl;
    server.listen("localhost", 4000);

    return 0;
}