#ifndef MD_RPP_RRV_HPP
#define MD_RPP_RRV_HPP

#include "../IO/io.hpp"

#include <vector>
#include <iostream>
#include <set>
#include <algorithm>

using namespace std;

struct Penalties {
    double missing_edges;
    double battery_excess;
    double unnecessary_trips;
    double unvisited_required_edges;
    double invalid_depots;
};

struct FO {
    double makespan;
    vector<double> vehicle_times;
    vector<int> trips_counter;
    bool feasible;

    int missing_edges;
    double battery_excess;
    int unnecessary_trips;
    int unvisited_required_edges;
    int invalid_depots;

    double getFitness(const Penalties& penalties) const {
        if(feasible) return makespan;

        double penalty_cost = 0.0;
        penalty_cost += missing_edges * penalties.missing_edges;
        penalty_cost += battery_excess * penalties.battery_excess;
        penalty_cost += unnecessary_trips * penalties.unnecessary_trips;
        penalty_cost += invalid_depots * penalties.invalid_depots;

        return makespan + penalty_cost;
    }
};

Penalties getPenalties(const Instance& instance);

// Calcula estritamente a FO
FO evaluate(
    const vector<vector<vector<int>>> &routes,    // Vetor de rotas, por veiculo
    const vector<vector<double>>& dist,           // Matriz de distancias
    const double recharge_time,             // Tempo de recarga ao final da viagem 
    const Instance& instance                // Valores da instancia 
);

#endif