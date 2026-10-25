#include "mt.hpp"

// Retorna um conjunto com todas as arestas obrigatorias 
set<pair<int, int>> getRequiredEdges(const Instance& instance){
    set<pair<int, int>> remaining_required_edges;

    for(int i = 0; i < instance.n_required_edges; ++i){
        int edge_idx = instance.required_edges[i];

        const Edge& edge = instance.edges[edge_idx];

        pair<int, int> e; 
        e = make_pair(edge.from, edge.to);
        remaining_required_edges.insert(e);
    }

    return remaining_required_edges;
}

// Retorna o indice do veiculo com menor tempo gasto (livre mais cedo)
int getMinMakespanVehicle(const vector<double>& vehicle_times, const Instance& instance){
    auto min_makespan = min_element(vehicle_times.begin(), vehicle_times.end());
    int vehicle = distance(vehicle_times.begin(), min_makespan);
    return vehicle;
}

// Retorna qual o deposito mais proximo do vertice src 
pair<double, int> getNearestDepot(
    const int src, 
    const vector<vector<double>> &dist_matrix, 
    const Instance& instance
){
    double min_dist = numeric_limits<double>::max();
    int depot_idx = -1;
    
    for(const int depot : instance.depots){
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

// Retorna a aresta obrigatoria escolhida, o caminho ate o final
// dela e o custo total da operacao 
tuple<pair<int, int>, vector<int>, double> nextViableEdge(
    const int src, 
    const double used_capacity, 
    const set<pair<int, int>> remaining_required_edges, 
    Graph& graph, 
    const Instance& instance
){
    double min_cost = numeric_limits<double>::max();
    pair<int, int> chosen_edge = {-1, -1};
    vector<int> path;

    vector<vector<double>> dist_matrix = graph.getDistMatrix();

    for(auto [v, u] : remaining_required_edges){
        vector<pair<int, int>> orientations;
        orientations.push_back(make_pair(v, u));
        if(not instance.directed){
            orientations.push_back(make_pair(u, v));
        }

        for(auto [edge_from, edge_to] : orientations){
            double dist_to_edge = dist_matrix[src][edge_from];
            double edge_w = graph.getEdge(edge_from, edge_to);
            double travel_cost = dist_to_edge + edge_w;

            // Encontra o deposito mais proximo a partir do final da
            // aresta obrigatoria
            int next_depot;
            double dist_to_depot;
            tie(next_depot, dist_to_depot) = getNearestDepot(edge_to, dist_matrix, instance);

            // Testa se a rota e viavel em termos de capacidade
            if((used_capacity + travel_cost + dist_to_depot <= instance.capacity + __DBL_EPSILON__) and (travel_cost < min_cost)){
                min_cost = travel_cost;
                chosen_edge = make_pair(edge_from, edge_to);

                // Menor caminho ate a aresta + final da aresta 
                path = graph.getPath(src, edge_from);
                path.push_back(edge_to);
            }
        }
    }

    return make_tuple(chosen_edge, path, min_cost);
}

tuple<FO, vector<vector<vector<int>>>> MT(
    const double recharge_time, 
    Graph& graph, 
    const Instance& instance
){
    // Tempo gasto por cada um dos veiculos
    vector<double> vehicle_times (instance.n_vehicles, 0.0);

    // Contador de viagens por veiculo
    vector<int> trips_counter (instance.n_vehicles, 0);

    const vector<vector<double>>& dist_matrix = graph.getDistMatrix();

    // Conjunto de arestas obrigatorias restantes 
    set<pair<int, int>> remaining_required_edges = getRequiredEdges(instance);

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
        tie(required_edge, path_to_edge, path_to_edge_cost) = nextViableEdge(
            curr_node[vehicle], 
            used_capacity[vehicle], 
            remaining_required_edges, 
            graph, 
            instance
        );

        // Verifica se e possivel realizar um caminho que cubra uma 
        // aresta obrigatoria 
        if(required_edge != make_pair(-1, -1)){ // E possivel -> Percorrer aresta
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
                vehicle_times[vehicle] += recharge_time;
                used_capacity[vehicle] = 0.0;
                
                // É iniciada uma nova viagem
                routes[vehicle].push_back({curr_node[vehicle]});
            }

            remaining_required_edges.erase(required_edge);
        } else { // Nao e possivel -> Ir recarregar
          
            // Encontra o deposito mais proximo para recarregar
            int next_depot;
            double dist_to_depot;
            tie(next_depot, dist_to_depot) = getNearestDepot(curr_node[vehicle], dist_matrix, instance);

            if(used_capacity[vehicle] + dist_to_depot <= instance.capacity + __DBL_EPSILON__){
                // Obtem caminho para o deposito mais proximo 
                vector<int> path_to_depot;
                path_to_depot = graph.getPath(curr_node[vehicle], next_depot);

                // Anexa os nos do caminho ate o deposito para a rota 
                //
                // Exclui o primeiro, ja que ele ja esta incluso na rota 
                vector<int>& current_trip = routes[vehicle].back();
                for(int i = 1; i < path_to_edge.size(); ++i){
                    current_trip.push_back(path_to_edge[i]);
                }

                vehicle_times[vehicle] += dist_to_depot + recharge_time;
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
            tie(final_depot, dist_to_depot) = getNearestDepot(
                curr_node[vehicle],
                dist_matrix,
                instance
            );

            vector<int> path_to_depot = graph.getPath(curr_node[vehicle], final_depot);

            vector<int>& current_trip = routes[vehicle].back();
            for(int i = 0; i < path_to_depot.size(); ++i){
                current_trip.push_back(path_to_depot[i]);
            }
        }

        vector<vector<int>>& vehicle_routes = routes[vehicle];
        if(!vehicle_routes.empty() and vehicle_routes.back().size() <= 1){
            vehicle_routes.pop_back();
        }
    }

    FO fo;
    fo = evaluate(
        routes,
        dist_matrix,
        recharge_time,
        instance
    );

    return make_tuple(fo, routes);
}