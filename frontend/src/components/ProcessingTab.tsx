import { useState } from "react";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
import { Upload, Play, Loader2, Users, GraduationCap, Calendar, MapPin } from "lucide-react";
import { useToast } from "@/hooks/use-toast";
import FileUpload from "./FileUpload";
import RouteVisualization from "./RouteVisualization";
import AlgorithmSelection from "./AlgorithmSelection";
import Statistics from "./Statistics";

interface ProcessingStage {
  stage: 'upload' | 'greedy' | 'optimization' | 'complete';
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

interface Route {
  id: number;
  path: number[];
  cost: number;
  load: number;
}

const ProcessingTab = () => {
  const [file, setFile] = useState<File | null>(null);
  const [processing, setProcessing] = useState(false);
  const [stage, setStage] = useState<ProcessingStage>({ stage: 'upload' });
  const { toast } = useToast();

  const handleFileUpload = (uploadedFile: any) => {
    fetch("http://localhost:4000/carregarArquivo", {
      method: "POST",
      body: uploadedFile,
    }).then(response => {
      if (!response.ok) {
        console.error("Erro ao enviar o arquivo");
      }
      return response.json();
    }).then(data => {
      if (data.success) {
        setFile(uploadedFile);
        toast({
          title: "Arquivo enviado",
          description: data.message || "Arquivo carregado com sucesso!",
        });
      } else {
        console.error("Erro no processamento do arquivo:", data.message);
        toast({
          title: "Erro no processamento",
          description: data.message || "Ocorreu um erro ao processar o arquivo.",
          variant: "destructive"
        });
      }
    }).catch(error => {
      console.error("Erro na requisição:", error);
      toast({
        title: "Erro na requisição",
        description: "Não foi possível conectar ao servidor.",
        variant: "destructive"
      });
    });
  };

  const processFile = async () => {
    if (!file) return;
    
    setProcessing(true);
    toast({
      title: "Processando...",
      description: "Executando algoritmos gulosos (Vizinho mais próximo e Inserção mais barata)",
    });

    // fetch("http://localhost:4000/processarGulosos")
    //   .then(response => {
    //     if (!response.ok) {
    //       console.error("Erro ao processar os algoritmos gulosos");
    //     }
    //     return response.json();
    //   })
    //   .then(data => {
    //     if (data.success) {
    //       setStage({ 
    //         stage: 'greedy', 
    //         greedyResults: {nearestNeighbor: data.vizinho_mais_proximo, cheapestInsertion: data.insercao_mais_barata} 
    //       });
    //       setProcessing(false);
          
    //       toast({
    //         title: "Processamento concluído",
    //         description: "Algoritmos gulosos executados com sucesso!",
    //       });
    //     } else {
    //       console.error("Erro no processamento dos algoritmos gulosos:", data.message);
    //       toast({
    //         title: "Erro no processamento",
    //         description: data.message || "Ocorreu um erro ao processar os algoritmos gulosos.",
    //         variant: "destructive"
    //       });
    //       setProcessing(false);
    //     }
    //   })
    //   .catch(error => {
    //     console.error("Erro na requisição:", error);
    //     toast({
    //       title: "Erro na requisição",
    //       description: "Não foi possível conectar ao servidor.",
    //       variant: "destructive"
    //     });
    //     setProcessing(false);
    //   });

    setTimeout(() => {
      const mockResults = {
        nearestNeighbor: [
          { id: 1, path: [0, 1, 3, 5, 0], cost: 120.5, load: 45 },
          { id: 2, path: [0, 2, 4, 6, 0], cost: 98.2, load: 38 }
        ],
        cheapestInsertion: [
          { id: 1, path: [0, 1, 2, 4, 0], cost: 115.8, load: 42 },
          { id: 2, path: [0, 3, 5, 6, 0], cost: 102.3, load: 41 }
        ]
      };

      setStage({ 
        stage: 'greedy', 
        greedyResults: mockResults 
      });
      setProcessing(false);
      
      toast({
        title: "Processamento concluído",
        description: "Algoritmos gulosos executados com sucesso!",
      });
    }, 3000);
  };

  const selectOptimization = (algorithm: string) => {
    setProcessing(true);
    toast({
      title: "Executando otimização",
      description: `Aplicando ${algorithm}...`,
    });

    setTimeout(() => {
      const mockOptimization = {
        [algorithm]: [
          { id: 1, path: [0, 1, 3, 2, 9, 11, 12, 16, 18, 20, 21, 22, 0], cost: 108.5, load: 43 },
          { id: 2, path: [0, 4, 6, 5, 0], cost: 95.1, load: 40 }
        ]
      };

      setStage(prev => ({
        ...prev,
        stage: 'optimization',
        optimizationResults: { ...prev.optimizationResults, ...mockOptimization }
      }));
      setProcessing(false);
      
      toast({
        title: "Otimização concluída",
        description: `${algorithm} executado com sucesso!`,
      });
    }, 2500);
  };

  return (
    <div className="space-y-6">
      {/* About Section */}
      <Card className="bg-gradient-card border-border shadow-elegant">
        <CardHeader>
          <CardTitle className="flex items-center space-x-2">
            <GraduationCap className="h-5 w-5 text-primary" />
            <span>Sobre o Projeto</span>
          </CardTitle>
          <CardDescription>
            Trabalho desenvolvido para a disciplina de Análise e Projetos de Algoritmos
          </CardDescription>
        </CardHeader>
        <CardContent>
          <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
            <div className="space-y-2">
              <div className="flex items-center space-x-2">
                <Users className="h-4 w-4 text-muted-foreground" />
                <span className="text-sm">Grupo: Josué Guedes, Herick José e Luis Reis</span>
              </div>
              <div className="flex items-center space-x-2">
                <MapPin className="h-4 w-4 text-muted-foreground" />
                <span className="text-sm">Universidade Federal da Paraíba (UFPB)</span>
              </div>
            </div>
            <div className="space-y-2">
              <div className="flex items-center space-x-2">
                <Calendar className="h-4 w-4 text-muted-foreground" />
                <span className="text-sm">Período: [2025.1]</span>
              </div>
              <Badge variant="secondary" className="w-fit">
                Análise e Projetos de Algoritmos
              </Badge>
            </div>
          </div>
        </CardContent>
      </Card>

      {/* File Upload */}
      <Card className="bg-gradient-card border-border shadow-elegant">
        <CardHeader>
          <CardTitle className="flex items-center space-x-2">
            <Upload className="h-5 w-5 text-primary" />
            <span>Upload do Arquivo de Entrada</span>
          </CardTitle>
          <CardDescription>
            Faça upload do arquivo .txt contendo os dados do problema VRP
          </CardDescription>
        </CardHeader>
        <CardContent>
          <div className="space-y-4">
            <FileUpload onFileUpload={handleFileUpload} />
            {file && (
              <div className="flex items-center justify-between p-4 bg-muted rounded-lg">
                <span className="text-sm">{file.name}</span>
                <Button 
                  onClick={processFile} 
                  disabled={processing}
                  className="bg-gradient-primary hover:opacity-90 shadow-glow"
                >
                  {processing ? (
                    <>
                      <Loader2 className="h-4 w-4 mr-2 animate-spin" />
                      Processando...
                    </>
                  ) : (
                    <>
                      <Play className="h-4 w-4 mr-2" />
                      Processar
                    </>
                  )}
                </Button>
              </div>
            )}
          </div>
        </CardContent>
      </Card>

      {/* Results */}
      {stage.stage !== 'upload' && (
        <>
          <RouteVisualization 
            greedyResults={stage.greedyResults}
            optimizationResults={stage.optimizationResults}
            isProcessing={processing}
          />
          
          { !processing && (
            <AlgorithmSelection onSelect={selectOptimization} />
          )}
          
          <Statistics 
            greedyResults={stage.greedyResults}
            optimizationResults={stage.optimizationResults}
          />
        </>
      )}
    </div>
  );
};

export default ProcessingTab;