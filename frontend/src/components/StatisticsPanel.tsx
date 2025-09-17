import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
import { 
  Trophy, 
  TrendingDown, 
  Clock, 
  Package, 
  Target,
  BarChart3
} from "lucide-react";

interface Statistic {
  algorithm: string;
  baseMethod: string;
  totalCost: number;
  executionTime: number;
  gapIndex: number;
  timestamp: number;
}

interface StatisticsPanelProps {
  statistics: Statistic[];
}

export const StatisticsPanel = ({ statistics }: StatisticsPanelProps) => {
  // Ordenar estatísticas por custo total (melhor primeiro)
  const sortedStats = [...statistics].sort((a, b) => {
    return Number(a.totalCost) - Number(b.totalCost);
  });

  const getAlgorithmName = (algorithm: string, baseMethod: string) => {
    const names: Record<string, string> = {
      greedy: "Buscas Gulosas",
      local: "Melhoria Local", 
      vnd: "VND",
      ils: "ILS"
    };
    const algorithmName = names[algorithm] || algorithm;
    return `${algorithmName} (${baseMethod})`;
  };

  const getRankBadge = (index: number) => {
    const variants = ["default", "secondary", "outline"];
    const icons = [Trophy, TrendingDown, BarChart3];
    const Icon = icons[Math.min(index, 2)];
    
    return (
      <Badge variant={variants[Math.min(index, 2)] as any} className="flex items-center gap-1">
        <Icon className="w-3 h-3" />
        {index + 1}º
      </Badge>
    );
  };

  return (
    <Card className="shadow-card">
      <CardHeader>
        <CardTitle className="flex items-center gap-2">
          <BarChart3 className="w-5 h-5 text-accent" />
          Estatísticas Gerais
        </CardTitle>
      </CardHeader>
      <CardContent>
        <div className="space-y-4">
          {sortedStats.map((stat, index) => (
            <div 
              key={stat.timestamp}
              className="flex items-center justify-between p-4 border rounded-lg hover:bg-muted/30 transition-colors"
            >
              <div className="flex items-center gap-4">
                {getRankBadge(index)}
                <div>
                  <h4 className="font-semibold text-foreground">
                    {getAlgorithmName(stat.algorithm, stat.baseMethod)}
                  </h4>
                  <div className="flex items-center gap-4 text-sm text-muted-foreground">
                    <span className="flex items-center gap-1">
                      <Target className="w-3 h-3" />
                      Custo: {stat.totalCost.toFixed(2)}
                    </span>
                    <span className="flex items-center gap-1">
                      <Clock className="w-3 h-3" />
                      {stat.executionTime}ms
                    </span>
                  </div>
                </div>
              </div>
              
              <div className="text-right">
                <div className="text-sm font-medium text-foreground">
                  GAP: {stat.gapIndex.toFixed(2)}%
                </div>
                <div className="text-xs text-muted-foreground">
                  {new Date(stat.timestamp).toLocaleTimeString()}
                </div>
              </div>
            </div>
          ))}
          
          {statistics.length === 0 && (
            <div className="text-center py-8 text-muted-foreground">
              <BarChart3 className="w-12 h-12 mx-auto mb-3 opacity-50" />
              <p>Execute algum algoritmo para ver as estatísticas</p>
            </div>
          )}
        </div>
      </CardContent>
    </Card>
  );
};