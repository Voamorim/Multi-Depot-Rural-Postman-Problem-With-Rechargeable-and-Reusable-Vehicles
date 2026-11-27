#include "mt.hpp"

// Retorna um conjunto com todas as arestas obrigatorias 
vector<pair<int, int>> getRequiredEdges(const Instance& instance);

// Retorna o indice do veiculo com menor tempo gasto (livre mais cedo)
int getMinMakespanVehicle(const vector<double>& vehicle_times, const Instance& instance);

// Retorna a aresta obrigatoria escolhida, o caminho ate o final
// dela, o custo total da operacao e o índice da aresta no vetor
tuple<pair<int, int>, vector<int>, double, int> nextViableEdge(
    const int src, 
    const double used_capacity, 
    const vector<pair<int, int>>& remaining_required_edges, 
    const vector<vector<double>>& dist_matrix,
    vector<double>& cost_to_edges, 
    Graph& graph, 
    const Instance& instance
);

// Retorna o depósito que se aproxima mais de uma aresta obrigatória (não permite retornar
// o depósito atual)
pair<double, int> getClosestFeasibleDepot(
    const int curr_node,
    const double used_capacity,
    const int nearest_edge_idx, 
    const vector<pair<int, int>>& remaining_required_edges,
    const vector<vector<double>>& dist_matrix,
    Graph& graph,
    const Instance& instance
);

