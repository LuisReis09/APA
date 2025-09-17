import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
import { Route, MapPin } from "lucide-react";

interface RouteVisualizationProps {
  title: string;
  routes: Array<{
    id: number;
    nodes: number[];
    color: string;
  }>;
}

export const RouteVisualization = ({ title, routes }: RouteVisualizationProps) => {

  return (
    <Card className="shadow-card">
      <CardHeader>
        <CardTitle className="flex items-center gap-2">
          <Route className="w-5 h-5 text-primary" />
          {title}
        </CardTitle>
        <div className="flex flex-wrap gap-2 mt-2">
          {routes?.map((route, index) => (
            <Badge 
              key={index} 
              variant="outline"
              style={{ borderColor: route.color, color: route.color }}
            >
              <MapPin className="w-3 h-3 mr-1" />
              Rota {route.id}
            </Badge>
          ))}
        </div>
      </CardHeader>
      <CardContent>
        <div className="space-y-3">
          <h4 className="font-semibold text-sm">Rotas encontradas:</h4>
          <div className="space-y-2">
            {routes?.map((route, index) => (
              <div key={`${route.id}-${index}`} className="flex items-center gap-5">
                <div 
                  className="w-3 h-3 rounded-full flex-shrink-0"
                  style={{ backgroundColor: route.color }}
                />
                <span className="text-sm font-mono text-justify">
                  Rota {route.id}: {route.nodes.join(" → ")}
                </span>
              </div>
            ))}
          </div>
        </div>

        <div className="mt-4 pt-4 border-t text-sm text-muted-foreground">
          <p>• Cada cor representa uma rota diferente</p>
          <p>• As setas indicam a direção do percurso</p>
        </div>
      </CardContent>
    </Card>
  );
};