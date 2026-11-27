#ifndef EVALUATE_HPP
#define EVALUATE_HPP

#include "md_rpp_rrv.hpp"
#include "instance.hpp"
#include "graph.hpp"
#include "utils.hpp"

#include <vector>
#include <iostream>
#include <set>

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
        penalty_cost += unvisited_required_edges * penalties.unvisited_required_edges;
        penalty_cost += invalid_depots * penalties.invalid_depots;

        return makespan + penalty_cost;
    }
};

Penalties getPenalties(const Instance& instance);

// Calcula estritamente a FO
FO evaluate(
    const vector<vector<vector<int>>> &routes,      // Vetor de rotas, por veiculo
    Graph& graph, 
    const Instance& instance                        // Valores da instancia 
);

FO evaluateSolution2D(
    const Solution2D &solution,
    Graph& graph, 
    const Instance& instance
);

#endif