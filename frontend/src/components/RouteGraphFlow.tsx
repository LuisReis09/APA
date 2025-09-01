import { useMemo, useCallback } from "react";
import {
  ReactFlow,
  Node,
  Edge,
  useNodesState,
  useEdgesState,
  Controls,
  Background,
  MiniMap,
  Position,
  MarkerType,
} from '@xyflow/react';
import '@xyflow/react/dist/style.css';

interface Route {
  id: number;
  path: number[];
  cost: number;
  load: number;
}

interface RouteGraphFlowProps {
  routes: Route[];
}

const RouteGraphFlow = ({ routes }: RouteGraphFlowProps) => {
  const { initialNodes, initialEdges } = useMemo(() => {
    const allNodes = new Set<number>();
    const routeEdges: Array<{ from: number; to: number; routeId: number }> = [];

    // Extrair todos os nós e arestas das rotas
    routes.forEach(route => {
      route.path.forEach(node => allNodes.add(node));
      for (let i = 0; i < route.path.length - 1; i++) {
        routeEdges.push({
          from: route.path[i],
          to: route.path[i + 1],
          routeId: route.id
        });
      }
    });

    // Gerar posições em círculo
    const nodeArray = Array.from(allNodes).sort((a, b) => a - b);
    const positions = new Map<number, { x: number; y: number }>();
    
    const centerX = 300;
    const centerY = 300;
    const radius = Math.max(200, nodeArray.length * 15); // Dynamic radius based on node count
    
    nodeArray.forEach((nodeId, index) => {
      if (nodeId === 0) {
        // Depósito no centro
        positions.set(nodeId, { x: centerX, y: centerY });
      } else {
        // Outros nós em círculo
        const angle = (2 * Math.PI * (index - 1)) / (nodeArray.length - 1);
        positions.set(nodeId, {
          x: centerX + radius * Math.cos(angle),
          y: centerY + radius * Math.sin(angle)
        });
      }
    });

    // Criar nós para React Flow
    const nodes: Node[] = nodeArray.map(nodeId => ({
      id: nodeId.toString(),
      type: 'default',
      position: positions.get(nodeId)!,
      data: { 
        label: nodeId === 0 ? 'Depósito' : nodeId.toString() 
      },
      style: {
        backgroundColor: nodeId === 0 ? 'hsl(var(--primary))' : 'hsl(var(--muted))',
        color: nodeId === 0 ? 'hsl(var(--primary-foreground))' : 'hsl(var(--foreground))',
        border: `2px solid ${nodeId === 0 ? 'hsl(var(--primary))' : 'hsl(var(--border))'}`,
        borderRadius: '50%',
        width: nodeId === 0 ? 60 : 40,
        height: nodeId === 0 ? 60 : 40,
        fontSize: '12px',
        fontWeight: 'bold'
      },
      sourcePosition: Position.Right,
      targetPosition: Position.Left,
    }));

    // Cores para as rotas
    const getRouteColor = (routeId: number) => {
      const colors = [
        '#22c55e', // verde
        '#3b82f6', // azul
        '#8b5cf6', // roxo
        '#ef4444', // vermelho
        '#f59e0b', // laranja
        '#06b6d4'  // ciano
      ];
      return colors[(routeId - 1) % colors.length];
    };

    // Criar arestas para React Flow
    const edges: Edge[] = routeEdges.map((edge, index) => ({
      id: `edge-${index}`,
      source: edge.from.toString(),
      target: edge.to.toString(),
      type: 'smoothstep',
      animated: true,
      style: {
        stroke: getRouteColor(edge.routeId),
        strokeWidth: 3,
      },
      markerEnd: {
        type: MarkerType.ArrowClosed,
        color: getRouteColor(edge.routeId),
      },
    }));

    return { initialNodes: nodes, initialEdges: edges };
  }, [routes]);

  const [nodes, , onNodesChange] = useNodesState(initialNodes);
  const [edges, , onEdgesChange] = useEdgesState(initialEdges);

  const onConnect = useCallback(() => {
    // Não permitir conexões manuais neste caso
  }, []);

  return (
    <div className="bg-muted/10 rounded-lg border border-border" style={{ height: '600px' }}>
      <ReactFlow
        nodes={nodes}
        edges={edges}
        onNodesChange={onNodesChange}
        onEdgesChange={onEdgesChange}
        onConnect={onConnect}
        fitView
        attributionPosition="bottom-right"
        nodesDraggable={true}
        nodesConnectable={false}
        elementsSelectable={true}
      >
        <Controls 
          style={{
            backgroundColor: 'hsl(var(--background))',
            border: '1px solid hsl(var(--border))',
            borderRadius: '8px',
          }}
        />
        <Background 
          gap={20} 
          size={1}
          color="hsl(var(--muted-foreground))"
        />
        <MiniMap 
          style={{
            backgroundColor: 'hsl(var(--background))',
            border: '1px solid hsl(var(--border))',
            borderRadius: '8px',
          }}
          nodeColor="hsl(var(--primary))"
          maskColor="hsl(var(--muted) / 0.8)"
        />
      </ReactFlow>
      
      {/* Legenda */}
      <div className="p-2 border-t border-border mb-6">
        <div className="flex flex-wrap gap-2">
          {routes.map(route => {
            const colors = ['#22c55e', '#3b82f6', '#8b5cf6', '#ef4444', '#f59e0b', '#06b6d4'];
            const color = colors[(route.id - 1) % colors.length];
            return (
              <div key={route.id} className="flex items-center space-x-1">
                <div 
                  className="w-3 h-3 rounded border"
                  style={{ backgroundColor: color }}
                />
                <span className="text-xs text-muted-foreground">
                  Rota {route.id}
                </span>
              </div>
            );
          })}
        </div>
      </div>
    </div>
  );
};

export default RouteGraphFlow;