import { useMemo } from "react";

interface Route {
  id: number;
  path: number[];
  cost: number;
  load: number;
}

interface RouteGraphProps {
  routes: Route[];
}

const RouteGraph = ({ routes }: RouteGraphProps) => {
  // Generate node positions based on routes
  const { nodes, edges } = useMemo(() => {
    const allNodes = new Set<number>();
    const graphEdges: Array<{ from: number; to: number; routeId: number }> = [];

    routes.forEach(route => {
      route.path.forEach(node => allNodes.add(node));
      for (let i = 0; i < route.path.length - 1; i++) {
        graphEdges.push({
          from: route.path[i],
          to: route.path[i + 1],
          routeId: route.id
        });
      }
    });

    // Generate positions in a circle for better visualization
    const nodeArray = Array.from(allNodes).sort((a, b) => a - b);
    const positions = new Map<number, { x: number; y: number }>();
    
    const centerX = 150;
    const centerY = 100;
    const radius = 60;
    
    nodeArray.forEach((node, index) => {
      if (node === 0) {
        // Depot at center
        positions.set(node, { x: centerX, y: centerY });
      } else {
        // Other nodes in circle
        const angle = (2 * Math.PI * (index - 1)) / (nodeArray.length - 1);
        positions.set(node, {
          x: centerX + radius * Math.cos(angle),
          y: centerY + radius * Math.sin(angle)
        });
      }
    });

    return {
      nodes: Array.from(allNodes).map(node => ({
        id: node,
        ...positions.get(node)!
      })),
      edges: graphEdges.map(edge => ({
        ...edge,
        fromPos: positions.get(edge.from)!,
        toPos: positions.get(edge.to)!
      }))
    };
  }, [routes]);

  const getRouteColor = (routeId: number) => {
    const colors = [
      '#22c55e', // route-1 (green)
      '#3b82f6', // route-2 (blue) 
      '#8b5cf6', // route-3 (purple)
      '#ef4444', // route-4 (red)
      '#f59e0b', // route-5 (orange)
      '#06b6d4'  // route-6 (cyan)
    ];
    return colors[(routeId - 1) % colors.length];
  };

  return (
    <div className="bg-muted/10 rounded-lg p-4">
      <svg width="300" height="200" className="w-full h-auto">
        {/* Edges (Routes) */}
        {edges.map((edge, index) => (
          <line
            key={index}
            x1={edge.fromPos.x}
            y1={edge.fromPos.y}
            x2={edge.toPos.x}
            y2={edge.toPos.y}
            stroke={getRouteColor(edge.routeId)}
            strokeWidth="2"
            strokeDasharray="3,3"
            className="route-line"
            opacity="0.8"
          />
        ))}
        
        {/* Arrow markers for direction */}
        <defs>
          {[1, 2, 3, 4, 5, 6].map(routeId => (
            <marker
              key={routeId}
              id={`arrow-${routeId}`}
              markerWidth="10"
              markerHeight="10"
              refX="9"
              refY="3"
              orient="auto"
              markerUnits="strokeWidth"
            >
              <polygon
                points="0,0 0,6 9,3"
                fill={getRouteColor(routeId)}
              />
            </marker>
          ))}
        </defs>

        {/* Nodes */}
        {nodes.map(node => (
          <g key={node.id}>
            {/* Node circle */}
            <circle
              cx={node.x}
              cy={node.y}
              r={node.id === 0 ? "8" : "6"}
              fill={node.id === 0 ? "#ffffff" : "#374151"}
              stroke={node.id === 0 ? "#22c55e" : "#ffffff"}
              strokeWidth="2"
              className="route-node"
            />
            {/* Node label */}
            <text
              x={node.x}
              y={node.y + (node.id === 0 ? 20 : 16)}
              textAnchor="middle"
              className="text-xs font-medium fill-foreground"
            >
              {node.id === 0 ? 'Depósito' : node.id}
            </text>
          </g>
        ))}
      </svg>
      
      {/* Legend */}
      <div className="flex flex-wrap gap-2 mt-3">
        {routes.map(route => (
          <div key={route.id} className="flex items-center space-x-1">
            <div 
              className="w-3 h-3 rounded border"
              style={{ backgroundColor: getRouteColor(route.id) }}
            />
            <span className="text-xs text-muted-foreground">
              Rota {route.id}
            </span>
          </div>
        ))}
      </div>
    </div>
  );
};

export default RouteGraph;