tuple<FO, vector<vector<vector<int>>>> MT(
    Graph& graph, 
    const Instance& instance
){
    // Tempo gasto por cada um dos veiculos
    vector<double> vehicle_times (instance.n_vehicles, 0.0);

    // Contador de viagens por veiculo
    vector<int> trips_counter (instance.n_vehicles, 0);

    const vector<vector<double>>& dist_matrix = graph.getDistMatrix();

    // Conjunto de arestas obrigatorias restantes 
    vector<pair<int, int>> remaining_required_edges = getRequiredEdges(instance);

    // No atual de cada um dos veiculos
    //
    // Inicializado com o deposito inicial de cada veiculo
    vector<int> curr_node = instance.initial_depots; 

    // Bateria utilizada por cada um dos veiculos antes do final da
    // viagem
    vector<double> used_capacity (instance.n_vehicles, 0.0);

    // Inicializa rotas com vertices iniciais
    //
    // routes[veiculo][viagem][no]
    vector<vector<vector<int>>> routes (instance.n_vehicles);
    for(int vehicle = 0; vehicle < instance.n_vehicles; ++vehicle){
        // Rota com apenas a cidade inicial
        vector<int> initial_trip (1, curr_node[vehicle]);
        routes[vehicle].push_back(initial_trip);
    }

    while(!remaining_required_edges.empty()){
        // Identifica o veiculo que fica livre mais cedo
        int vehicle = getMinMakespanVehicle(vehicle_times, instance);

        // Verifica se todos os veiculos "morreram" no caminho
        if(vehicle_times[vehicle] == numeric_limits<double>::max()){
            break; // Solucao inviável
        }
        
        // Busca a aresta obrigatoria mais proxima do veiculo atual
        // (e que seja viavel)
        pair<int, int> required_edge;
        vector<int> path_to_edge;
        double path_to_edge_cost;
        int chosen_edge_idx = -1;

        vector<double> cost_to_edges(remaining_required_edges.size(), numeric_limits<double>::max());

        tie(required_edge, path_to_edge, path_to_edge_cost, chosen_edge_idx) = nextViableEdge(
            curr_node[vehicle], 
            used_capacity[vehicle], 
            remaining_required_edges, 
            dist_matrix,
            cost_to_edges,
            graph, 
            instance
        );

        // Verifica se e possivel realizar um caminho que cubra uma 
        // aresta obrigatoria 
        if(chosen_edge_idx != -1){ // E possivel -> Percorrer aresta
            vector<int> &current_trip = routes[vehicle].back();
            
            // Adiciona o caminho ate o final da aresta
            //
            // Pula o no inicial, porque ele ja esta incluso no caminho
            for(int i = 1; i < path_to_edge.size(); ++i){
                current_trip.push_back(path_to_edge[i]);
            }
            
            used_capacity[vehicle] += path_to_edge_cost;
            vehicle_times[vehicle] += path_to_edge_cost;
            curr_node[vehicle] = path_to_edge.back();

            // Se o final da aresta for um deposito, recarrega e encerra a viagem
            if(instance.depots.find(curr_node[vehicle]) != instance.depots.end()){
                vehicle_times[vehicle] += instance.recharge_time;
                used_capacity[vehicle] = 0.0;
                
                // É iniciada uma nova viagem
                routes[vehicle].push_back({curr_node[vehicle]});
            }
            
            // Remove a aresta obrigatoria que acabou de ser coberta
            remaining_required_edges.erase(remaining_required_edges.begin() + chosen_edge_idx);

        } else { // Nao e possivel -> Ir recarregar
            auto min_cost_it = min_element(cost_to_edges.begin(), cost_to_edges.end());
            int nearest_edge_idx = distance(cost_to_edges.begin(), min_cost_it);

            // Encontra o melhor deposito viavel para recarregar e se aproximar da aresta
            int next_depot;
            double dist_to_depot;
            tie(dist_to_depot, next_depot) = getClosestFeasibleDepot(
                curr_node[vehicle],
                used_capacity[vehicle],
                nearest_edge_idx,
                remaining_required_edges,
                dist_matrix,
                graph,
                instance
            );

            if(next_depot != -1 and (used_capacity[vehicle] + dist_to_depot <= instance.capacity + EPS)){
                // Obtem caminho para o deposito mais proximo 
                vector<int> path_to_depot;
                path_to_depot = graph.getPath(curr_node[vehicle], next_depot);

                // Anexa os nos do caminho ate o deposito para a rota 
                //
                // Exclui o primeiro, ja que ele ja esta incluso na rota 
                vector<int>& current_trip = routes[vehicle].back();
                for(int i = 1; i < path_to_depot.size(); ++i){
                    current_trip.push_back(path_to_depot[i]);
                }

                vehicle_times[vehicle] += dist_to_depot + instance.recharge_time;
                curr_node[vehicle] = next_depot;
                used_capacity[vehicle] = 0.0;

                // É iniciada uma nova viagem
                routes[vehicle].push_back({curr_node[vehicle]});
            } else {
                // Veículo ficou preso no meio do caminho
                vehicle_times[vehicle] = numeric_limits<double>::max();
            }
        }
    }

    // Todos os veiculos retornam a um deposito no final
    for(int vehicle = 0; vehicle < instance.n_vehicles; ++vehicle){
        if(vehicle_times[vehicle] == numeric_limits<double>::max()) continue;

        if(instance.depots.find(curr_node[vehicle]) == instance.depots.end()){
            int final_depot;
            double dist_to_depot;
            tie(dist_to_depot, final_depot) = getNearestDepot(
                curr_node[vehicle],
                dist_matrix,
                instance
            );

            if(used_capacity[vehicle] + dist_to_depot <= instance.capacity + EPS){
                vector<int> path_to_depot = graph.getPath(curr_node[vehicle], final_depot);

                vector<int>& current_trip = routes[vehicle].back();
                for(int i = 1; i < path_to_depot.size(); ++i){
                    current_trip.push_back(path_to_depot[i]);
                }
                curr_node[vehicle] = final_depot;
                vehicle_times[vehicle] += dist_to_depot;
            } else {
                // Veículo nao consegue voltar para deposito algum 
                vehicle_times[vehicle] = numeric_limits<double>::max();
            }
        }

        vector<vector<int>>& vehicle_routes = routes[vehicle];
        if(!vehicle_routes.empty() and vehicle_routes.back().size() <= 1){
            vehicle_routes.pop_back();
            if(!vehicle_routes.empty()){
                vehicle_times[vehicle] -= instance.recharge_time;
            }
        }
    }

    FO fo;
    fo = evaluate(
        routes,
        graph,
        instance
    );

    return make_tuple(fo, routes);
}

// Retorna um conjunto com todas as arestas obrigatorias 
vector<pair<int, int>> getRequiredEdges(
    const Instance& instance
){
    vector<pair<int, int>> remaining_required_edges;

    for(int i = 0; i < instance.n_required_edges; ++i){
        int edge_idx = instance.required_edges[i];

        const Edge& edge = instance.edges[edge_idx];

        pair<int, int> e = make_pair(edge.from, edge.to);
        remaining_required_edges.push_back(e);
    }

    return remaining_required_edges;
}

