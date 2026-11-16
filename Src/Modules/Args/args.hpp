#ifndef ARGS_HPP
#define ARGS_HPP

#include <string>
#include <iostream>
#include <cstring>

using namespace std;

#define INSTANCES_PATH "../Instances/ParsedInstances/Instances/"

class Args {
    private:
        int getId(const int argc, char** argv, const string& flag, const string& full_flag);
        int getInstanceId(const int argc, char **argv) ;
        char getInstancesSet(const int argc, char **argv);
    public:
        string getInputFile(const int& argc, char **argv);
};

#endif