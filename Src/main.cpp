#include "graph.hpp"
#include "instance.hpp"
#include "mt.hpp"
#include "args.hpp"

#include <vector>
#include <set>
#include <algorithm>
#include <limits>
#include <string>
#include <fstream>

using namespace std;

int main(int argc, char** argv){

    // Estes arquivos ainda nao batem com os resultados do artigo
    //string input_file_path = instances_path + "SetA/A5.txt";
    //string input_file_path = instances_path + "SetA/A6.txt";
    //string input_file_path = instances_path + "SetA/A13.txt";
    //string input_file_path = instances_path + "SetA/A20.txt";

    string input_file_path = Args().getInputFile(argc, argv);

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

    // Converte para uma representacao mais facil de varrer a vizinhanca
    vector<vector<Node>> solution2d = convert3dSolutionTo2d(routes, instance);

    return 0;
}