#include "neighborhood_operators.hpp"

// Troca a posição de dois nós intermediários aleatórios dentro de uma rota
void swapNodesRoute(
    vector<Node> &trip, 
    vector<int>& req_edges_idx,
    vector<int>& depots_idx,
    mt19937& gen
){
    // Precisa de pelo menos 2 nós intermediários
    if(trip.size() < 4) return;

    // Desconsidera os depósitos de origem e de fim para esse operador
    int trip_size = trip.size() - 2;

    if(trip_size < 1) return;

    int node1_idx = getRandomInteger(1, 1 + trip_size, gen);
    int node2_idx;
    do{
        node2_idx = getRandomInteger(1, 1 + trip_size, gen);
    } while(node1_idx == node2_idx);

    // Mantém consistência de quais posições estão com depósitos e quais com
    // arestas obrigatórias
    if(trip[node1_idx].type != trip[node2_idx].type){
        int target_depot_pos, target_req_edge_pos;
        if(trip[node1_idx].type == DEPOT){
            target_depot_pos = node1_idx;
            target_req_edge_pos = node2_idx;
        } else {
            target_depot_pos = node2_idx;
            target_req_edge_pos = node1_idx;
        }
        
        int i = 0;
        while(depots_idx[i] != target_depot_pos){
            i+=1;
        }
        depots_idx[i] = target_req_edge_pos;
        
        i = 0;
        while(req_edges_idx[i] != target_req_edge_pos){
            i+=1;
        }
        depots_idx[i] = target_depot_pos;
    }

    swap(trip[node1_idx], trip[node2_idx]);
}

// Inverte o sentido de aresta obrigatória aleatória
void invertRequiredEdge(
    vector<Node> &trip, 
    const vector<int>& req_edges_idx, 
    const Instance& instance,
    mt19937& gen
){
    // Não inverte o sentido se o grafo for direcionado
    if(instance.directed or !req_edges_idx.size()) return;

    // Seleciona uma aresta obrigatória qualquer
    int i_edge = getRandomInteger(0, req_edges_idx.size()-1, gen);
    int node_pos = req_edges_idx[i_edge];

    // Troca o sentido da aresta
    swap(trip[node_pos].u, trip[node_pos].v);
}

// Troca um depósito intermediário por um depósito qualquer pertencente
// ao conjunto de depósitos
void depotSwap(
    vector<Node> &trip, 
    const vector<int> &depots_idx, 
    const Instance& instance, 
    mt19937& gen
){
    if(depots_idx.size() <= 2 or instance.depots.size() < 2) return;

    // Seleciona um depósito qualquer da rota
    int i_depot = getRandomInteger(0, depots_idx.size()-1, gen);
    int depot_node_pos = depots_idx[i_depot];

    // Troca o depósito por outro depósito qualquer
    int new_depot_idx = getRandomInteger(0, instance.depots_vec.size()-1, gen);
    int new_depot = instance.depots_vec[new_depot_idx];

    trip[depot_node_pos].v = new_depot;
    trip[depot_node_pos].u = new_depot;
}

// Remove um depósito intermediário entre duas viagens consecutivas
void tripMerge(
    vector<Node> &trip, 
    vector<int>& depots_idx,
    mt19937& gen
){
    if(depots_idx.size() <= 2) return;

    int rand_depot_idx = getRandomInteger(1, depots_idx.size() - 2, gen);
    int pos = depots_idx[rand_depot_idx];

    depots_idx.erase(depots_idx.begin() + rand_depot_idx);
    trip.erase(trip.begin() + pos);
}

// Troca o depósito final da rota pelo depósito mais próximo do penúltimo
// vertice 
void optimizeLastDepot(
    vector<Node> &trip,
    const vector<vector<double>>& dist_matrix,
    const Instance& instance
){
    if(trip.size() < 2) return;

    int prev_vertex = trip[trip.size() - 2].u;

    auto [dist, final_depot] = getNearestDepot(prev_vertex, dist_matrix, instance);

    trip.back().v = final_depot;
    trip.back().u = final_depot;
    trip.back().type = DEPOT;
    trip.back().edge_id = -1;
}