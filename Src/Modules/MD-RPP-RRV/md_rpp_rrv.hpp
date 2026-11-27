#ifndef MD_RPP_RRV_HPP
#define MD_RPP_RRV_HPP

#include "instance.hpp"
#include "utils.hpp"
#include "graph.hpp"

#include <vector>
#include <iostream>
#include <set>

using namespace std;

enum NodeType {
    DEPOT,
    REQ_EDGE
};

struct Node {
    NodeType type;
    int v;              // Nó de origem da aresta obrigatória (depot_idx se type==DEPOT)
    int u;              // Nó de destino da aresta obrigratória (depot_idx se type==DEPOT)
    int edge_id;        // Identificador da aresta (-1 se for deposito)
};

struct Solution2D {
    vector<vector<Node>> vechicle_routes;
    vector<vector<int>> depots_idx;         // Índices dos depósitos na rota de cada veículo
    vector<vector<int>> req_edges_idx;      // Índices das arestas obrigatórias na rota de cada veículo
};

Solution2D convert3dSolutionTo2d(
    const vector<vector<vector<int>>> &routes,
    const Instance& instance
);

vector<vector<vector<int>>> convert2dSolutionTo3d(
    const vector<vector<Node>> &routes,
    Graph& graph,
    const Instance& instance
);

// Retorna qual o deposito mais proximo do vertice src 
pair<double, int> getNearestDepot(
    const int src, 
    const vector<vector<double>> &dist_matrix, 
    const Instance& instance
);

// Retorna o indice do veiculo com menor tempo gasto (livre mais cedo)
int getMinMakespanVehicle(
    const vector<double>& vehicle_times, 
    const Instance& instance
);

#endif