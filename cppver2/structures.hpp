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
            int max_iteracoes = 2000;
            int max_sem_melhora = 200;
            int valor_otimo = -1;

            Problema() : qnt_veiculos(0), capacidade_max(0), qnt_estacoes(0), veiculos_disponiveis(0) {}

            Problema(string file_content, bool isFile = false){
                this->init(file_content, isFile);
            }

            void init(string file_content, bool isFile = false){
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

            void PrintProblema() const {
                cout << "Numero de estacoes: " << this->qnt_estacoes << endl;
                cout << "Numero de veiculos: " << this->qnt_veiculos << endl;
                cout << "Capacidade maxima: " << this->capacidade_max << endl;
                cout << "Demandas: ";
                for(const int& d : this->demandas){
                    cout << d << " ";
                }
                cout << endl;

                cout << "Matriz de custos: " << endl;
                for(int i = 0; i <= this->qnt_estacoes; i++){
                    for(int j = 0; j <= this->qnt_estacoes; j++){
                        cout << this->matriz_custo[i][j] << " ";
                    }
                    cout << endl;
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
                for(const int& e : estacoes){
                    cout << e << " ";
                }
                cout << endl;
            }

            int RefreshCost(){
                this->custo_total_d1 = 0;
                for(int i = 0; i < this->estacoes.size() - 1; i++){
                    this->custo_total_d1 += p.matriz_custo[this->estacoes[i]][this->estacoes[i+1]];
                }

                return this->custo_total_d1;
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

                if(indice <= 0 || indice >= n-1){
                    cout << "Tentou remover indice " << indice << " de rota com tamanho " << n << endl;
                    this->PrintRota();
                    throw runtime_error("Remocao invalida (Garagem)");
                }

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
            vector<vector<int>> rotas;

            void SalvarSolucao(string nome_arquivo) const {
                ofstream output(nome_arquivo);

                output << custo_total << endl;
                output << this->rotas.size() << endl;
                for (const vector<int>& r : rotas)
                {
                    for (const int &e : r)
                    {
                        output << e << " ";
                    }
                    output << endl;
                }

                output.close();
            }

            int AtualizarCusto() {
                this->custo_total = 0;
                for (const auto& rota : rotas) {
                    this->custo_total += this->CalculaCusto(rota);
                }
                return this->custo_total;
            }

            private:
            int CalculaCusto(vector<int> rota){
                int custo = 0;
                for(int i = 0; i < rota.size() - 1; i++){
                    custo += p.matriz_custo[rota[i]][rota[i+1]];
                }
                return custo;
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

    class No
    {
    public:
        int estacao;
        No *proximo;  // Ponteiro para próximo nó
        No *anterior; // Ponteiro para o nó anterior

        No(int e = 0) : estacao(e), proximo(nullptr), anterior(nullptr) {}
    };

    class LinkedList{
    No *rota_i; // Início da lista encadeada
        No *rota_f; // Fim da lista encadeada
        int custo_total_d1, custo_total_d2;
        int rotaTam; // Número de estações na rota (excluindo os depósitos)
        DirecaoRota direcao_atual;

        // Construtor padrão
        LinkedList() : custo_total_d1(0), custo_total_d2(0),
                rotaTam(0), direcao_atual(INICIO_FIM)
        {
            rota_i = new No(0); // Nó inicial (depósito)
            rota_f = new No(0); // Nó final (depósito)
            rota_i->proximo = rota_f;
            rota_f->anterior = rota_i;
        }

        // Construtor de cópia (deep copy)
        LinkedList(const LinkedList &other)
        {
            custo_total_d1 = other.custo_total_d1;
            custo_total_d2 = other.custo_total_d2;
            rotaTam = other.rotaTam;
            direcao_atual = other.direcao_atual;
            rota_i = copiaLista(other.rota_i, other.rota_f, &rota_f);
        }

        // Operador de atribuição (deep copy)
        LinkedList &operator=(const LinkedList &other)
        {
            if (this == &other)
                return *this;

            limpa(); // libera a lista atual

            custo_total_d1 = other.custo_total_d1;
            custo_total_d2 = other.custo_total_d2;
            rotaTam = other.rotaTam;
            direcao_atual = other.direcao_atual;
            rota_i = copiaLista(other.rota_i, other.rota_f, &rota_f);

            return *this;
        }

        int stationAtIndex(int posicao)
        {
            if (posicao > (rotaTam + 1) || posicao < 0)
                return -1;

            No *atual = rota_i;
            int index_atual = 0;
            while (index_atual != posicao)
            {
                atual = atual->proximo;
                index_atual++;
            }
            return index_atual;
        }

        void InsertAt(int posicao, int estacao)
        {
            extern Problema p;
            if (posicao < 1 || posicao > (rotaTam + 1))
                return; // Posição inválida

            // Se a posição for mais perto do início, percorre a partir do início
            // Caso contrário, percorre a partir do fim

            No *novo = new No(estacao);
            No *atual;

            if (posicao <= rotaTam / 2)
            {
                atual = rota_i->proximo;
                for (int i = 1; i < posicao; i++)
                    atual = atual->proximo;

                // Quando eu chego na posição que eu quero, a estação vai ser o novo elemento daquela posição
                // Ou seja, o atual que está na posição posicao vai ser empurrado para frente, ficando na posição posicao + 1
                novo->proximo = atual;
                novo->anterior = atual->anterior;
                atual->anterior->proximo = novo;
                atual->anterior = novo;
                rotaTam++;
            }
            else
            {
                atual = rota_f->anterior;
                for (int i = rotaTam; i > posicao; i--)
                    atual = atual->anterior;

                // Quando eu chego na posição que eu quero, a estação vai ser o novo elemento daquela posição
                // Ou seja, o atual que está na posição posicao vai ser empurrado para frente, ficando na posição posicao + 1
                novo->proximo = atual;
                novo->anterior = atual->anterior;
                atual->anterior->proximo = novo;
                atual->anterior = novo;
                rotaTam++;
            }

            // Atualiza custos da direcao INICIO_FIM
            custo_total_d1 -= p.matriz_custo[novo->anterior->estacao][atual->estacao];
            custo_total_d1 += p.matriz_custo[novo->anterior->estacao][estacao] + p.matriz_custo[estacao][atual->estacao];

            // Atualiza custos da direcao FIM_INICIO
            custo_total_d2 -= p.matriz_custo[atual->estacao][novo->anterior->estacao];
            custo_total_d2 += p.matriz_custo[atual->estacao][estacao] + p.matriz_custo[estacao][novo->anterior->estacao];

        }

        void InsertEnd(int estacao)
        {
            extern Problema p;
            No *novo = new No(estacao);
            novo->anterior = rota_f->anterior;
            novo->proximo = rota_f;
            rota_f->anterior->proximo = novo;
            rota_f->anterior = novo;
            rotaTam++;

            // Retira o custo matriz[rota_f->anterior->estacao][0] do custo_total_d1
            // E incrementa o custo da antiga última estação até a nova estação + o custo da nova estação até o galpão
            custo_total_d1 -= p.matriz_custo[novo->anterior->estacao][0];
            custo_total_d1 += p.matriz_custo[novo->anterior->estacao][estacao] + p.matriz_custo[estacao][0];

            // Retira o custo matriz[0][rota_f->anterior->estacao] do custo_total_d2
            // E incrementa o custo do galpão até a nova estação + o custo da nova estação até a antiga última estação
            custo_total_d2 -= p.matriz_custo[0][novo->anterior->estacao];
            custo_total_d2 += p.matriz_custo[0][estacao] + p.matriz_custo[estacao][novo->anterior->estacao];

            // Agora temos a estação inserida e o novo custo atualizado
        }
    }

#endif