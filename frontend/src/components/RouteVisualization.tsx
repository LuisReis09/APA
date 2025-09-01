import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";
import { Route as RouteIcon, MapPin, TrendingUp, Loader2, Truck } from "lucide-react";
import RouteGraphFlow from "./RouteGraphFlow";

interface Route {
  id: number;
  path: number[];
  cost: number;
  load: number;
}

interface RouteVisualizationProps {
  greedyResults?: {
    nearestNeighbor: Route[];
    cheapestInsertion: Route[];
  };
  optimizationResults?: {
    localSearch?: Route[];
    vnd?: Route[];
    ils?: Route[];
  };
  isProcessing: boolean;
}

const RouteVisualization = ({ greedyResults, optimizationResults, isProcessing }: RouteVisualizationProps) => {
  if (isProcessing) {
    return (
      <Card className="bg-gradient-card border-border shadow-elegant">
        <CardContent className="flex flex-col items-center justify-center py-12 space-y-4">
          <Loader2 className="h-8 w-8 text-primary animate-spin" />
          <div className="text-center">
            <p className="font-medium text-foreground">Processando...</p>
            <p className="text-sm text-muted-foreground">Aguarde enquanto os algoritmos são executados</p>
          </div>
        </CardContent>
      </Card>
    );
  }

  if (!greedyResults) return null;

  const calculateTotalCost = (routes: Route[]) => {
    return routes.reduce((sum, route) => sum + route.cost, 0);
  };

  const calculateTotalLoad = (routes: Route[]) => {
    return routes.reduce((sum, route) => sum + route.load, 0);
  };

  const calculateGap = (cost1: number, cost2: number) => {
    return Math.abs((cost1 - cost2) / Math.min(cost1, cost2) * 100);
  };

  return (
    <Card className="border bg-gradient-card border-border shadow-elegant">
      <CardHeader>
        <CardTitle className="flex items-center space-x-2">
          <RouteIcon className="h-5 w-5 text-primary" />
          <span>Visualização das Rotas</span>
        </CardTitle>
        <CardDescription>
          Resultados dos algoritmos aplicados ao problema VRP
        </CardDescription>
      </CardHeader>
      <CardContent>
        <Tabs defaultValue="greedy" className="w-full">
          <TabsList className="grid w-full grid-cols-2">
            <TabsTrigger value="greedy">Algoritmos Gulosos</TabsTrigger>
            <TabsTrigger value="optimization" disabled={!optimizationResults}>
              Otimização
            </TabsTrigger>
          </TabsList>

          <TabsContent value="greedy" className="space-y-6">
            <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
              {/* Nearest Neighbor */}
              <div className="space-y-4">
                <div className="flex items-center justify-between">
                  <div className="flex items-center space-x-2">
                    <MapPin className="h-4 w-4 text-route-1" />
                    <h3 className="font-semibold">Vizinho Mais Próximo</h3>
                  </div>
                  <Badge variant="outline">
                    Custo: {calculateTotalCost(greedyResults.nearestNeighbor).toFixed(2)}
                  </Badge>
                </div>
                
                <RouteGraphFlow routes={greedyResults.nearestNeighbor} />
                
                <div className="space-y-2">
                  {greedyResults.nearestNeighbor.map((route) => (
                    <div key={route.id} className="flex items-center justify-between p-2 bg-muted/50 rounded">
                      <div className="flex items-center space-x-2">
                        <Truck className={`h-4 w-4 route-color-${route.id}`} />
                        <span className="text-sm">Rota {route.id}: {route.path.join(' → ')}</span>
                      </div>
                      <div className="text-xs text-muted-foreground">
                        Carga: {route.load} | Custo: {route.cost.toFixed(2)}
                      </div>
                    </div>
                  ))}
                  <div className="pt-2 border-t border-border">
                    <div className="flex justify-between text-sm">
                      <span>Carga Total:</span>
                      <span className="font-medium">{calculateTotalLoad(greedyResults.nearestNeighbor)}</span>
                    </div>
                  </div>
                </div>
              </div>

              {/* Cheapest Insertion */}
              <div className="space-y-4">
                <div className="flex items-center justify-between">
                  <div className="flex items-center space-x-2">
                    <TrendingUp className="h-4 w-4 text-route-2" />
                    <h3 className="font-semibold">Inserção Mais Barata</h3>
                  </div>
                  <Badge variant="outline">
                    Custo: {calculateTotalCost(greedyResults.cheapestInsertion).toFixed(2)}
                  </Badge>
                </div>
                
                <RouteGraphFlow routes={greedyResults.cheapestInsertion} />
                
                <div className="space-y-2">
                  {greedyResults.cheapestInsertion.map((route) => (
                    <div key={route.id} className="flex items-center justify-between p-2 bg-muted/50 rounded">
                      <div className="flex items-center space-x-2">
                        <Truck className={`h-4 w-4 route-color-${route.id}`} />
                        <span className="text-sm">Rota {route.id}: {route.path.join(' → ')}</span>
                      </div>
                      <div className="text-xs text-muted-foreground">
                        Carga: {route.load} | Custo: {route.cost.toFixed(2)}
                      </div>
                    </div>
                  ))}
                  <div className="pt-2 border-t border-border">
                    <div className="flex justify-between text-sm">
                      <span>Carga Total:</span>
                      <span className="font-medium">{calculateTotalLoad(greedyResults.cheapestInsertion)}</span>
                    </div>
                  </div>
                </div>
              </div>
            </div>

            {/* Comparison */}
            <Card className="bg-muted/20">
              <CardContent className="pt-4">
                <div className="grid grid-cols-3 gap-4 text-center">
                  <div>
                    <p className="text-sm text-muted-foreground">Gap entre algoritmos</p>
                    <p className="text-lg font-semibold text-primary">
                      {calculateGap(
                        calculateTotalCost(greedyResults.nearestNeighbor),
                        calculateTotalCost(greedyResults.cheapestInsertion)
                      ).toFixed(2)}%
                    </p>
                  </div>
                  <div>
                    <p className="text-sm text-muted-foreground">Melhor solução</p>
                    <p className="text-lg font-semibold">
                      {calculateTotalCost(greedyResults.nearestNeighbor) < calculateTotalCost(greedyResults.cheapestInsertion) 
                        ? "Vizinho Mais Próximo" 
                        : "Inserção Mais Barata"
                      }
                    </p>
                  </div>
                  <div>
                    <p className="text-sm text-muted-foreground">Diferença de custo</p>
                    <p className="text-lg font-semibold text-accent">
                      {Math.abs(
                        calculateTotalCost(greedyResults.nearestNeighbor) - 
                        calculateTotalCost(greedyResults.cheapestInsertion)
                      ).toFixed(2)}
                    </p>
                  </div>
                </div>
              </CardContent>
            </Card>
          </TabsContent>

          <TabsContent value="optimization" className="space-y-6">
            {optimizationResults && (
              <div className="space-y-6">
                {Object.entries(optimizationResults).map(([algorithm, routes]) => {
                  if (!routes) return null;
                  
                  return (
                    <div key={algorithm} className="space-y-4">
                      <div className="flex items-center justify-between">
                        <h3 className="font-semibold capitalize">{algorithm.replace(/([A-Z])/g, ' $1').trim()}</h3>
                        <Badge variant="outline">
                          Custo: {calculateTotalCost(routes).toFixed(2)}
                        </Badge>
                      </div>
                      
                      <RouteGraphFlow routes={routes} />
                      
                      <div className="space-y-2">
                        {routes.map((route) => (
                          <div key={route.id} className="flex items-center justify-between p-2 bg-muted/50 rounded">
                            <div className="flex items-center space-x-2">
                              <Truck className={`h-4 w-4 route-color-${route.id}`} />
                              <span className="text-sm">Rota {route.id}: {route.path.join(' → ')}</span>
                            </div>
                            <div className="text-xs text-muted-foreground">
                              Carga: {route.load} | Custo: {route.cost.toFixed(2)}
                            </div>
                          </div>
                        ))}
                      </div>
                    </div>
                  );
                })}
              </div>
            )}
          </TabsContent>
        </Tabs>
      </CardContent>
    </Card>
  );
};

export default RouteVisualization;