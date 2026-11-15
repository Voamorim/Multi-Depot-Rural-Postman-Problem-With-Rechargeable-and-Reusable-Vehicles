#include "io.hpp"

Instance readInstance(ifstream& input_file, double recharge_time)
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
    vector<pair<int, int>> req_edges(instance.n_required_edges);
    for(int i = 0; i < instance.n_required_edges; ++i){
        int v, u; 
        input_file >> v >> u;
        req_edges[i] = make_pair(v, u);
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
        instance.edges[i].required = false;
    }

    // Associa cada aresta obrigatoria ao seu indice
    for(int r = 0; r < instance.n_required_edges; ++r){
        auto [req_v, req_u] = req_edges[r];
        int found_idx = -1;

        for(int i = 0; i < instance.m; ++i){
            int v = instance.edges[i].from;
            int u = instance.edges[i].to;

            if(v == req_v and u == req_u){
                found_idx = i;
                break;
            }
            if(!instance.directed and v == req_u and u == req_v){
                found_idx = i;
                break;
            }
        }

        // Caso no Set C a aresta obrigatoria tenha sido especificada somente no sentido inverso
        if(found_idx == -1 and instance.directed){
            for(int i = 0; i < instance.m; ++i){
                int v = instance.edges[i].from;
                int u = instance.edges[i].to;
                if(v == req_u and u == req_v){
                    found_idx = i;
                    break;
                }
            }
        }

        if(found_idx != -1){
            instance.edges[found_idx].required = true;
            instance.required_edges.push_back(found_idx);
        } else {
            cout << "ERRO: Aresta obrigatoria (" << req_v << ", " << req_u << ") nao encontrada dentre as arestas do grafo." << endl;
        }
    }

    if(instance.n_depots > 0){
        vector<vector<int>> vehicles_per_depot(instance.n_depots);
        int p = 0;
        for(int vehicle = 0; vehicle < instance.n_vehicles; ++vehicle){
            if(p > instance.n_depots - 1){
                p = 0;
            }
            vehicles_per_depot[p].push_back(depots[p]);
            p += 1;
        }

        for(int j = 0; j < instance.n_depots; ++j){
            for(int d : vehicles_per_depot[j]){
                instance.initial_depots.push_back(d);
            }
        }
    }
 

    // IMPORTANTE: isso só vale para as instâncias do Set A 
    if(recharge_time == -1)
        instance.recharge_time = 2.0 * instance.capacity;    
    else
        instance.recharge_time = recharge_time;

    return instance;
}
