import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
import { Users, GraduationCap, Calendar, MapPin } from "lucide-react";

export const AboutProject = () => {
  const teamMembers = [
    "Josué Guedes",
    "Herick José", 
    "Luis Reis"
  ];

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
              <Users className="w-4 h-4" />
              Equipe de Desenvolvimento
            </h4>
            <div className="flex flex-wrap gap-2">
              {teamMembers.map((member, index) => (
                <Badge key={index} variant="secondary">
                  {member}
                </Badge>
              ))}
            </div>
          </div>
        </CardContent>
      </Card>

      <Card className="shadow-card bg-gradient-accent text-accent-foreground">
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
    </div>
  );
};