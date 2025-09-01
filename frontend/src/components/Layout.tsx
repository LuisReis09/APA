import { useState } from "react";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";
import { Route, Calculator, Info, Truck } from "lucide-react";
import ProcessingTab from "./ProcessingTab";
import ExplanationsTab from "./ExplanationsTab";

const Layout = () => {
  const [activeTab, setActiveTab] = useState("processing");

  return (
    <div className="min-h-screen bg-gradient-hero">
      {/* Header */}
      <header className="border-b border-border bg-card/50 backdrop-blur-sm sticky top-0 z-50">
        <div className="container mx-auto px-6 py-4">
          <div className="flex items-center justify-between">
            <div className="flex items-center space-x-3">
              <div className="p-2 bg-gradient-primary rounded-lg shadow-glow">
                <Truck className="h-6 w-6 text-primary-foreground" />
              </div>
              <div>
                <h1 className="text-xl font-bold text-foreground">VRP Solver</h1>
                <p className="text-sm text-muted-foreground">Vehicle Routing Problem</p>
              </div>
            </div>
            <div className="text-right">
              <p className="text-sm text-muted-foreground">UFPB - Análise e Projetos de Algoritmos</p>
            </div>
          </div>
        </div>
      </header>

      {/* Main Content */}
      <main className="container mx-auto px-6 py-8">
        <Tabs value={activeTab} onValueChange={setActiveTab} className="w-full">
          <TabsList className="grid w-full grid-cols-2 bg-card/50 backdrop-blur-sm">
            <TabsTrigger value="processing" className="flex items-center space-x-2 data-[state=active]:bg-primary data-[state=active]:text-primary-foreground">
              <Calculator className="h-4 w-4" />
              <span>Processamento</span>
            </TabsTrigger>
            <TabsTrigger value="explanations" className="flex items-center space-x-2 data-[state=active]:bg-primary data-[state=active]:text-primary-foreground">
              <Info className="h-4 w-4" />
              <span>Algoritmos</span>
            </TabsTrigger>
          </TabsList>

          <TabsContent value="processing" className="mt-6">
            <ProcessingTab />
          </TabsContent>

          <TabsContent value="explanations" className="mt-6">
            <ExplanationsTab />
          </TabsContent>
        </Tabs>
      </main>
    </div>
  );
};

export default Layout;