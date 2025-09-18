import { useState, useEffect } from "react";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Button } from "@/components/ui/button";
import { Badge } from "@/components/ui/badge";
import { Progress } from "@/components/ui/progress";
import { Alert, AlertDescription } from "@/components/ui/alert";
import { 
  Play, 
  Clock, 
  TrendingUp, 
  Package, 
  Route,
  AlertCircle,
  CheckCircle,
  Loader2
} from "lucide-react";
import { RouteVisualization } from "./RouteVisualization";
import { StatisticsPanel } from "./StatisticsPanel";

interface ProcessingPanelProps {
  inputData: any;
}

let colorIndex = 0;

export const ProcessingPanel = ({ inputData }: ProcessingPanelProps) => {
  const [currentStep, setCurrentStep] = useState<string>("waiting");
  const [results, setResults] = useState<any>(null);
  const [statistics, setStatistics] = useState<any[]>([]);
  const [isProcessing, setIsProcessing] = useState(false);

  const colors = [
    "#FF5733", // vermelho
    "#33FF57", // verde
    "#3357FF", // azul
    "#F1C40F", // amarelo
    "#9B59B6", // roxo
    "#E67E22", // laranja
    "#1ABC9C", // turquesa
    "#E74C3C", // vermelho escuro
    "#2ECC71", // verde escuro
    "#3498DB", // azul escuro
    "#F39C12", // amarelo escuro
    "#8E44AD", // roxo escuro
    "#D35400", // laranja escuro
    "#16A085", // verde azulado
    "#C0392B"  // vermelho mais escuro
  ]

  async function fetchColor() {
    const color = colors[colorIndex];
    colorIndex = (colorIndex + 1) % colors.length;
    return color;
  }

  const steps = [
    { id: "greedy", name: "Buscas Gulosas", description: "Vizinho mais próximo e Inserção mais barata" },
    { id: "vnd1", name: "VND1", description: "Variable Neighborhood Descent Intra e Inter Rotas" },
    { id: "vnd2", name: "VND2", description: "Variable Neighborhood Descent" },
    { id: "ils", name: "ILS", description: "Iterated Local Search" },
  ];

  if (!inputData) {
    return (
      <div className="text-center py-12">
        <Alert>
          <AlertCircle className="h-4 w-4" />
          <AlertDescription>
            Primeiro faça upload dos dados na aba "Entrada de Dados" para começar o processamento.
          </AlertDescription>
        </Alert>
      </div>
    );
  }

  const handleProcessStep = (stepId: string) => {
    setIsProcessing(true);
    setCurrentStep(stepId);

    // Realiza um fetch para o backend 
    let rota = "";
    switch(stepId) {
      case "greedy":
        rota = "processarGulosos";
        break;
      case "vnd1":
        rota = "aplicarVND1";
        break;
      case "vnd2":
        rota = "aplicarVND2";
        break;
      case "ils":
        rota = "aplicarILS";
        break;
    }

    fetch(`http://localhost:4000/${rota}`)
      .then(res => res.json())
      .then(async data => {
        console.log("Dados recebidos do backend:", data);

        const nearestNeighborRoutes = await Promise.all(
          data.vizinho_mais_proximo.map(async (route: any, id: number) => ({
            id: id,
            nodes: route,
            color: await fetchColor()
          }))
        );
        const cheapestInsertionRoutes = await Promise.all(
          data.insercao_mais_barata.map(async (route: any, id: number) => ({
            id: id,
            nodes: route,
            color: await fetchColor()
          }))
        );

        const nearestNeighborResult = {
          totalCost: data.custo_vizinho_mais_proximo,
          executionTime: data.tempo_vizinho_mais_proximo, 
          gapIndex: data.gap_vizinho_mais_proximo
        };

        const cheapestInsertionResult = {
          totalCost: data.custo_insercao_mais_barata,
          executionTime: data.tempo_insercao_mais_barata,
          gapIndex: data.gap_insercao_mais_barata
        };

        const resultData = {
          nearestNeighborRoutes,
          cheapestInsertionRoutes,
          nearestNeighborResult,
          cheapestInsertionResult
        };

        setResults(resultData);
        setStatistics(prev => [...prev, 
          {
            algorithm: stepId,
            baseMethod: "vizinho mais próximo",
            ...nearestNeighborResult,
            timestamp: Date.now()
          },
          {
            algorithm: stepId,
            baseMethod: "inserção mais barata",
            ...cheapestInsertionResult,
            timestamp: Date.now() 
          }
        ]);
        setIsProcessing(false);
      })
      .catch(err => {
        console.error("Erro ao chamar API:", err);
        setIsProcessing(false);
      });
  };

  // const generateMockRoutes = (type: string) => {
  //   const numRoutes = Math.floor(Math.random() * 3) + 2;
  //   return Array.from({ length: numRoutes }, (_, i) => ({
  //     id: i + 1,
  //     nodes: [0, ...Array.from({ length: Math.floor(Math.random() * 5) + 2 }, () => 
  //       Math.floor(Math.random() * 10) + 1), 0],
  //     color: type === "nearest" 
  //       ? `hsl(${200 + (i * 60)}, 70%, 50%)` 
  //       : `hsl(${20 + (i * 60)}, 70%, 50%)`
  //   }));
  // };

  return (
    <div className="space-y-8">
      {/* Processing Status */}
      <Card className="shadow-card">
        <CardHeader>
          <CardTitle className="flex items-center gap-2">
            <Route className="w-5 h-5 text-primary" />
            Status do Processamento
          </CardTitle>
          <CardDescription>
            Arquivo: {inputData.file_name}
          </CardDescription>
        </CardHeader>
        <CardContent className="space-y-4">
          {isProcessing && (
            <div className="flex items-center gap-3 p-4 bg-warning/10 rounded-lg">
              <Loader2 className="w-5 h-5 animate-spin text-warning" />
              <span className="text-sm font-medium">Processando {currentStep}...</span>
            </div>
          )}
          
          <div className="grid grid-cols-2 md:grid-cols-4 gap-4">
            {steps.map((step) => (
              <Button
                key={step.id}
                variant={currentStep === step.id ? "processing" : "outline"}
                onClick={() => handleProcessStep(step.id)}
                disabled={isProcessing}
                className="h-auto p-4 flex flex-col items-start"
              >
                <div className="font-medium text-left">{step.name}</div>
                <div className="text-xs text-left opacity-70">{step.description}</div>
              </Button>
            ))}
          </div>
        </CardContent>
      </Card>

      {/* Results */}
      {results && (
        <div className="grid grid-cols-1 lg:grid-cols-3 gap-8">
          {/* Metrics */}
          <Card className="shadow-card">
            <CardHeader>
              <CardTitle className="flex items-center gap-2">
                <TrendingUp className="w-5 h-5 text-accent" />
                Métricas
              </CardTitle>
            </CardHeader>
            <CardContent className="space-y-4">
              <div className="space-y-4">
                <div>
                  <h4 className="font-semibold text-sm mb-2">Vizinho Mais Próximo</h4>
                  <div className="space-y-2 text-sm">
                    <div className="flex justify-between">
                      <span className="text-muted-foreground">Custo:</span>
                      <Badge variant="outline">{results.nearestNeighborResult.totalCost?.toFixed(2)}</Badge>
                    </div>
                    <div className="flex justify-between">
                      <span className="text-muted-foreground">Tempo:</span>
                      <Badge variant="outline">{results.nearestNeighborResult.executionTime}ms</Badge>
                    </div>
                  </div>
                </div>
                
                <div className="border-t pt-3">
                  <h4 className="font-semibold text-sm mb-2">Inserção Mais Barata</h4>
                  <div className="space-y-2 text-sm">
                    <div className="flex justify-between">
                      <span className="text-muted-foreground">Custo:</span>
                      <Badge variant="outline">{results.cheapestInsertionResult.totalCost?.toFixed(2)}</Badge>
                    </div>
                    <div className="flex justify-between">
                      <span className="text-muted-foreground">Tempo:</span>
                      <Badge variant="outline">{results.cheapestInsertionResult.executionTime}ms</Badge>
                    </div>
                  </div>
                </div>
              </div>
            </CardContent>
          </Card>

          {/* Route Visualizations */}
          <div className="lg:col-span-2 space-y-6">
            <RouteVisualization 
              title="Vizinho Mais Próximo"
              routes={results.nearestNeighborRoutes} 
            />
            <RouteVisualization 
              title="Inserção Mais Barata"
              routes={results.cheapestInsertionRoutes}
            />
          </div>
        </div>
      )}

      {/* Statistics */}
      {statistics.length > 0 && (
        <StatisticsPanel statistics={statistics} />
      )}
    </div>
  );
};