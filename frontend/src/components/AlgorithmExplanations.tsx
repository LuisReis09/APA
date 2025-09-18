import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
import { 
  BookOpen, 
  Zap, 
  Target, 
  Search, 
  RefreshCw, 
  TrendingUp,
  ArrowRight
} from "lucide-react";

export const AlgorithmExplanations = () => {
  const algorithms = [
    {
      id: "nearest-neighbor",
      name: "Vizinho Mais Próximo",
      category: "Construção Gulosa",
      icon: Target,
      description: "Algoritmo construtivo que sempre escolhe o próximo nó mais próximo ainda não visitado.",
      steps: [
        "Inicia cada veículo (rota) no depósito, ou seja, nó 0",
        "Para cada rota, constrói sua fila de prioridades com os nós não visitados, ordenados pela distância ao nó atual",
        "Escolhe a rota que tiver o menor custo de inserção para ser atualizada",
        "Adiciona em tal rota sua estação de maior prioridade (nó mais próximo)",
        "Atualiza a fila de prioridades da rota atual",
        "Retira das outras rotas a possibilidade de inserir o nó adicionado",
        "Repete os passos 3 a 6 até que todos os nós sejam visitados",
        "Retorna as rotas construídas"
      ],
      complexity: "O(n²)",
      advantages: ["Simples de implementar", "Rápido", "Solução inicial fácil e intuitiva"],
      disadvantages: ["Pode gerar soluções medianas", "Sensível ao ponto de partida"]
    },
    {
      id: "cheapest-insertion",
      name: "Inserção Mais Barata",
      category: "Construção Gulosa", 
      icon: Zap,
      description: "Constrói rotas inserindo nós na posição que causa menor aumento no custo total.",
      steps: [
        "Inicia com uma rota parcial",
        "Para cada nó não visitado, calcula o custo de inserção em cada aresta das rotas existentes, inclusive o caso de inverter a rota",
        "Verifica se a inserção respeita as restrições de capacidade e demanda",
        "Se o custo de ir do galpão até tal estação e voltar for menor que o custo de inseri-la em qualquer rota, cria uma nova rota para ela",
        "Insere o nó na posição que gera o menor aumento de custo",
        "Repete os passos 2 a 5 até que todos os nós sejam visitados",
        "Retorna as rotas construídas"
      ],
      complexity: "O(n³)",
      advantages: ["Melhor qualidade que vizinho mais próximo", "Considera impacto global"],
      disadvantages: ["Mais lento que vizinho mais próximo", "Ainda pode ser subótimo"]
    },
    {
      id: "vnd",
      name: "Variable Neighborhood Descent (VND)",
      category: "Metaheurística",
      icon: Search,
      description: "Explora sistematicamente diferentes estruturas de vizinhança.",
      steps: [
        "Define conjunto de estruturas de vizinhança",
        "Para cada estrutura, aplica melhoria local",
        "Se encontra melhoria, reinicia com primeira estrutura",
        "Senão, passa para próxima estrutura"
      ],
      complexity: "Depende das vizinhanças usadas",
      advantages: ["Explora múltiplas vizinhanças", "Escapa de ótimos locais"],
      disadvantages: ["Mais complexo", "Tempo de execução maior"]
    },
    {
      id: "ils",
      name: "Iterated Local Search (ILS)",
      category: "Metaheurística",
      icon: RefreshCw,
      description: "Combina busca local intensiva com perturbações para escapar de ótimos locais.",
      steps: [
        "Recebe solução inicial e aplica busca local",
        "Aplica perturbação na solução atual",
        "Executa busca local na solução perturbada",
        "Aceita nova solução baseado em critério",
        "Repete até critério de parada"
      ],
      complexity: "Depende da busca local usada",
      advantages: ["Bom balanço intensificação/diversificação", "Resultados de alta qualidade", "Flexibilidade de criação de perturbações"],
      disadvantages: ["Requer ajuste de parâmetros", "Tempo de execução elevado"]
    }
  ];

  return (
    <div className="space-y-6">
      <Card className="shadow-card bg-purple-700 text-accent-foreground">
        <CardHeader>
          <CardTitle className="flex items-center gap-2">
            <BookOpen className="w-5 h-5" />
            Algoritmos para o Problema das Rotas de Veículos
          </CardTitle>
          <CardDescription className="text-accent-foreground/80">
            Compreenda como funcionam os algoritmos implementados neste projeto
          </CardDescription>
        </CardHeader>
      </Card>

      <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
        {algorithms.map((algorithm) => {
          const Icon = algorithm.icon;
          return (
            <Card key={algorithm.id} className="shadow-card">
              <CardHeader>
                <div className="flex items-start justify-between">
                  <div className="flex items-center gap-3">
                    <div className="p-2 rounded-lg bg-primary/10">
                      <Icon className="w-5 h-5 text-primary" />
                    </div>
                    <div>
                      <CardTitle className="text-lg">{algorithm.name}</CardTitle>
                      <Badge variant="secondary" className="mt-1">
                        {algorithm.category}
                      </Badge>
                    </div>
                  </div>
                </div>
                <CardDescription className="mt-3">
                  {algorithm.description}
                </CardDescription>
              </CardHeader>
              
              <CardContent className="space-y-4">
                <div>
                  <h4 className="font-semibold text-sm mb-2 flex items-center gap-2">
                    <ArrowRight className="w-4 h-4" />
                    Funcionamento
                  </h4>
                  <ol className="space-y-1 text-sm text-muted-foreground">
                    {algorithm.steps.map((step, index) => (
                      <li key={index} className="flex gap-2">
                        <span className="text-primary font-medium min-w-[20px]">
                          {index + 1}.
                        </span>
                        <span>{step}</span>
                      </li>
                    ))}
                  </ol>
                </div>

                <div className="grid grid-cols-1 sm:grid-cols-2 gap-4 pt-2">
                  <div>
                    <h5 className="font-medium text-sm text-accent mb-1">Vantagens</h5>
                    <ul className="space-y-1 text-xs text-muted-foreground">
                      {algorithm.advantages.map((advantage, index) => (
                        <li key={index} className="flex items-start gap-1">
                          <span className="text-accent mt-0.5">•</span>
                          <span>{advantage}</span>
                        </li>
                      ))}
                    </ul>
                  </div>
                  
                  <div>
                    <h5 className="font-medium text-sm text-destructive mb-1">Desvantagens</h5>
                    <ul className="space-y-1 text-xs text-muted-foreground">
                      {algorithm.disadvantages.map((disadvantage, index) => (
                        <li key={index} className="flex items-start gap-1">
                          <span className="text-destructive mt-0.5">•</span>
                          <span>{disadvantage}</span>
                        </li>
                      ))}
                    </ul>
                  </div>
                </div>

                <div className="pt-2 border-t">
                  <span className="text-xs text-muted-foreground">
                    <strong>Complexidade:</strong> {algorithm.complexity}
                  </span>
                </div>
              </CardContent>
            </Card>
          );
        })}
      </div>
    </div>
  );
};