// Retorna a aresta obrigatoria escolhida, o caminho ate o final
// dela, o custo total da operacao e o índice da aresta no vetor
tuple<pair<int, int>, vector<int>, double, int> nextViableEdge(
    const int src, 
    const double used_capacity, 
    const vector<pair<int, int>>& remaining_required_edges, 
    const vector<vector<double>>& dist_matrix,
    vector<double>& cost_to_edges, 
    Graph& graph, 
    const Instance& instance
){
    double min_cost = numeric_limits<double>::max();
    pair<int, int> chosen_edge = make_pair(-1, -1);
    vector<int> path;
    int chosen_idx = -1;

    double remaining_capacity = instance.capacity - used_capacity;

    for(int e = 0; e < remaining_required_edges.size(); ++e){
        auto [v, u] = remaining_required_edges[e];

        double dv = dist_matrix[src][v];
        double du = dist_matrix[src][u];
        double w = graph.getEdge(v, u);

        double curr_min_cost = numeric_limits<double>::max();
        int from_node = -1, to_node = -1;

        if(src == v){ // Veículo já está no nó de origem da aresta
            curr_min_cost = w;
            from_node = v, to_node = u;
        } else if (src == u and !instance.directed){  // O veículo já está no outro nó da aresta (não direcionado)
            curr_min_cost = w;
            from_node = u, to_node = v;
        } else { // Veículo está em um outro nó qualquer
            double cost_v = numeric_limits<double>::max();
            double cost_u = numeric_limits<double>::max();

            if(dv != numeric_limits<double>::max()) 
                cost_v = dv + w;
            if(!instance.directed and du != numeric_limits<double>::max()) 
                cost_u = du + w;
           
            if(cost_v <= cost_u and cost_v != numeric_limits<double>::max()){
                curr_min_cost = cost_v;
                from_node = v, to_node = u;
            } else if (cost_u < cost_v){
                curr_min_cost = cost_u;
                from_node = u, to_node = v;
            }
        }

        cost_to_edges[e] = curr_min_cost;

        if(curr_min_cost <= remaining_capacity + EPS) {
            int next_depot;
            double dist_to_depot;
            tie(dist_to_depot, next_depot) = getNearestDepot(to_node, dist_matrix, instance);

            if(next_depot != -1 and (curr_min_cost + dist_to_depot <= remaining_capacity + EPS)){
                if(curr_min_cost < min_cost){
                    min_cost = curr_min_cost;
                    chosen_edge = make_pair(from_node, to_node);
                    chosen_idx = e;

                    if(src != from_node){
                        path = graph.getPath(src, from_node);
                        path.push_back(to_node);
                    } else {
                        path = {src, to_node};
                    }
                }
            }
        }
    }

    return make_tuple(chosen_edge, path, min_cost, chosen_idx);
}

pair<double, int> getClosestFeasibleDepot(
    const int curr_node,
    const double used_capacity,
    const int nearest_edge_idx, 
    const vector<pair<int, int>>& remaining_required_edges,
    const vector<vector<double>>& dist_matrix,
    Graph& graph,
    const Instance& instance
){
    int closest_depot = -1;
    double min_total_dist_via_depot = numeric_limits<double>::max();
    double chosen_dist_to_depot = numeric_limits<double>::max(); 
    double remaining_capacity = instance.capacity - used_capacity;

    auto [v, u] = remaining_required_edges[nearest_edge_idx];
    double edge_w = graph.getEdge(v, u);

    for(int depot : instance.depots){
        // Se o veiculo ja esta neste deposito, nao pode escolher ele
        if(depot == curr_node) continue;

        double curr_to_depot = dist_matrix[curr_node][depot];
        if(curr_to_depot > remaining_capacity + EPS or 
           curr_to_depot == numeric_limits<double>::max()){
           continue;
        }
       
        // Calcula o custo de sair de depot e cobrir a aresta mais proxima
        double dist_v = dist_matrix[depot][v];
        double dist_u = numeric_limits<double>::max();
        if(!instance.directed)
            dist_u = dist_matrix[depot][u];
        
        double depot_to_edge_cost = numeric_limits<double>::max(); 
       
        if(depot == v){
            depot_to_edge_cost = edge_w;
        } else if (depot == u and !instance.directed){
            depot_to_edge_cost = edge_w;
        } else {
            double cost_v = numeric_limits<double>::max();
            double cost_u = numeric_limits<double>::max();

            if(dist_v != numeric_limits<double>::max()){
                cost_v = dist_v + edge_w;
            }
            if(!instance.directed and dist_u != numeric_limits<double>::max()){
                cost_u = dist_u + edge_w;
            }

            depot_to_edge_cost = min(cost_v, cost_u);
        }

        if(depot_to_edge_cost == numeric_limits<double>::max()) continue;

        // Ir ate o deposito + ir do deposito ate o final da aresta
        double total_cost_via_depot = curr_to_depot + depot_to_edge_cost;

        if(total_cost_via_depot < min_total_dist_via_depot){
            min_total_dist_via_depot = total_cost_via_depot;
            closest_depot = depot;
            chosen_dist_to_depot = curr_to_depot;
        }
    }

    return make_pair(chosen_dist_to_depot, closest_depot);
}