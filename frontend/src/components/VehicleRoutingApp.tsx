import { useState } from "react";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Truck, Upload, BarChart3, BookOpen } from "lucide-react";
import { DataInput } from "./DataInput";
import { ProcessingPanel } from "./ProcessingPanel";
import { AlgorithmExplanations } from "./AlgorithmExplanations";
import { ThemeToggle } from "./ThemeToggle";

export const VehicleRoutingApp = () => {
  const [activeTab, setActiveTab] = useState("input");
  const [inputData, setInputData] = useState<any>(null);

  return (
    <div className="min-h-screen bg-gradient-subtle">
      {/* Header */}
      <header className="border-b bg-card shadow-card">
        <div className="container mx-auto px-6 py-4">
          <div className="flex items-center justify-between">
            <div className="flex items-center gap-3">
              <div className="p-2 rounded-lg bg-gradient-primary">
                <Truck className="w-6 h-6 text-primary-foreground" />
              </div>
              <div>
                <h1 className="text-2xl font-bold text-foreground">
                  Problema das Rotas para M Veículos
                </h1>
                <p className="text-sm text-muted-foreground">
                  Análise e Projeto de Algoritmos - UFPB
                </p>
              </div>
            </div>
            <ThemeToggle />
          </div>
        </div>
      </header>

      {/* Main Content */}
      <main className="container mx-auto px-6 py-8">
        <Tabs value={activeTab} onValueChange={setActiveTab} className="w-full">
          <TabsList className="grid w-full grid-cols-3 mb-8">
            <TabsTrigger value="input" className="flex items-center gap-2">
              <Upload className="w-4 h-4" />
              Entrada de Dados
            </TabsTrigger>
            <TabsTrigger value="processing" className="flex items-center gap-2">
              <BarChart3 className="w-4 h-4" />
              Processamento
            </TabsTrigger>
            <TabsTrigger value="explanations" className="flex items-center gap-2">
              <BookOpen className="w-4 h-4" />
              Algoritmos
            </TabsTrigger>
          </TabsList>

          <TabsContent value="input" className="space-y-6">
            <DataInput onDataSubmit={setInputData} />
          </TabsContent>

          <TabsContent value="processing" className="space-y-6">
            <ProcessingPanel inputData={inputData} />
          </TabsContent>

          <TabsContent value="explanations" className="space-y-6">
            <AlgorithmExplanations />
          </TabsContent>
        </Tabs>
      </main>
    </div>
  );
};