import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
import { BarChart, TrendingUp, Clock, Target } from "lucide-react";

interface Route {
  id: number;
  path: number[];
  cost: number;
  load: number;
}

type AlgorithmType = "greedy" | "optimization";

interface StatisticsProps {
  greedyResults?: {
    nearestNeighbor: Route[];
    cheapestInsertion: Route[];
  };
  optimizationResults?: {
    localSearch?: Route[];
    vnd?: Route[];
    ils?: Route[];
  };
}

const Statistics = ({ greedyResults, optimizationResults }: StatisticsProps) => {
  if (!greedyResults) return null;

  const calculateTotalCost = (routes: Route[]) => {
    return routes.reduce((sum, route) => sum + route.cost, 0);
  };

  const calculateTotalLoad = (routes: Route[]) => {
    return routes.reduce((sum, route) => sum + route.load, 0);
  };

  const calculateImprovement = (originalCost: number, newCost: number) => {
    return ((originalCost - newCost) / originalCost * 100);
  };

  const nnCost = calculateTotalCost(greedyResults.nearestNeighbor);
  const ciCost = calculateTotalCost(greedyResults.cheapestInsertion);
  const bestGreedyCost = Math.min(nnCost, ciCost);

  const stats: Array<{
    label: string;
    cost: number;
    load: number;
    gap: number;
    type: AlgorithmType;
  }> = [
    {
      label: "Vizinho Mais Próximo",
      cost: nnCost,
      load: calculateTotalLoad(greedyResults.nearestNeighbor),
      gap: 0,
      type: "greedy"
    },
    {
      label: "Inserção Mais Barata", 
      cost: ciCost,
      load: calculateTotalLoad(greedyResults.cheapestInsertion),
      gap: calculateImprovement(nnCost, ciCost),
      type: "greedy"
    }
  ];

  // Add optimization results if available
  if (optimizationResults) {
    Object.entries(optimizationResults).forEach(([algorithm, routes]) => {
      if (routes) {
        const cost = calculateTotalCost(routes);
        stats.push({
          label: algorithm.replace(/([A-Z])/g, ' $1').trim(),
          cost,
          load: calculateTotalLoad(routes),
          gap: calculateImprovement(bestGreedyCost, cost),
          type: "optimization"
        });
      }
    });
  }

  const bestResult = stats.reduce((best, current) => 
    current.cost < best.cost ? current : best
  );

  return (
    <Card className="bg-gradient-card border-border shadow-elegant">
      <CardHeader>
        <CardTitle className="flex items-center space-x-2">
          <BarChart className="h-5 w-5 text-primary" />
          <span>Estatísticas Gerais</span>
        </CardTitle>
        <CardDescription>
          Comparativo de desempenho entre os algoritmos executados
        </CardDescription>
      </CardHeader>
      <CardContent>
        <div className="space-y-6">
          {/* Summary Cards */}
          <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
            <Card className="bg-muted/20">
              <CardContent className="p-4 text-center">
                <Target className="h-6 w-6 text-primary mx-auto mb-2" />
                <p className="text-sm text-muted-foreground">Melhor Solução</p>
                <p className="font-semibold">{bestResult.label}</p>
                <p className="text-lg font-bold text-primary">{bestResult.cost.toFixed(2)}</p>
              </CardContent>
            </Card>
            
            <Card className="bg-muted/20">
              <CardContent className="p-4 text-center">
                <TrendingUp className="h-6 w-6 text-accent mx-auto mb-2" />
                <p className="text-sm text-muted-foreground">Melhor Melhoria</p>
                <p className="font-semibold">
                  {stats.filter(s => s.gap > 0).sort((a, b) => b.gap - a.gap)[0]?.label || 'N/A'}
                </p>
                <p className="text-lg font-bold text-accent">
                  {Math.max(...stats.map(s => s.gap)).toFixed(1)}%
                </p>
              </CardContent>
            </Card>
            
            <Card className="bg-muted/20">
              <CardContent className="p-4 text-center">
                <Clock className="h-6 w-6 text-route-3 mx-auto mb-2" />
                <p className="text-sm text-muted-foreground">Algoritmos Testados</p>
                <p className="font-semibold">
                  {stats.filter(s => s.type === 'optimization').length + 2}
                </p>
                <p className="text-lg font-bold text-route-3">
                  {stats.length} resultados
                </p>
              </CardContent>
            </Card>
          </div>

          {/* Detailed Results Table */}
          <div className="space-y-3">
            <h3 className="font-semibold text-foreground">Resultados Detalhados</h3>
            <div className="space-y-2">
              {stats.map((stat, index) => (
                <div 
                  key={index}
                  className={`flex items-center justify-between p-3 rounded-lg ${
                    stat.label === bestResult.label 
                      ? 'bg-primary/10 border border-primary/20' 
                      : 'bg-muted/20'
                  }`}
                >
                  <div className="flex items-center space-x-3">
                    <Badge variant={stat.type === 'greedy' ? 'secondary' : 'default'}>
                      {stat.type === 'greedy' ? 'Guloso' : 'Otimização'}
                    </Badge>
                    <span className="font-medium">{stat.label}</span>
                    {stat.label === bestResult.label && (
                      <Badge variant="outline" className="text-primary border-primary">
                        Melhor
                      </Badge>
                    )}
                  </div>
                  
                  <div className="flex items-center space-x-6 text-sm">
                    <div className="text-right">
                      <p className="text-muted-foreground">Custo Total</p>
                      <p className="font-semibold">{stat.cost.toFixed(2)}</p>
                    </div>
                    <div className="text-right">
                      <p className="text-muted-foreground">Carga Total</p>
                      <p className="font-semibold">{stat.load}</p>
                    </div>
                    <div className="text-right">
                      <p className="text-muted-foreground">Gap</p>
                      <p className={`font-semibold ${
                        stat.gap > 0 ? 'text-green-400' : 
                        stat.gap < 0 ? 'text-red-400' : 'text-muted-foreground'
                      }`}>
                        {stat.gap === 0 ? 'Base' : `${stat.gap > 0 ? '+' : ''}${stat.gap.toFixed(1)}%`}
                      </p>
                    </div>
                  </div>
                </div>
              ))}
            </div>
          </div>
        </div>
      </CardContent>
    </Card>
  );
};

export default Statistics;