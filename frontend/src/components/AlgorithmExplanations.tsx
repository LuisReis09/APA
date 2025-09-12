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
        "Inicia no depósito (nó 0)",
        "A cada passo, visita o nó mais próximo ainda não visitado",
        "Quando a capacidade do veículo é atingida, retorna ao depósito",
        "Inicia uma nova rota com um novo veículo"
      ],
      complexity: "O(n²)",
      advantages: ["Simples de implementar", "Rápido", "Boa solução inicial"],
      disadvantages: ["Pode gerar soluções subótimas", "Sensível ao ponto de partida"]
    },
    {
      id: "cheapest-insertion",
      name: "Inserção Mais Barata",
      category: "Construção Gulosa", 
      icon: Zap,
      description: "Constrói rotas inserindo nós na posição que causa menor aumento no custo total.",
      steps: [
        "Inicia com uma rota parcial",
        "Para cada nó não visitado, calcula o custo de inserção em cada posição",
        "Insere o nó na posição de menor custo",
        "Repete até todos os nós serem visitados"
      ],
      complexity: "O(n³)",
      advantages: ["Melhor qualidade que vizinho mais próximo", "Considera impacto global"],
      disadvantages: ["Mais lento que vizinho mais próximo", "Ainda pode ser subótimo"]
    },
    {
      id: "local-improvement",
      name: "Melhoria Local",
      category: "Otimização",
      icon: TrendingUp,
      description: "Aplica movimentos locais para melhorar uma solução existente.",
      steps: [
        "Recebe uma solução inicial",
        "Aplica movimentos como 2-opt, swap, relocate",
        "Aceita movimentos que melhoram a solução",
        "Para quando não há mais melhorias possíveis"
      ],
      complexity: "O(n²) por iteração",
      advantages: ["Melhora qualidade da solução", "Convergência garantida"],
      disadvantages: ["Pode ficar preso em ótimos locais", "Depende da solução inicial"]
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
        "Gera solução inicial e aplica busca local",
        "Aplica perturbação na solução atual",
        "Executa busca local na solução perturbada",
        "Aceita nova solução baseado em critério",
        "Repete até critério de parada"
      ],
      complexity: "Depende da busca local usada",
      advantages: ["Bom balanço intensificação/diversificação", "Resultados de alta qualidade"],
      disadvantages: ["Requer ajuste de parâmetros", "Tempo de execução elevado"]
    }
  ];

  return (
    <div className="space-y-6">
      <Card className="shadow-card bg-gradient-accent text-accent-foreground">
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