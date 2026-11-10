#include "../../Modules/Graph/graph.hpp"
#include "../../Modules/IO/io.hpp"
#include "mt.hpp"

#include <vector>
#include <set>
#include <algorithm>
#include <limits>
#include <string>
#include <fstream>

using namespace std;

const string instances_path = "../../../Instances/ParsedInstances/Instances/";

void debugInstance(const Instance& instance);

int main(){

    // Estes arquivos ainda nao batem com os resultados do artigo
    //string input_file_path = instances_path + "SetA/A5.txt";
    //string input_file_path = instances_path + "SetA/A6.txt";
    //string input_file_path = instances_path + "SetA/A13.txt";
    //string input_file_path = instances_path + "SetA/A20.txt";

    string input_file_path = instances_path + "SetA/A1.txt";

    // Arquivos dos Sets B e C ainda nao funcionam. Verificar o pq
    //string input_file_path = instances_path + "SetB/B3.txt";

    cout << "Caminho da instancia: " << input_file_path << endl << endl;

    ifstream instance_file(input_file_path);

    if(!instance_file.is_open()){
        cerr << "[ERRO] Nao foi possivel de abrir o arquivo de entrada da instancia" << endl;
        return 1;
    }

    Instance instance = readInstance(instance_file);
    instance_file.close();

    debugInstance(instance);

    Graph graph = Graph();
    graph.buildGraph(instance);
    graph.calcDistMatrix();

    auto [fo, routes] = MT(graph, instance);

    cout << "FO: " << fo.makespan << endl;
    cout << "Feasible? " << fo.feasible << endl;
    return 0;
}

void debugInstance(const Instance& instance){
    cout << "=-=-=-=-=-=-=- INSTANCIA =-=-=-=-=-=-=-=-" << endl;
    cout << "Directed: " << instance.directed << endl;
    cout << "n: " << instance.n << endl << endl;;
    cout << "N depots: " << instance.n_depots << endl;
    cout << "Depots:";
    for(auto &d : instance.depots) cout << ' ' << d;
    cout << endl << endl;
    cout << "N vehicles: " << instance.n_vehicles << endl;
    cout << "Capacity: " << instance.capacity << endl << endl;
    cout << "N required edges: " << instance.n_required_edges << endl;
    cout << "req_edges.size " << instance.required_edges.size() << endl;
    cout << "Required edges:";
    for(auto & e : instance.required_edges) cout << ' ' << e;
    cout << endl << endl;
    cout << "M: " << instance.m << endl;
    cout << "Edges: " << endl;
    for(auto & e : instance.edges){
        cout << '\t' << e.from << ' ' << e.to << ' ' << e.weight << endl;
    }
    cout << endl; 
    cout << "Initial depots:"; 
    for(auto i : instance.initial_depots) cout << ' ' << i;
    cout << endl;
    cout << "Recharge time: " << instance.recharge_time << endl;
    cout << endl;
}