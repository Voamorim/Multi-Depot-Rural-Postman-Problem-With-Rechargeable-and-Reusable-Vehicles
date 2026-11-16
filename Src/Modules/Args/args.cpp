#include "args.hpp"

int Args::getId(const int argc, char** argv, const string& flag, const string& full_flag){
    for(int i = 1; i < argc; i += 2){
        if(not strcmp(argv[i], flag.c_str()) or not strcmp(argv[i], full_flag.c_str())){
            return atoi(argv[i+1]);
        }
    }
    return 1;
}

int Args::getInstanceId(const int argc, char **argv){
    string instance_id_flag = "-i";
    string full_instance_id_flag = "--id";
    return getId(argc, argv, instance_id_flag, full_instance_id_flag);
}

char Args::getInstancesSet(const int argc, char **argv){
    string instance_set_flag = "-s";
    string full_instance_set_flag = "-set";

    for(int i = 1; i < argc; i += 2){
        if(not strcmp(argv[i], instance_set_flag.c_str()) or not strcmp(argv[i], full_instance_set_flag.c_str())){
            return argv[i+1][0];
        }
    }
    return 'A';
}

string Args::getInputFile(const int& argc, char **argv){
    int instance_id = getInstanceId(argc, argv);
    char instance_set = getInstancesSet(argc, argv);

    string dir_path = "Set"s + instance_set + "/"s;
    string file_name = dir_path + instance_set + to_string(instance_id) + ".txt"s;

    string instance_path = INSTANCES_PATH + file_name;
    return instance_path;
}