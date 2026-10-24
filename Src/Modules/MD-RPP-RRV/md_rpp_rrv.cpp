#include "md_rpp_rrv.hpp"

Penalties getPenalties(const Instance& instance){
    Penalties p;
    p.missing_edges = instance.capacity * 10.0;
    p.battery_excess = 100.0;
    p.unnecessary_trips = instance.capacity;
    p.unvisited_required_edges = instance.capacity * 10.0;
    p.invalid_depots = instance.capacity * 10.0;
}

FO evaluate(
    vector<vector<vector<int>>> &routes,    // Vetor de rotas, por veiculo
    vector<vector<double>>& dist,           // Matriz de distancias
    const double recharge_time,             // Tempo de recarga ao final da viagem 
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
                double w = dist[v][u];
                
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
            if (trip_duration > instance.capacity + __FLT64_EPSILON__){
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
            total_recharge_time = (fo.trips_counter[ivehicle] - 1) * recharge_time;
        }
        fo.vehicle_times[ivehicle] = vehicle_total_duration + total_recharge_time;

        // Makespan: tempo maximo gasto entre todos os veiculos
        fo.makespan = max(fo.makespan, fo.vehicle_times[ivehicle]);
    }

    // Restricao 10: Todas as arestas obrigatorias devem ser visitadas
    if(!remaining_required_edges.empty()){
        fo.feasible = false;
        fo.unvisited_required_edges += 1;
    }

    return fo;
}

// Calcula a FO aplicada de punicoes em caso de violacao de restricoes 
FO fitness(
    vector<vector<vector<int>>> &routes,    // Vetor de rotas, por veiculo
    vector<vector<double>>& dist,           // Matriz de distancias
    const double recharge_time,             // Tempo de recarga ao final da viagem
    const Instance& instance,               // Valores da instancia 
    const Penalties& penalties              // Penalidades para solucoes invalidas
){
    
}