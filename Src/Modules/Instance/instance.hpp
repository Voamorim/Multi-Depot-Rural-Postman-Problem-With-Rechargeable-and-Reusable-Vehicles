#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <set>

using namespace std;

struct Edge {
    bool required;
    int from, to;
    double weight;
};

struct Instance {
    bool directed;
    int n;

    int n_depots;
    set<int> depots;
    
    int n_vehicles;
    double capacity;

    int n_required_edges;
    vector<int> required_edges; // id's das arestas obrigatorias
    
    int m;
    vector<Edge> edges;

    vector<int> initial_depots;

    double recharge_time;
};

// Por padrão, usa o tempo de recarga = 2 * capacidade
// Usa o tempo de recarga customizado se especificado
Instance readInstance(ifstream& input_file, double recharge_time=-1.0);

void debugInstance(const Instance& instance);

#endif