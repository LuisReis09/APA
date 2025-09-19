import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
import { Users, GraduationCap, Calendar, MapPin, Github, Download } from "lucide-react";
import { jsPDF } from "jspdf";
import autoTable from "jspdf-autotable";

export const AboutProject = () => {
  const teamMembers = [
    { nome: "Josué Guedes", github: "JosueGuedes"},
    { nome: "Herick José", github: "Herickjf"}, 
    { nome: "Luis Reis", github: "LuisReis09"}
  ];

  const handleDownload = () => {
    fetch("http://localhost:4000/baixarRelatorio")
      .then(response => response.text())
      .then(csv_content => {
        if (csv_content) {
          const csvContent = csv_content;
          const doc = new jsPDF("p", "pt", "a4");

          // Título e autores
          doc.setFontSize(16);
          doc.text("Autores:", 40, 40);
          doc.setFontSize(12);
          doc.text("Luis Gustavo Lacerda Reis", 60, 60);
          doc.text("Josué Guedes Ferreira", 60, 80);
          doc.text("Herick José de Freitas", 60, 100);

          doc.setFontSize(14);
          doc.text("Resultados:", 40, 140);

          // CSV -> Array
          const rows = csvContent
            .trim()
            .split("\n")
            .map(row => row.split(",").map(cell => cell.replace(/"/g, "").trim()));

          const headers = ["Arquivo", "Algoritmo", "Custo Total", "Tempo (ms)", "Valor Ótimo", "Gap (%)"];

          // Tabela
          autoTable(doc, {
            startY: 160,
            head: [headers],
            body: rows.slice(1),
            styles: { fontSize: 10, cellPadding: 4 },
            headStyles: { fillColor: [22, 38, 66], textColor: 255, fontStyle: 'bold' },
            columnStyles: { 2: { halign: "right" }, 3: { halign: "right" }, 4: { halign: "right" }, 5: { halign: "right" } }
          });


          // Abrir em nova aba (sem baixar direto)
          window.open(doc.output("bloburl"), "_blank");
        }
      })
      .catch(error => {
        console.error("Erro ao baixar o relatório:", error);
      });
  }

  return (
    <div className="space-y-6">
      <Card className="shadow-card">
        <CardHeader>
          <CardTitle className="flex items-center gap-2">
            <GraduationCap className="w-5 h-5 text-primary" />
            Sobre o Projeto
          </CardTitle>
          <CardDescription>
            Informações sobre o desenvolvimento desta aplicação
          </CardDescription>
        </CardHeader>
        <CardContent className="space-y-6">
          <div className="space-y-4">
            <div className="flex items-start gap-3">
              <MapPin className="w-5 h-5 text-primary mt-0.5" />
              <div>
                <h4 className="font-semibold text-foreground">Instituição</h4>
                <p className="text-sm text-muted-foreground">
                  Universidade Federal da Paraíba (UFPB)
                </p>
              </div>
            </div>

            <div className="flex items-start gap-3">
              <GraduationCap className="w-5 h-5 text-primary mt-0.5" />
              <div>
                <h4 className="font-semibold text-foreground">Disciplina</h4>
                <p className="text-sm text-muted-foreground">
                  Análise e Projeto de Algoritmos
                </p>
              </div>
            </div>

            <div className="flex items-start gap-3">
              <Calendar className="w-5 h-5 text-primary mt-0.5" />
              <div>
                <h4 className="font-semibold text-foreground">Período</h4>
                <p className="text-sm text-muted-foreground">
                  2025.1
                </p>
              </div>
            </div>
          </div>

          <div className="pt-4 border-t">
            <h4 className="font-semibold text-foreground mb-3 flex items-center gap-2">
              <Github className="w-5 h-5 mt-0.5 text-pink-600"/>
              Equipe de Desenvolvimento
            </h4>
            <div className="flex flex-wrap gap-2 cursor-pointer">
              {teamMembers.map((member, index) => (
                <Badge key={index} variant="secondary" onClick={() => window.open("https://github.com/" + member.github, "_blank")}>
                  {member.nome}
                </Badge>
              ))}
            </div>
          </div>
        </CardContent>
      </Card>

      <Card className="shadow-card bg-purple-700 text-accent-foreground">
        <CardContent className="pt-6">
          <div className="text-center space-y-2">
            <h3 className="font-semibold">Objetivo do Projeto</h3>
            <p className="text-sm opacity-90">
              Implementar e comparar algoritmos para o Problema das Rotas de Veículos (VRP),
              analisando diferentes abordagens heurísticas e metaheurísticas para otimização.
            </p>
          </div>
        </CardContent>
      </Card>

      <Card
        onClick={handleDownload} 
        className="shadow-lg hover:shadow-xl  text-primary-foreground 
                  cursor-pointer transition-all duration-200 text-white
                  hover:bg-primary/90 active:scale-95 rounded-2xl"
      >
        <CardContent className="flex flex-col items-center justify-center gap-2 py-8">
          <div className="flex items-center justify-center gap-2">
            <Download className="w-8 h-8" />
            <h3 className="font-semibold text-lg">Baixar Resultados</h3>
          </div>
          <p className="text-sm opacity-80 text-center">
            Baixe o relatório em PDF dos resultados obtidos.
          </p>
        </CardContent>
      </Card>
    </div>
  );
};