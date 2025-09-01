import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Button } from "@/components/ui/button";
import { Target, RefreshCw, Zap } from "lucide-react";

interface AlgorithmSelectionProps {
  onSelect: (algorithm: string) => void;
}

const AlgorithmSelection = ({ onSelect }: AlgorithmSelectionProps) => {
  const algorithms = [
    {
      id: "localSearch",
      name: "Busca Local",
      description: "Melhora as soluções gulosas explorando soluções vizinhas com realocações",
      icon: <Target className="h-5 w-5" />,
      complexity: "Rápido",
      expectedImprovement: "5-15%"
    },
    {
      id: "VND",
      name: "Variable Neighborhood Descent",
      description: "Aplica diferentes estruturas de vizinhança sistematicamente",
      icon: <RefreshCw className="h-5 w-5" />,
      complexity: "Moderado",
      expectedImprovement: "10-25%"
    },
    {
      id: "ILS",
      name: "Iterated Local Search",
      description: "Combina busca local com perturbações para escapar de ótimos locais",
      icon: <Zap className="h-5 w-5" />,
      complexity: "Lento",
      expectedImprovement: "15-35%"
    }
  ];

  return (
    <Card className="bg-gradient-card border-border shadow-elegant">
      <CardHeader>
        <CardTitle>Escolha o Algoritmo de Otimização</CardTitle>
        <CardDescription>
          Selecione um algoritmo para melhorar as soluções gulosas encontradas
        </CardDescription>
      </CardHeader>
      <CardContent>
        <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
          {algorithms.map((algorithm) => (
            <Card key={algorithm.id} className="bg-muted/20 border-border hover:bg-muted/30 transition-colors">
              <CardContent className="p-4 space-y-4">
                <div className="flex items-center space-x-2">
                  <div className="p-2 bg-primary/10 rounded-lg text-primary">
                    {algorithm.icon}
                  </div>
                  <div>
                    <h3 className="font-semibold text-sm">{algorithm.name}</h3>
                    <p className="text-xs text-muted-foreground">{algorithm.complexity}</p>
                  </div>
                </div>
                
                <p className="text-sm text-muted-foreground">
                  {algorithm.description}
                </p>
                
                <div className="space-y-2">
                  <div className="flex justify-between text-xs">
                    <span className="text-muted-foreground">Melhoria esperada:</span>
                    <span className="font-medium text-primary">{algorithm.expectedImprovement}</span>
                  </div>
                </div>
                
                <Button 
                  onClick={() => onSelect(algorithm.id)}
                  className="w-full bg-gradient-primary hover:opacity-90"
                  size="sm"
                >
                  Aplicar {algorithm.name}
                </Button>
              </CardContent>
            </Card>
          ))}
        </div>
      </CardContent>
    </Card>
  );
};

export default AlgorithmSelection;