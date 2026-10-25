#include "../../Modules/Graph/graph.hpp"
#include "../../Modules/IO/io.hpp"

#include <vector>
#include <set>
#include <algorithm>
#include <limits>
#include <string>
#include <fstream>

using namespace std;

const string instances_path = "../../../Instances/ParsedInstances/Instances/";

int main(){
    string input_file_path = instances_path + "SetA/SetA_instance_0.txt";

    ifstream instance_file(input_file_path);
    Instance instance = readInstance(instance_file);
    instance_file.close();

    

    return 0;
}