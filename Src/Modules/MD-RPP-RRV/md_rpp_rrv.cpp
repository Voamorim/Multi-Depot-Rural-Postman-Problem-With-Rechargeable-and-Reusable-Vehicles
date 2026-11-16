#include "md_rpp_rrv.hpp"

Penalties getPenalties(const Instance& instance){
    Penalties p;
    p.missing_edges = instance.capacity * 10.0;
    p.battery_excess = 100.0;
    p.unnecessary_trips = instance.capacity;
    p.unvisited_required_edges = instance.capacity * 10.0;
    p.invalid_depots = instance.capacity * 10.0;
    
    return p;
}

FO evaluate(
    const vector<vector<vector<int>>> &routes,    // Vetor de rotas, por veiculo
    Graph& graph, 
    const Instance& instance                // Valores da instancia 
){
    FO fo;
    fo.makespan = 0.0;
    fo.feasible = true;
    fo.trips_counter.assign(instance.n_vehicles, 0);
    fo.vehicle_times.assign(instance.n_vehicles, 0.0);

    // Conjunto de vertices obrigatorios ainda nao visitados
    set<pair<int, int>> remaining_required_edges;
    for (const int i_edge : instance.required_edges){
        const Edge& edge = instance.edges[i_edge];

        pair<int, int> e;

        if(!instance.directed and edge.from > edge.to){
            e = make_pair(edge.to, edge.from);
        } else {
            e = make_pair(edge.from, edge.to);
        }
            
        remaining_required_edges.insert(e);
    }

    for(int ivehicle = 0; ivehicle < instance.n_vehicles; ++ivehicle){
        const vector<vector<int>>& trips = routes[ivehicle];
        double vehicle_total_duration = 0.0;
        int previous_end_depot = -1; // Deposito onde o ultimo percurso terminou

        for(int t = 0; t < trips.size(); ++t){
            const vector<int>& trip = trips[t];
            
            if(trip.empty()) continue;

            int trip_size = trip.size();

            // Restricao 2: Nao podem existir viagens desnecessarias
            if(trip_size < 2){
                fo.feasible = false;
                fo.unnecessary_trips += 1;
                continue;
            }

            int starting_depot = trip[0];
            int end_depot = trip[trip_size - 1];
            
            // Restricao 1: Na primeira viagem, o veiculo deve partir obrigatoriamente
            //              do seu deposito de origem designado.
            if(t == 0 and starting_depot != instance.initial_depots[ivehicle]){
                fo.feasible = false;
                fo.invalid_depots += 1;
            }

            // Restricao 5: Cada viagem deve comecar e terminar em depositos
            if(instance.depots.find(starting_depot) == instance.depots.end()){
                fo.feasible = false;
                fo.invalid_depots += 1;
            }
            if(instance.depots.find(end_depot) == instance.depots.end()){
                fo.feasible = false;
                fo.invalid_depots += 1;
            }

            // Restricao 4: A viagem t deve comecar no deposito em que a viagem
            //              t-1 terminou.
            if(previous_end_depot != -1 and starting_depot != previous_end_depot){
                fo.feasible = false;
                fo.invalid_depots += 1;
            }

            // Computa a duracao da viagem
            double trip_duration = 0.0;
            for(int i = 0; i < trip_size - 1; ++i){
                int v = trip[i];
                int u = trip[i+1];
                double w = graph.getEdge(v, u);
                
                trip_duration += w;

                // Remove a aresta da lista de obrigatorias caso ela seja obrigatoria
                pair<int, int> e;
                if (!instance.directed and v > u) {
                    e = make_pair(u, v);
                } else {
                    e = make_pair(v, u);
                }
                remaining_required_edges.erase(e);
            }

            // Restricao 7: A duracao da viagem nao pode exceder a autonomia (capacity)
            if (trip_duration > instance.capacity + EPS){
                fo.feasible = false;
                fo.battery_excess += trip_duration - instance.capacity;
            }

            vehicle_total_duration += trip_duration;
            previous_end_depot = end_depot;
            fo.trips_counter[ivehicle] += 1;
        }

        // Restricao 6: Adiciona tempos de recarga entre viagens
        double total_recharge_time = 0.0;
        if(fo.trips_counter[ivehicle] > 1){
            total_recharge_time = (fo.trips_counter[ivehicle] - 1) * instance.recharge_time;
        }
        fo.vehicle_times[ivehicle] = vehicle_total_duration + total_recharge_time;

        // Makespan: tempo maximo gasto entre todos os veiculos
        fo.makespan = max(fo.makespan, fo.vehicle_times[ivehicle]);
    }

    // Restricao 10: Todas as arestas obrigatorias devem ser visitadas
    if(!remaining_required_edges.empty()){
        fo.feasible = false;
        fo.unvisited_required_edges += remaining_required_edges.size();
    }

    return fo;
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

vector<vector<Node>> convert3dSolutionTo2d(
    const vector<vector<vector<int>>> &routes,
    const Instance& instance
){
    vector<vector<Node>> solution(instance.n_vehicles);
    set<int> covered_required_edges;

    for(int vehicle = 0; vehicle < instance.n_vehicles; ++vehicle){
        const auto& vehicle_trips = routes[vehicle];

        // Se não realizou nenhuma viagem, guarda apenas o deposito inicial
        if(vehicle_trips.empty()){
            int initial_depot = instance.initial_depots[vehicle];
            Node new_node = {DEPOT, initial_depot, initial_depot, -1};
            solution[vehicle].push_back(new_node);
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
                solution[vehicle].push_back(new_node);
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
                    solution[vehicle].push_back(new_node);

                    covered_required_edges.insert(req_edge_idx);
                }
            }

            int final_depot = trip.back();
            Node new_node = {DEPOT, final_depot, final_depot, -1};
            solution[vehicle].push_back(new_node);
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