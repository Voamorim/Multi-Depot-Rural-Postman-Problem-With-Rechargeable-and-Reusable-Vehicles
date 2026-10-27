#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "../IO/io.hpp"

#include <vector>
#include <iostream>
#include <limits>
#include <algorithm>
#include <queue>
#include <tuple>

using namespace std;

class Graph {
    private:
        vector<vector<double>> G;
        int n;

        vector<vector<double>> dist_matrix;
        vector<vector<int>> par;
    public:
        void initGraph(const int num_vertices);
        void resetGraph();
       
        bool hasEdge(const int src, const int dest);        
        double getEdge(const int src, const int dest);
        void setEdge(const int src, const int dest, const double value);

        int getNumVertices() const;

        void calcDistMatrix(void);
        vector<vector<double>>& getDistMatrix(void);
        vector<int> getPath(const int src, const int dest);

        void buildGraph(const Instance& instance);

        Graph();
};

#endif