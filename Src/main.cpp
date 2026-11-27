#include "graph.hpp"
#include "instance.hpp"
#include "mt.hpp"
#include "args.hpp"
#include "evaluate.hpp"
#include "timer.hpp"
#include "build_csv.hpp"
#include "vns.hpp"

#include <vector>
#include <set>
#include <algorithm>
#include <limits>
#include <string>
#include <fstream>

using namespace std;

// Estes arquivos ainda nao batem com os resultados do artigo
//string input_file_path = instances_path + "SetA/A5.txt";
//string input_file_path = instances_path + "SetA/A6.txt";
//string input_file_path = instances_path + "SetA/A13.txt";
//string input_file_path = instances_path + "SetA/A20.txt";

// Arquivos dos Sets B e C ainda nao funcionam. Verificar o pq
//string input_file_path = instances_path + "SetB/B3.txt";

int main(int argc, char** argv){
    random_device rd;
    mt19937 gen(rd());

    string label = "Metaheuristicas para o problema do MD-RPP-RRV.";
    Timer timer(label);

    string input_file_path = Args().getInputFile(argc, argv);

    cout << "Caminho da instancia: " << input_file_path << endl << endl;

    ifstream instance_file(input_file_path);

    if(!instance_file.is_open()){
        cerr << "[ERRO] Nao foi possivel de abrir o arquivo de entrada da instancia" << endl;
        return 1;
    }

    Instance instance = readInstance(instance_file);
    instance_file.close();

    //debugInstance(instance);

    Graph graph = Graph();
    graph.buildGraph(instance);
    graph.calcDistMatrix();

    auto [fo, routes] = MT(graph, instance);

    cout << "FO: " << fo.makespan << endl;
    cout << "Feasible? " << fo.feasible << endl;

    // Converte para uma representacao mais facil de varrer a vizinhanca
    Solution2D solution2d = convert3dSolutionTo2d(routes, instance);

    cout << "OK" << endl;

    auto final_sol = vns(solution2d, 100, 100, 100, graph, instance, gen, timer);

    FO final_fo = evaluateSolution2D(final_sol, graph, instance);

    cout << endl;
    cout << "VNS FO " << final_fo.makespan << endl;
    cout << "FEASIBLE " << final_fo.feasible << endl;

    return 0;
}