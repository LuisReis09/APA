import { useState, useCallback } from "react";
import { useDropzone } from "react-dropzone";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Upload, FileText, AlertCircle } from "lucide-react";
import { Alert, AlertDescription } from "@/components/ui/alert";
import { AboutProject } from "./AboutProject";

interface DataInputProps {
  onDataSubmit: (data: any) => void;
}

async function LerArquivo(file: File){
  return await file.text();
}

export const DataInput = ({ onDataSubmit }: DataInputProps) => {
  const [file, setFile] = useState<File | null>(null);
  const [optimalValue, setOptimalValue] = useState("");
  const [maxIterations, setMaxIterations] = useState("1000");
  const [maxIterationsWithoutImprovement, setMaxIterationsWithoutImprovement] = useState("100");
  const [error, setError] = useState("");

  const onDrop = useCallback((acceptedFiles: File[]) => {
    const selectedFile = acceptedFiles[0];
    (LerArquivo(selectedFile).then((res) => console.log(res)))

      setFile(selectedFile);
      if (selectedFile && selectedFile.name.endsWith('.txt')) {

        fetch("http://localhost:4000/carregarArquivo", {
          method: "POST",
          body: JSON.stringify({
            "input": LerArquivo(file).then(res => res)
          })
      })
      .then(response => {
        if (!response.ok) throw new Error("Erro ao carregar arquivo");
        return response.json();
      })
      .then(data => {
        if(!data.success){
          throw new Error(data.message);
        }
      })
      .catch(error => {
        setError(error.message);
      })
    } else {
      setError("Por favor, selecione um arquivo .txt válido");
    }
  }, []);

  const { getRootProps, getInputProps, isDragActive } = useDropzone({
    onDrop,
    accept: {
      'text/plain': ['.txt']
    },
    multiple: false
  });

  const handleSubmit = () => {
    if (!file) {
      setError("Por favor, selecione um arquivo de entrada");
      return;
    }

    const numberRegex = /^[0-9]+$/;
    if(!numberRegex.test(maxIterations) || !numberRegex.test(maxIterationsWithoutImprovement)){
      setError("Por favor, selecione valores válidos para as iterações.")
    }

    const data = {
      file,
      optimalValue: optimalValue ? parseFloat(optimalValue) : null,
      maxIterations: parseInt(maxIterations),
      maxIterationsWithoutImprovement: parseInt(maxIterationsWithoutImprovement)
    };

    fetch("http://localhost:4000/configuracoes", {
      method: "POST",
      headers: {
          "Content-Type": "application/json"
        },
      body: JSON.stringify({
        valor_otimo: data.optimalValue,
        maximo_iteracoes: data.maxIterations,
        maximo_iteracoes_sem_melhora: data.maxIterationsWithoutImprovement
      })
    })
    .then()
  };

  return (
    <div className="grid grid-cols-1 lg:grid-cols-2 gap-8">
      {/* Input Section */}
      <div className="space-y-6">
        <Card className="shadow-card">
          <CardHeader>
            <CardTitle className="flex items-center gap-2">
              <Upload className="w-5 h-5 text-primary" />
              Upload do Arquivo de Entrada
            </CardTitle>
            <CardDescription>
              Faça upload do arquivo .txt contendo os dados do problema
            </CardDescription>
          </CardHeader>
          <CardContent className="space-y-4">
            {/* File Upload Area */}
            <div
              {...getRootProps()}
              className={`border-2 border-dashed rounded-lg p-8 text-center cursor-pointer transition-all duration-300 ${
                isDragActive
                  ? "border-primary bg-primary/5"
                  : "border-muted hover:border-primary/50 hover:bg-muted/50"
              }`}
            >
              <input {...getInputProps()} />
              <div className="flex flex-col items-center gap-3">
                <div className="p-3 rounded-full bg-muted">
                  <FileText className="w-6 h-6 text-muted-foreground" />
                </div>
                {file ? (
                  <div className="text-sm">
                    <p className="font-medium text-foreground">{file.name}</p>
                    <p className="text-muted-foreground">
                      {(file.size / 1024).toFixed(2)} KB
                    </p>
                  </div>
                ) : isDragActive ? (
                  <p className="text-sm text-primary">Solte o arquivo aqui...</p>
                ) : (
                  <div className="text-sm">
                    <p className="text-foreground">Arraste um arquivo ou clique para selecionar</p>
                    <p className="text-muted-foreground">Aceita apenas arquivos .txt</p>
                  </div>
                )}
              </div>
            </div>

            {error && (
              <Alert variant="destructive">
                <AlertCircle className="h-4 w-4" />
                <AlertDescription>{error}</AlertDescription>
              </Alert>
            )}
          </CardContent>
        </Card>

        {/* Parameters Section */}
        <Card className="shadow-card">
          <CardHeader>
            <CardTitle>Parâmetros de Otimização</CardTitle>
            <CardDescription>
              Configure os parâmetros para os algoritmos de otimização
            </CardDescription>
          </CardHeader>
          <CardContent className="space-y-4">
            <div className="space-y-2">
              <Label htmlFor="optimal-value">Valor Ótimo (opcional)</Label>
              <Input
                id="optimal-value"
                type="number"
                value={optimalValue}
                onChange={(e) => setOptimalValue(e.target.value)}
                placeholder="Ex: 1000.5"
              />
            </div>

            <div className="grid grid-cols-2 gap-4">
              <div className="space-y-2">
                <Label htmlFor="max-iterations">Máx. Iterações</Label>
                <Input
                  id="max-iterations"
                  type="number"
                  value={maxIterations}
                  onChange={(e) => setMaxIterations(e.target.value)}
                />
              </div>

              <div className="space-y-2">
                <Label htmlFor="max-no-improvement">Máx. Sem Melhoria</Label>
                <Input
                  id="max-no-improvement"
                  type="number"
                  value={maxIterationsWithoutImprovement}
                  onChange={(e) => setMaxIterationsWithoutImprovement(e.target.value)}
                />
              </div>
            </div>

            <Button 
              onClick={handleSubmit} 
              className="w-full"
              variant="hero"
              size="lg"
            >
              <Upload className="w-4 h-4 mr-2" />
              Processar Dados
            </Button>
          </CardContent>
        </Card>
      </div>

      {/* About Project Section */}
      <AboutProject />
    </div>
  );
};