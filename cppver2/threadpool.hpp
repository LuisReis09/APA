#ifndef _THREADPOOL_HPP_
#define _THREADPOOL_HPP_

    #include <iostream>
    #include <pthread.h>
    #include <vector>
    #include <queue>
    #include <functional>

    using namespace std;

    class ThreadPool
    {
        private:

            /**
            * Estrutura que define uma task: uma função e seus parâmetros.
            */
            typedef struct{
                void (*task)(void *arg);
                void *arg;
            } Task;

            /**
            * Variável para indicar se as threads devem parar ou não.
            *
            * - `true`: as threads devem parar de processar tarefas e sair. 
            * - `false`: as threads continuam processando tarefas normalmente.
            */
            bool stop = false;

            /**
            * Variável para indicar se as threads devem encerrar ou não.
            *
            * - `true`: as threads devem parar de processar tarefas e encerrar. 
            * - `false`: as threads continuam processando tarefas normalmente.
            */
            bool kill = false;


            /*
            * Vetor de threads que compõem o pool de threads.
            *
            * Cada thread no vetor é uma instância da classe `pthread_t`, que representa uma thread do sistema operacional.
            */
            vector<pthread_t> threads;

            /*
            * Fila de tarefas a serem executadas pelas threads do pool.
            *
            * Cada tarefa é uma função que não recebe parâmetros e não retorna valor (`function<void()>`).
            * As threads retiram tarefas dessa fila para processá-las.
            */
            queue<Task> tasks;

            /*
            * Mutex para proteger o acesso à fila de tarefas.
            *
            * Garante que apenas uma thread possa acessar a fila de tarefas ao mesmo tempo, evitando condições de corrida.
            */
            pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

            /*
            * Variável de condição para notificar as threads quando uma nova tarefa é adicionada à fila.
            *
            * As threads aguardam nessa variável de condição até que uma nova tarefa esteja disponível.
            */
            pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;

            // Pode guardar uma variável compartilhada, útil para ILS, por exemplo, que as threads compartilham o mel
            void* shared_var = nullptr;
            pthread_mutex_t sh_mtx = PTHREAD_MUTEX_INITIALIZER;

            /*
            * Função que será executada por cada thread do pool.
            *
            * Essa função fica em loop, aguardando novas tarefas na fila. Quando uma tarefa é adicionada, a thread a executa.
            * Se `stop` for verdadeiro e `kill` também, a thread sai do loop e termina sua execução
            * 
            * @param arg Ponteiro para o objeto `ThreadPool` que contém a fila de tarefas e outras informações necessárias.
            * @return `nullptr` quando a thread termina sua execução
            */
            static void *ThreadFunction(void *arg){
                // Converte o argumento recebido para um ponteiro do tipo ThreadPool
                ThreadPool* pool = static_cast<ThreadPool*>(arg);

                while(1){
                    // Variável para armazenar a tarefa a ser executada
                    Task to_execute;

                    // Lock para pegar a tarefa da fila
                    pthread_mutex_lock(&pool->mtx);

                    // Enquanto não houver tarefas e stop for falso, espera pela notificação
                    while(!pool->stop && pool->tasks.empty()){
                        pthread_cond_wait(&pool->cond_var, &pool->mtx);
                    }

                    // Verifica se o programa está encerrando...
                    if (pool->stop && pool->kill) {
                        pthread_mutex_unlock(&pool->mtx);
                        break;
                    }

                    // Pega uma tarefa da fila
                    to_execute = move(pool->tasks.front());
                    pool->tasks.pop();

                    // Libera o lock para permitir que outras threads acessem a fila
                    pthread_mutex_unlock(&pool->mtx);

                    // Executa a tarefa passada.
                    to_execute.task(to_execute.arg);
                }

                return nullptr;
            }

        public:
            ThreadPool(int qtd_threads) {
                threads.resize(qtd_threads);

                for(int i=0; i<qtd_threads; i++){
                    pthread_create(&threads[i], NULL, ThreadFunction, this);
                }
            }

            /*
            * Adiciona uma nova tarefa à fila de tarefas do pool.
            *
            * Essa função é chamada para adicionar uma nova tarefa que será executada por uma das threads do pool.
            *
            * @param task A tarefa a ser adicionada à fila. Deve ser uma função que não recebe parâmetros e não retorna valor.
            */
            void EnqueueTask(void (*task)(void*), void *parameters){
                // Lock para garantir que apenas uma thread acesse a fila de tarefas ao mesmo tempo
                pthread_mutex_lock(&mtx);

                // Adiciona a tarefa passada a lista de tarefas
                tasks.push({task, parameters});

                // Despausa a execução, caso esteja travada.
                stop = 0;

                // Acorda uma thread para que ela receba a tarefa
                pthread_cond_signal(&cond_var);

                // Libera o lock, para que outras threas tenham acesso a fila de tarefas
                pthread_mutex_unlock(&mtx);
            }

    
            void StopAllThreads(){
                pthread_mutex_lock(&mtx);
                stop = 1;
                tasks = queue<Task>(); // limpa a fila de tarefas
                pthread_cond_broadcast(&cond_var);
                pthread_mutex_unlock(&mtx);
            }

            template <typename T>
            void setSharedVar(T* var){
                pthread_mutex_lock(&sh_mtx);
                shared_var = static_cast<void*>(var);
                pthread_mutex_unlock(&sh_mtx);
            }

            template <typename T>
            T* getSharedVar(){
                pthread_mutex_lock(&sh_mtx);
                T* var = static_cast<T*>(shared_var);
                pthread_mutex_unlock(&sh_mtx);
                return var;
            }

            ~ThreadPool(){
                kill = 1;
                StopAllThreads();
                pthread_mutex_destroy(&mtx);
                pthread_cond_destroy(&cond_var);
            }
    };

#endif