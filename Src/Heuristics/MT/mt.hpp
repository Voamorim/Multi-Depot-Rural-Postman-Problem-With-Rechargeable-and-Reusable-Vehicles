#ifndef MT_HPP
#define MT_HPP

#include "../../Modules/Graph/graph.hpp"
#include "../../Modules/IO/io.hpp"
#include "../../Modules/MD-RPP-RRV/md_rpp_rrv.hpp"
#include "../../Modules/Utils/utils.hpp"

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