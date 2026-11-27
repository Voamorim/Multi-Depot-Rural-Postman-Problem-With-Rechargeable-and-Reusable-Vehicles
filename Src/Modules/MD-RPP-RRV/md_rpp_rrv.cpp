#include "md_rpp_rrv.hpp"

int isRequiredEdge(const pair<int, int> &edge, const Instance& instance);

Solution2D convert3dSolutionTo2d(
    const vector<vector<vector<int>>> &routes,
    const Instance& instance
){
    Solution2D solution;
    solution.vechicle_routes.resize(instance.n_vehicles);
    solution.depots_idx.resize(instance.n_vehicles);
    solution.req_edges_idx.resize(instance.n_vehicles);

    vector<vector<Node>>& vehicle_routes = solution.vechicle_routes;
    set<int> covered_required_edges;

    for(int vehicle = 0; vehicle < instance.n_vehicles; ++vehicle){
        const auto& vehicle_trips = routes[vehicle];

        // Se não realizou nenhuma viagem, guarda apenas o deposito inicial
        if(vehicle_trips.empty()){
            int initial_depot = instance.initial_depots[vehicle];
            Node new_node = {DEPOT, initial_depot, initial_depot, -1};
            vehicle_routes[vehicle].push_back(new_node);

            // Marca a posição do depósito na rota
            //solution.depots_idx[vehicle].push_back(vehicle_routes[vehicle].size()-1);
            continue;
        }

        for(int t = 0; t < vehicle_trips.size(); ++t){
            const auto &trip = vehicle_trips[t];
            if(trip.empty()) continue;
        
            // Adiciona o deposito inicial da primeira viagem
            // (somente da primeira porque o inicial das proximas é o final 
            // da viagem anterior)
            if(t == 0){
                int initial_depot = trip[0];
                Node new_node = {DEPOT, initial_depot, initial_depot, -1};
                vehicle_routes[vehicle].push_back(new_node);
                
                // Marca a posição do depósito na rota
                solution.depots_idx[vehicle].push_back(vehicle_routes[vehicle].size()-1);
            }

            // Adiciona as arestas obrigatórias à solução
            for(int i = 0; i < (long) trip.size() - 1; ++i){ // não pode esquecer do typecast :)
                pair<int, int> edge = make_pair(trip[i], trip[i+1]);

                // Verifica se é uma aresta obrigatória
                int req_edge_idx = isRequiredEdge(edge, instance);

                // Não adiciona a aresta obrigatória se ela já foi coberta antes
                //
                // Se nao repetir arestas obrigatorias, acaba melhorando as
                // solucoes, mas a conversao nao fica exata
                // - codigo nao repetindo: 
                // if(req_edge_idx != -1 && covered_required_edges.find(req_edge_idx) == covered_required_edges.end()){
                if(req_edge_idx != -1){
                    Node new_node = {REQ_EDGE, edge.first, edge.second, req_edge_idx};
                    vehicle_routes[vehicle].push_back(new_node);

                    covered_required_edges.insert(req_edge_idx);
                    
                    // Marca a posição da aresta na rota
                    solution.req_edges_idx[vehicle].push_back(vehicle_routes[vehicle].size()-1);
                }
            }

            int final_depot = trip.back();
            Node new_node = {DEPOT, final_depot, final_depot, -1};
            vehicle_routes[vehicle].push_back(new_node);
            
            // Marca a posição do depósito na rota
            solution.depots_idx[vehicle].push_back(vehicle_routes[vehicle].size()-1);
        }
    }

    return solution;
}

vector<vector<vector<int>>> convert2dSolutionTo3d(
    const vector<vector<Node>> &solution,
    Graph& graph,
    const Instance& instance
){
    vector<vector<vector<int>>> routes(instance.n_vehicles);

    for(int vehicle = 0; vehicle < instance.n_vehicles; ++vehicle){
        const auto& nodes = solution[vehicle];
        if(nodes.size() <= 1) continue;

        vector<int> curr_trip;
        curr_trip.push_back(nodes[0].v); // Deposito inicial 

        for(int node = 1; node < nodes.size(); ++node){
            const Node& curr_node = nodes[node];
            int prev_vertex = curr_trip.back();

            // Expande o caminho minimo entre os vertices
            if(prev_vertex != curr_node.v){
                vector<int> intermediate_path = graph.getPath(prev_vertex, curr_node.v);
                for(int i = 1; i < intermediate_path.size(); ++i){
                    curr_trip.push_back(intermediate_path[i]);
                }
            }

            if(curr_node.type == REQ_EDGE){
                // Adiciona o fim da aresta obrigatoria
                curr_trip.push_back(curr_node.u);
            } else {
                // Se for deposito, encerra a viagem atual
                routes[vehicle].push_back(curr_trip);

                // Se houverem mais nos depois deste deposito, inicia uma nova viagem
                if(node + 1 < nodes.size()){
                    curr_trip.clear();
                    curr_trip.push_back(curr_node.v);
                }
            }
        }
    }

    return routes;
}

int isRequiredEdge(
    const pair<int, int> &edge,
    const Instance& instance
){
    for(int required_edge_idx : instance.required_edges){
        const Edge& req_edge = instance.edges[required_edge_idx];

        if(req_edge.from == edge.first and req_edge.to == edge.second){
            return required_edge_idx;
        }

        if(!instance.directed and req_edge.from == edge.second and req_edge.to == edge.first){
            return required_edge_idx;
        }
    }

    return -1;
}

// Retorna qual o deposito mais proximo do vertice src 
pair<double, int> getNearestDepot(
    const int src, 
    const vector<vector<double>> &dist_matrix, 
    const Instance& instance
){
    double min_dist = numeric_limits<double>::max();
    int depot_idx = -1;

    for(const auto depot : instance.depots){
        double dist = dist_matrix[src][depot];

        // Pula os depositos inalcancaveis a partir do vertice atual
        if(dist == numeric_limits<double>::max()) continue;
        
        if(dist < min_dist){
            min_dist = dist;
            depot_idx = depot;
        }
    }

    return make_pair(min_dist, depot_idx);
}

// Retorna o indice do veiculo com menor tempo gasto (livre mais cedo)
int getMinMakespanVehicle(
    const vector<double>& vehicle_times, 
    const Instance& instance
){
    auto min_makespan = min_element(vehicle_times.begin(), vehicle_times.end());
    int vehicle = distance(vehicle_times.begin(), min_makespan);
    return vehicle;
}