#ifndef MT_HPP
#define MT_HPP

#include "graph.hpp"
#include "instance.hpp"
#include "md_rpp_rrv.hpp"
#include "utils.hpp"
#include "evaluate.hpp"

#include <vector>
#include <tuple>
#include <algorithm>
#include <limits>
#include <set>

using namespace std;

tuple<FO, vector<vector<vector<int>>>> MT(
    Graph& graph, 
    const Instance& instance
);

#endif