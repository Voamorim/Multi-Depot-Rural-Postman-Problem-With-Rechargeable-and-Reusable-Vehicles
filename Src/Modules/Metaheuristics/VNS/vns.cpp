#include "vns.hpp"

const int INTRA_ROUTE_NEIGHBORHOOD_OPERATORS = 5;

// Função para aplicar operador de vizinhança interno na rota
// de um veículo qualquer
void applyIntraRouteNeighborhoodOperator(
    Solution2D& sol,
    int vehicle,
    int op,
    const vector<vector<double>>& dist_matrix,
    const Instance& instance,
    mt19937& gen
);

// Busca Local: Variable Neighborhood Descent (VND)
void vnd(
    Solution2D& sol,
    FO& sol_fo,
    const int max_iterations_local_search,
    const Penalties &penalties,
    Graph& graph,
    const Instance& instance,
    mt19937& gen
);

// Perturbação
Solution2D perturbation(
    const Solution2D &curr_sol,
    const int k,
    const vector<vector<double>>& dist_matrix,
    const Instance& instance,
    mt19937& gen
);

// Altera vizinhança
void updateNeighborhood(
    Solution2D &best_sol,
    FO &best_fo,
    const Solution2D &new_sol,
    const FO& new_fo,
    const Penalties &penalties,
    int &k,
    bool &improved,
    Timer& timer
);

// VNS (Variable Neighborhood Search)
Solution2D vns(
    const Solution2D &initial_sol,
    const int VNSMax,
    const int VMax,
    const int max_iterations_local_search,
    Graph& graph,
    const Instance& instance,
    mt19937& gen,
    Timer& timer
){
    const Penalties penalties = getPenalties(instance);
    const vector<vector<double>>& dist_matrix = graph.getDistMatrix();

    Solution2D best_sol = initial_sol;
    FO best_fo = evaluateSolution2D(best_sol, graph, instance);

    // Aplica busca local na solucao inicial
    vnd(best_sol, best_fo, max_iterations_local_search, penalties, graph, instance, gen);

    int iterations_without_improvement = 0;

    while(true){
        if(iterations_without_improvement >= VNSMax){
            cout << "Criterio de parada por estagnacao acionado!" << endl;
            timer.elapsed();
            cout << endl;
            break;
        }

        bool improved = false;
        int k = 1;

        while(k <= VMax){
            // Perturbação
            Solution2D new_sol = perturbation(best_sol, k, dist_matrix, instance, gen);
            FO new_fo = evaluateSolution2D(new_sol, graph, instance);

            vnd(new_sol, new_fo, max_iterations_local_search, penalties, graph, instance, gen);

            // Altera Vizinhança
            updateNeighborhood(best_sol, best_fo, new_sol, new_fo, penalties, k, improved, timer);
        }

        if(improved){
            iterations_without_improvement = 0;
        } else {
            iterations_without_improvement += 1;
        }
    }

    return best_sol;
}

void applyIntraRouteNeighborhoodOperator(
    Solution2D& sol,
    int vehicle,
    int op,
    const vector<vector<double>>& dist_matrix,
    const Instance& instance,
    mt19937& gen
){
    vector<Node>& route = sol.vechicle_routes[vehicle];

    switch(op){
        // Inversão de aresta obrigatória
        case 1:
            invertRequiredEdge(
                route, 
                sol.req_edges_idx[vehicle],
                instance,
                gen
            );
            break;

        // Otimiza o último depósito da rota
        case 2:
            optimizeLastDepot(
                route,
                dist_matrix,
                instance
            );
            break;

        case 3:
            // Troca de depósitos intermediários
            depotSwap(
                route,
                sol.depots_idx[vehicle],
                instance,
                gen
            );
            break;
        
        case 4:
            // Troca de nós intermediários dentro de uma mesma rota
            swapNodesRoute(
                route,
                sol.req_edges_idx[vehicle],
                sol.depots_idx[vehicle],
                gen
            );
            break;
        
        case 5:
            // Remove depósito intermediário entre duas viagens
            tripMerge(
                route,
                sol.depots_idx[vehicle],
                gen
            );
            break;

        default:
            cerr << "Erro! Id de operador de vizinhanca intra route invalido!" << endl;
            break;
    }
    return;
}

// Busca Local: Variable Neighborhood Descent (VND)
void vnd(
    Solution2D& sol,
    FO& sol_fo,
    const int max_iterations_local_search,
    const Penalties &penalties,
    Graph& graph,
    const Instance& instance,
    mt19937& gen
){
    const vector<vector<double>>& dist_matrix = graph.getDistMatrix();

    double best_fit = sol_fo.getFitness(penalties);
    int intra_route_op = 1;

    while(intra_route_op <= INTRA_ROUTE_NEIGHBORHOOD_OPERATORS){
        bool improved = false;

        for(int i = 0; i < max_iterations_local_search; ++i){
            // Escolhe um veículo aleatório
            int vehicle = getRandomInteger(0, instance.n_vehicles-1, gen);

            // Pula os veíuclos ociosos
            if(sol.vechicle_routes[vehicle].size() <= 1) continue;

            Solution2D new_sol = sol;
            applyIntraRouteNeighborhoodOperator(
                new_sol,
                vehicle,
                intra_route_op,
                dist_matrix,
                instance,
                gen
            );

            FO new_fo = evaluateSolution2D(
                new_sol,
                graph, 
                instance
            );
            double new_fit = new_fo.getFitness(penalties);

            if(new_fit < best_fit - EPS){
                sol = new_sol;
                sol_fo = new_fo;
                best_fit = new_fit;
                improved = true;
                break;
            }
        }

        if(improved){
            intra_route_op = 1;
        } else {
            intra_route_op += 1;
        }
    }
    return;
}

Solution2D perturbation(
    const Solution2D &curr_sol,
    const int k,
    const vector<vector<double>>& dist_matrix,
    const Instance& instance,
    mt19937& gen
){
    Solution2D perturbed_sol = curr_sol;

    for(int i = 0; i < k; ++i){
        int vehicle = getRandomInteger(0, instance.n_vehicles-1, gen);

        // Pula veículos ociosos
        if(perturbed_sol.vechicle_routes[vehicle].size() <= -1) continue;

        int intra_route_op = getRandomInteger(1, 5, gen);
        applyIntraRouteNeighborhoodOperator(
            perturbed_sol, 
            vehicle, 
            intra_route_op, 
            dist_matrix, 
            instance,
            gen
        );
    }

    return perturbed_sol;
}

void updateNeighborhood(
    Solution2D &best_sol,
    FO &best_fo,
    const Solution2D &new_sol,
    const FO& new_fo,
    const Penalties &penalties,
    int &k,
    bool &improved,
    Timer& timer
){
    double best_fitness = best_fo.getFitness(penalties);
    double new_fitness = new_fo.getFitness(penalties);

    if(new_fitness < best_fitness - EPS){
        best_sol = new_sol;
        best_fo = new_fo;
        k = 1;
        improved = true;

        cout << "Nova melhor solucao encontrada!" << endl;
        timer.elapsed();
        cout << endl;
    } else {
        k += 1;
    }
}