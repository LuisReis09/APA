import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
import { Route, MapPin } from "lucide-react";
import {
  ReactFlow,
  Node,
  Edge,
  Background,
  Controls,
  MiniMap,
  useNodesState,
  useEdgesState,
} from '@xyflow/react';
import '@xyflow/react/dist/style.css';
import { useEffect } from "react";

interface RouteVisualizationProps {
  title: string;
  routes: Array<{
    id: number;
    nodes: number[];
    color: string;
  }>;
}

export const RouteVisualization = ({ title, routes }: RouteVisualizationProps) => {
  const [nodes, setNodes, onNodesChange] = useNodesState([]);
  const [edges, setEdges, onEdgesChange] = useEdgesState([]);

  useEffect(() => {
    if (!routes?.length) return;

    // Criar nós únicos de todas as rotas
    const allNodes = new Set<number>();
    routes.forEach(route => {
      route.nodes.forEach(nodeId => allNodes.add(nodeId));
    });

    // Posicionar nós em círculo para melhor visualização
    const nodeArray = Array.from(allNodes);
    const centerX = 300;
    const centerY = 200;
    const radius = 150;

    const flowNodes: Node[] = nodeArray.map((nodeId, index) => {
      const angle = (index * 2 * Math.PI) / nodeArray.length;
      const x = centerX + radius * Math.cos(angle);
      const y = centerY + radius * Math.sin(angle);

      return {
        id: nodeId.toString(),
        position: { x, y },
        data: { 
          label: nodeId === 0 ? "D" : `Est. ${nodeId}` 
        },
        type: nodeId === 0 ? 'depot' : 'station',
        style: {
          backgroundColor: nodeId === 0 ? 'hsl(var(--primary))' : 'hsl(var(--card))',
          color: nodeId === 0 ? 'hsl(var(--primary-foreground))' : 'hsl(var(--card-foreground))',
          border: '2px solid hsl(var(--border))',
          borderRadius: '50%',
          width: nodeId === 0 ? 60 : 40,
          height: nodeId === 0 ? 60 : 40,
          fontSize: '10px',
          fontWeight: 'bold'
        }
      };
    });

    // Criar arestas para cada rota
    const flowEdges: Edge[] = [];
    routes.forEach((route, routeIndex) => {
      for (let i = 0; i < route.nodes.length - 1; i++) {
        const sourceId = route.nodes[i].toString();
        const targetId = route.nodes[i + 1].toString();
        
        flowEdges.push({
          id: `route-${routeIndex}-${i}`,
          source: sourceId,
          target: targetId,
          type: 'smoothstep',
          style: {
            stroke: route.color,
            strokeWidth: 3,
          },
          markerEnd: {
            type: 'arrowclosed',
            color: route.color,
          },
          label: `R${route.id}`,
          labelStyle: {
            fontSize: '10px',
            fontWeight: 'bold',
            fill: route.color,
          }
        });
      }
    });

    setNodes(flowNodes);
    setEdges(flowEdges);
  }, [routes, setNodes, setEdges]);

  return (
    <Card className="shadow-card">
      <CardHeader>
        <CardTitle className="flex items-center gap-2">
          <Route className="w-5 h-5 text-primary" />
          {title}
        </CardTitle>
        <div className="flex flex-wrap gap-2 mt-2">
          {routes?.map((route) => (
            <Badge 
              key={route.id} 
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
        <div className="h-96 w-full border rounded-lg overflow-hidden">
          <ReactFlow
            nodes={nodes}
            edges={edges}
            onNodesChange={onNodesChange}
            onEdgesChange={onEdgesChange}
            fitView
            attributionPosition="bottom-left"
          >
            <Background />
            <Controls />
            <MiniMap />
          </ReactFlow>
        </div>
        
        {/* Route Text Display */}
        <div className="mt-4 space-y-3">
          <h4 className="font-semibold text-sm">Rotas encontradas:</h4>
          <div className="space-y-2">
            {routes?.map((route) => (
              <div key={route.id} className="flex items-center gap-2">
                <div 
                  className="w-3 h-3 rounded-full"
                  style={{ backgroundColor: route.color }}
                />
                <span className="text-sm font-mono">
                  Rota {route.id}: {route.nodes.join(" → ")}
                </span>
              </div>
            ))}
          </div>
        </div>

        <div className="mt-4 pt-4 border-t text-sm text-muted-foreground">
          <p>• O <strong>Depósito</strong> é representado pelo nó central maior</p>
          <p>• Cada cor representa uma rota diferente</p>
          <p>• As setas indicam a direção do percurso</p>
        </div>
      </CardContent>
    </Card>
  );
};