import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
import { Separator } from "@/components/ui/separator";
import { 
  MapPin, 
  TrendingUp, 
  Zap, 
  RefreshCw, 
  Target,
  ArrowRight,
  Truck,
  Route
} from "lucide-react";

const ExplanationsTab = () => {
  const algorithms = [
    {
      name: "Vizinho Mais Próximo",
      icon: <MapPin className="h-5 w-5 text-route-1" />,
      category: "Algoritmo Guloso",
      description: "Constrói rotas escolhendo a estação mais próxima não visitada.",
      steps: [
        "Inicia todas as rotas no depósito (nó 0)",
        "Cria a fila de prioridades para cada rota, com as estações não visitadas",
        "Escolhe a melhor rota pra inserir uma próxima estação, minimizando o custo incremental e respeitando a capacidade do veículo",
        "Insere a estação de menor custo na rota escolhida",
        "Atualiza a fila de prioridades da rota afetada",
        "Retira a estação do conjuntos de estações não visitadas",
        "Retira a estação visitada das filas de prioridades das outras rotas",
        "Repete até todas as estações serem visitadas ou não poderem ser inseridas em nenhuma rota",
        "Finaliza todas as rotas retornando ao depósito (nó 0)"
      ],
      complexity: "O(n²)",
      pros: ["Simples de implementar", "Rápido para problemas pequenos"],
      cons: ["Pode gerar soluções de baixa qualidade", "Não considera capacidade do veículo globalmente"]
    },
    {
      name: "Inserção Mais Barata",
      icon: <TrendingUp className="h-5 w-5 text-route-2" />,
      category: "Algoritmo Guloso",
      description: "Insere estações na aresta que causa menor aumento no custo total da rota.",
      steps: [
        "Inicia com uma rota parcial, sendo o galpão e outras 2 estações que maximizem a distância entre si",
        "Para cada cliente não visitado, calcula o custo de inserção em cada aresta das rotas existentes",
        "Se o menor custo conseguido for menor que ir e voltar ao depósito, ou não sobrar veículos desocupados, insere a estação na posição correspondente",
        "Se o custo de ir e voltar ao depósito for menor, e ainda sobrar veículo, cria uma nova rota para essa estação",
        "Insere a estação entre a melhor aresta encontrada, invertendo a ordem da rota se for mais barato",
        "Repete até todos os clientes serem inseridos"
      ],
      complexity: "O(n³)",
      pros: ["Geralmente melhor qualidade que vizinho mais próximo", "Possibilita inversões na rota"],
      cons: ["Mais lento que vizinho mais próximo", "Ainda pode ficar preso em ótimos locais"]
    },
    {
      name: "Busca Local (Local Search)",
      icon: <Target className="h-5 w-5 text-route-3" />,
      category: "Algoritmo de Melhoria",
      description: "Melhora uma solução inicial explorando soluções vizinhas.",
      steps: [
        "Inicia com uma solução gulosa",
        "Pra cada estação nas rotas, verifica se reposicioná-la em outra posição reduz o custo total e respeita a capacidade",
        "Caso seja, tira a estação da rota que ela estava e reposiciona na rota destino",
        "Caso tenha feito uma realocação, chama a função recursivamente para tentar melhorar mais",
        "Caso não tenha feito nenhuma realocação, itera pelas próximas estações até que todas sejam testadas"
      ],
      complexity: "O(n² × iterações)",
      pros: ["Melhora a qualidade da solução inicial", "Relativamente simples"],
      cons: ["Pode convergir para ótimos locais", "Qualidade depende da solução inicial"]
    },
    {
      name: "Variable Neighborhood Descent (VND)",
      icon: <RefreshCw className="h-5 w-5 text-route-4" />,
      category: "Algoritmo de Melhoria",
      description: "Aplica diferentes estruturas de vizinhança sistematicamente.",
      steps: [
        "Define uma sequência de estruturas de vizinhança",
        "Aplica busca local com a primeira estrutura",
        "Se houver melhoria, reinicia com a primeira estrutura",
        "Senão, passa para a próxima estrutura",
        "Para quando todas as estruturas foram testadas sem melhoria"
      ],
      complexity: "O(k × n² × iterações)",
      pros: ["Mais eficaz que busca local simples", "Explora diferentes tipos de movimento"],
      cons: ["Mais complexo de implementar", "Tempo de execução maior"]
    },
    {
      name: "Iterated Local Search (ILS)",
      icon: <Zap className="h-5 w-5 text-route-5" />,
      category: "Meta-heurística",
      description: "Combina busca local com perturbações para escapar de ótimos locais.",
      steps: [
        "Recebe uma solução inicial",
        "Aplica busca local para obter ótimo local",
        "Aplica perturbação na solução",
        "Aplica busca local na solução perturbada",
        "Aceita ou rejeita a nova solução",
        "Repete por um número determinado de iterações",
        "Há também um número para restringir o número de iterações sem melhorias consecutivas"
      ],
      complexity: "O(iterações × (perturbação + busca_local))",
      pros: ["Pode escapar de ótimos locais", "Balance entre intensificação e diversificação", "Aumenta intensidade ao não encontrar melhorias"],
      cons: ["Requer ajuste de parâmetros", "Tempo de execução pode ser alto"]
    }
  ];

  return (
    <div className="space-y-6">
      {/* Header */}
      <Card className="bg-gradient-card border-border shadow-elegant">
        <CardHeader>
          <CardTitle className="flex items-center space-x-2">
            <Route className="h-6 w-6 text-primary" />
            <span>Algoritmos para Problema de Roteamento de Veículos</span>
          </CardTitle>
          <CardDescription>
            Explicação das estruturas e algoritmos implementados no sistema VRP
          </CardDescription>
        </CardHeader>
        <CardContent>
          <p className="text-muted-foreground">
            O Problema de Roteamento de Veículos (VRP) é um problema de otimização combinatória que busca 
            determinar as melhores rotas para uma frota de veículos atender um conjunto de clientes, 
            minimizando custos como distância total percorrida ou tempo de viagem.
          </p>
        </CardContent>
      </Card>

      {/* Algorithms */}
      <div className="grid gap-6">
        {algorithms.map((algorithm, index) => (
          <Card key={index} className="bg-gradient-card border-border shadow-elegant">
            <CardHeader>
              <div className="flex items-start justify-between">
                <div className="flex items-center space-x-3">
                  {algorithm.icon}
                  <div>
                    <CardTitle className="text-lg">{algorithm.name}</CardTitle>
                    <Badge variant="outline" className="mt-1">
                      {algorithm.category}
                    </Badge>
                  </div>
                </div>
                <Badge variant="secondary">
                  {algorithm.complexity}
                </Badge>
              </div>
              <CardDescription className="mt-3">
                {algorithm.description}
              </CardDescription>
            </CardHeader>
            <CardContent>
              <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
                {/* Steps */}
                <div className="lg:col-span-2 space-y-3">
                  <h4 className="font-medium text-foreground flex items-center space-x-2">
                    <ArrowRight className="h-4 w-4 text-primary" />
                    <span>Passos do Algoritmo</span>
                  </h4>
                  <ol className="space-y-2">
                    {algorithm.steps.map((step, stepIndex) => (
                      <li key={stepIndex} className="flex items-start space-x-3">
                        <span className="flex-shrink-0 w-6 h-6 bg-primary text-primary-foreground rounded-full flex items-center justify-center text-xs font-medium">
                          {stepIndex + 1}
                        </span>
                        <span className="text-sm text-muted-foreground">{step}</span>
                      </li>
                    ))}
                  </ol>
                </div>

                {/* Pros and Cons */}
                <div className="space-y-4">
                  <div>
                    <h4 className="font-medium text-green-400 mb-2">Vantagens</h4>
                    <ul className="space-y-1">
                      {algorithm.pros.map((pro, proIndex) => (
                        <li key={proIndex} className="text-sm text-muted-foreground flex items-start space-x-2">
                          <span className="text-green-400 mt-1">•</span>
                          <span>{pro}</span>
                        </li>
                      ))}
                    </ul>
                  </div>
                  
                  <Separator />
                  
                  <div>
                    <h4 className="font-medium text-red-400 mb-2">Desvantagens</h4>
                    <ul className="space-y-1">
                      {algorithm.cons.map((con, conIndex) => (
                        <li key={conIndex} className="text-sm text-muted-foreground flex items-start space-x-2">
                          <span className="text-red-400 mt-1">•</span>
                          <span>{con}</span>
                        </li>
                      ))}
                    </ul>
                  </div>
                </div>
              </div>
            </CardContent>
          </Card>
        ))}
      </div>

      {/* Footer */}
      <Card className="bg-gradient-card border-border shadow-elegant">
        <CardContent className="pt-6">
          <div className="text-center space-y-2">
            <div className="flex items-center justify-center space-x-2 text-primary">
              <Truck className="h-5 w-5" />
              <span className="font-medium">Implementação VRP - UFPB</span>
            </div>
            <p className="text-sm text-muted-foreground">
              Sistema desenvolvido para demonstrar diferentes abordagens de solução do problema VRP
            </p>
          </div>
        </CardContent>
      </Card>
    </div>
  );
};

export default ExplanationsTab;