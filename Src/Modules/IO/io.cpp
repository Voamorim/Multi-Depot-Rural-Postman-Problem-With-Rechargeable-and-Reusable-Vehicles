#include "io.hpp"

Instance readInstance(ifstream& input_file)
{
    Instance instance; 
  
    // Direcionado
    input_file >> instance.directed;
    // Qtd. vertices
    input_file >> instance.n;

    // Depositos
    input_file >> instance.n_depots;
    vector<int> depots;
    for(int i = 0; i < instance.n_depots; ++i){
        int depot_node;
        input_file >> depot_node;
        instance.depots.insert(depot_node);
        depots.push_back(depot_node);
    }

    // Qtd. veiculos
    input_file >> instance.n_vehicles;

    // Capacidade
    input_file >> instance.capacity;

    // Arestas Obrigatorias
    input_file >> instance.n_required_edges;
    set<pair<int, int>> req_edges;
    for(int i = 0; i < instance.n_required_edges; ++i){
        int v, u; 
        input_file >> v >> u;
        req_edges.insert(make_pair(v, u));
    }
    
    // Arestas
    input_file >> instance.m;
    instance.edges.resize(instance.m) ;
    for(int i = 0; i < instance.m; ++i){
        int v, u;
        input_file >> v;
        input_file >> u; 
        input_file >> instance.edges[i].weight;

        instance.edges[i].from = v;
        instance.edges[i].to = u;

        if(req_edges.find(make_pair(v, u)) != req_edges.end()){
            instance.edges[i].required = true;
            instance.required_edges.push_back(i);
        }
    }

    // Define os depositos iniciais de cada veiculo (atribuicao ciclica)
    instance.initial_depots.resize(instance.n_vehicles);
    for(int vehicle = 0; vehicle < instance.n_vehicles; ++vehicle){
        instance.initial_depots[vehicle] = depots[vehicle % instance.n_depots];
    }

    return instance;
}
