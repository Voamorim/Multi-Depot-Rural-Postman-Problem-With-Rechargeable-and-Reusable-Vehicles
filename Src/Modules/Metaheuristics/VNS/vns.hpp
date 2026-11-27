#ifndef VNS_HPP
#define VNS_HPP

#include "instance.hpp"
#include "md_rpp_rrv.hpp"
#include "graph.hpp"
#include "neighborhood_operators.hpp"
#include "timer.hpp"
#include "evaluate.hpp"

#include <vector>
#include <algorithm>
#include <random>

Solution2D vns(
    const Solution2D &initial_sol,
    const int VNSMax,
    const int VMax,
    const int max_iterations_local_search,
    Graph& graph,
    const Instance& instance,
    mt19937& gen,
    Timer& timer
);

#endif