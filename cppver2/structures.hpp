#ifndef _STRUCTURES_H_
#define _STRUCTURES_H_

    #include <iostream>
    #include <vector>
    #include <string>
    #include <sstream>
    #include <fstream>

    using namespace std;

    class Problema
    {
        public:
            int qnt_veiculos;
            int capacidade_max;
            vector<int> demandas;
            int qnt_estacoes;
            int veiculos_disponiveis;
            vector<vector<int>> matriz_custo;

            Problema(string file_content, bool isFile = false){
                if(isFile){
                    ifstream mfile(file_content);
                    ostringstream outputStr;
                    outputStr << mfile.rdbuf();
                    file_content = outputStr.str();
                }

                istringstream input(file_content);
                string aux;

                // ---- Leitura básica ----
                if (!(input >> this->qnt_estacoes) || this->qnt_estacoes <= 0)
                    throw runtime_error("Numero invalido de estacoes.");

                if (!(input >> this->qnt_veiculos) || this->qnt_veiculos <= 0)
                    throw runtime_error("Numero invalido de veiculos.");

                if (!(input >> this->capacidade_max) || this->capacidade_max <= 0)
                    throw runtime_error("Capacidade maxima invalida.");

                this->veiculos_disponiveis = this->qnt_veiculos;

                getline(input, aux); // consome resto da linha

                // ---- Demandas ----
                this->demandas.resize(this->qnt_estacoes);
                for (int i = 0; i < this->qnt_estacoes; i++)
                {
                    if(!(input >> this->demandas[i]))
                        throw runtime_error("Demanda invalida para a estacao " + to_string(i+1) + ".");

                    if(abs(this->demandas[i]) > this->capacidade_max)
                        throw runtime_error("Demanda da estacao " + to_string(i+1) + " excede a capacidade maxima.");

                }

                getline(input, aux); // consome resto da linha

                // ---- Matriz de custos ----
                this->matriz_custo.resize(this->qnt_estacoes + 1,
                                            vector<int>(this->qnt_estacoes + 1));
                for (int i = 0; i <= this->qnt_estacoes; i++)
                {
                    for (int j = 0; j <= this->qnt_estacoes; j++)
                    {
                        if (!(input >> this->matriz_custo[i][j]))
                        {
                            throw runtime_error("Matriz de custo incompleta.");
                        }
                    }
                }
            }
    };

    extern Problema p;

    typedef enum
    {
        INICIO_FIM,
        FIM_INICIO
    } DirecaoRota;

    class Rota
    {
        public:
            // Caminho da rota: exemplo 0 -> 9 -> 5 -> 2 -> 0
            vector<int> estacoes;

            // Custos da rota, em cada direção: INICIO_FIM e FIM_INICIO
            int custo_total_d1, custo_total_d2;

            // Direção atual da rota
            DirecaoRota direcao_atual;

            // Construtor padrão
            Rota() : custo_total_d1(0), custo_total_d2(0), direcao_atual(INICIO_FIM) {
                estacoes.push_back(0); // Início (depósito)
                estacoes.push_back(0); // Fim (depósito)
            }

            Rota &operator=(const Rota &other)
            {
                if (this != &other)
                {
                    estacoes = other.estacoes;
                    custo_total_d1 = other.custo_total_d1;
                    custo_total_d2 = other.custo_total_d2;
                    direcao_atual = other.direcao_atual;
                }
                return *this;
            }

            void PrintRota() const {
                cout << "Custo total (d1): " << custo_total_d1 << endl;
                cout << "Custo total (d2): " << custo_total_d2 << endl;
                for(const int& e : estacoes){
                    cout << e << " ";
                }
                cout << endl;
            }

            void InsertEnd(int estacao){
                int ultima_estacao;
                if(this->direcao_atual == DirecaoRota::INICIO_FIM){
                    ultima_estacao = this->estacoes[this->estacoes.size()-2]; // ignora a garagem
                }else{
                    ultima_estacao = this->estacoes[1]; // ignora a garagem
                }

                // atualiza na direção 1
                int novo_custo= p.matriz_custo[ultima_estacao][estacao] + p.matriz_custo[estacao][0] -
                                p.matriz_custo[ultima_estacao][0];

                this->custo_total_d1 += novo_custo;
                this->custo_total_d2 += novo_custo;

                if(this->direcao_atual == DirecaoRota::INICIO_FIM){
                    this->estacoes.insert(this->estacoes.end() - 1, estacao);
                }else{
                    this->estacoes.insert(this->estacoes.begin() + 1, estacao);
                }
            }

            void InsertAt(int indice, int estacao){
                if(indice <= 0) throw runtime_error("Tentativa de insercao em indice negativo ou na garagem (invalido)");
                if(indice >= (this->estacoes.size()-1)) indice = this->estacoes.size()-2;

                // custo 1
                int novo_custo= p.matriz_custo[this->estacoes[indice]][estacao] + p.matriz_custo[estacao][this->estacoes[indice+1]] -
                p.matriz_custo[this->estacoes[indice]][this->estacoes[indice+1]];
                this->custo_total_d1 += novo_custo;
                // custo 2
                novo_custo= p.matriz_custo[this->estacoes[indice+1]][estacao] + p.matriz_custo[estacao][this->estacoes[indice]] -
                p.matriz_custo[this->estacoes[indice+1]][this->estacoes[indice]];
                this->custo_total_d2 += novo_custo;

                if(this->direcao_atual == DirecaoRota::INICIO_FIM){
                    this->estacoes.insert(this->estacoes.begin() + indice, estacao);
                    
                }else{
                    this->estacoes.insert(this->estacoes.begin() + (int)(this->estacoes.size()) - indice, estacao);
                }
            }

            void RemoveAt(int indice){
                int n = (int) this->estacoes.size();

                if(indice <= 0 || indice >= n-1)
                    throw runtime_error("Remocao invalida (Garagem)");

                // custo na direcao 1
                int custo =   p.matriz_custo[this->estacoes[indice-1]][this->estacoes[indice+1]]
                            - p.matriz_custo[this->estacoes[indice-1]][this->estacoes[indice]]
                            + p.matriz_custo[this->estacoes[indice]][this->estacoes[indice+1]];
                this->custo_total_d1 += custo;
                // custo na direcao 2
                custo =   p.matriz_custo[this->estacoes[indice+1]][this->estacoes[indice-1]]
                            - p.matriz_custo[this->estacoes[indice+1]][this->estacoes[indice]]
                            + p.matriz_custo[this->estacoes[indice]][this->estacoes[indice-1]];
                this->custo_total_d2 += custo;

                if(this->direcao_atual == INICIO_FIM){
                    this->estacoes.erase(this->estacoes.begin() + indice);
                }else{
                    this->estacoes.erase(this->estacoes.begin() + n - indice);
                }
            }
    };

    class Solucao
    {
        public:
            int custo_total;
            int veiculos_usados;
            int veiculos_disponiveis;
            vector<Rota> rotas;

            void SalvarSolucao(string nome_arquivo) const {
                ofstream output(nome_arquivo);

                output << custo_total << endl;
                output << veiculos_usados << endl;
                for (const Rota &r : rotas)
                {
                    for (const int &e : r.estacoes)
                    {
                        output << e << " ";
                    }
                    output << endl;
                }

                output.close();
            }
    };

    typedef struct{
        Solucao solucao;
        int i_rota;
        Rota rota_retorno;
    } VND_attr;

    typedef struct{
        int id_execucao;
        Solucao solucao;
        int max_iteracoes;
        int max_sem_melhora;
        Solucao* resultado;
        int pool_count;
    } ParametrosILS;

#endif