#include "neighborhood_operators.hpp"

void swapNodesRoute(vector<Node> &trip, mt19937& gen){
    // Desconsidera os depósitos de origem e de fim para esse operador
    int trip_size = trip.size() - 2;

    // Se não percorre pelo menos 3 nós, não faz nada 
    if(trip_size < 1) return;

    int node1_idx = getRandomInteger(1, 1 + trip_size, gen);
    int node2_idx;
    do{
        node2_idx = getRandomInteger(1, 1 + trip_size, gen);
    } while(node1_idx == node2_idx);

    swap(trip[node1_idx], trip[node2_idx]);
}