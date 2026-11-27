#ifndef NEIGHBORHOOD_OPERATORS_HPP
#define NEIGHBORHOOD_OPERATORS_HPP

#include <vector>
#include <algorithm>
#include <random>

#include "md_rpp_rrv.hpp"
#include "utils.hpp"

void swapNodesRoute(
    vector<Node> &trip, 
    vector<int>& req_edges_idx,
    vector<int>& depots_idx,
    mt19937& gen
);

void invertRequiredEdge(
    vector<Node> &trip, 
    const vector<int>& req_edges_idx, 
    const Instance& instance,
    mt19937& gen
);

void depotSwap(
    vector<Node> &trip, 
    const vector<int> &depots_idx, 
    const Instance& instance, 
    mt19937& gen
);

void tripMerge(
    vector<Node> &trip, 
    vector<int>& depots_idx,
    mt19937& gen
);

void optimizeLastDepot(
    vector<Node> &trip,
    const vector<vector<double>>& dist_matrix,
    const Instance& instance
);

#endif