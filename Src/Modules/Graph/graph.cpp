#include "graph.hpp"

Graph::Graph() : n(0) {}

Graph::Graph(const int num_vertices) : G(num_vertices, vector<double> (num_vertices, 0.0)), n(num_vertices) {}

void Graph::resetGraph() {
    for (auto& row : G) {
        fill(row.begin(), row.end(), 0.0);
    }
}

bool Graph::hasEdge(const int src, const int dest) {
    if (src <= n and dest <= n and src >= 1 and dest >= 1) {
        return G[src][dest] != 0.0;
    }
    return false;
}

double Graph::getEdge(const int src, const int dest) { return G[src][dest]; }

void Graph::setEdge(const int src, const int dest, const double value) { G[src][dest] = value; }

int Graph::getNumVertices() const { return n; }

tuple<vector<double>, vector<int>> dijkstra(Graph &G, int src){
    const int n = G.getNumVertices();

    vector<double> dist(n + 1, numeric_limits<double>::max());
    vector<bool> vis(n + 1, false);
    vector<int> par(n + 1, -1);

    priority_queue<pair<double, int>, vector<pair<double, int>> , greater<pair<double,int>>> min_heap;

    min_heap.push(make_pair(0.0, src));
    dist[src] = 0.0;
    vis[src] = true;
    par[src] = -1;

    while(!min_heap.empty()){
        auto& [d, v] = min_heap.top();
        min_heap.pop();

        if(vis[v]) continue;

        vis[v] = true;

        for(int u = 1; u <= n; ++u){
            if(u == src) continue;

            double w = G.getEdge(v, u);

            if(w == 0.0) continue;

            if(dist[v] + w < dist[u]){
                dist[u] = dist[v] + w;
                par[u] = v;
                min_heap.push(make_pair(dist[u], u));
            }
        }
    }
    return make_tuple(dist, par);
}

void Graph::calcDistMatrix(void){
    dist_matrix.assign(n + 1, vector<double>());
    par.assign(n, vector<int>());
    for(int v = 1; v <= n; ++v){
        tie(dist_matrix[v], par[v]) = dijkstra(*this, v);
    }
}

vector<vector<double>>& Graph::getDistMatrix(void){
    return dist_matrix;
}

vector<int> Graph::getPath(const int src, const int dest){
    vector<int> path;

    int curr = dest;
    while(curr != src and curr != -1){
        path.push_back(curr);
        curr = par[src][curr];
    }

    if(curr == src){
        path.push_back(curr);
    } else {
        path.clear();
    }

    return path;
}

void Graph::buildGraph(const Instance& instance){
    n = instance.n;

    G.resize(n + 1, vector<double> (n + 1, 0.0));

    for(const Edge& edge : instance.edges){
        int from = edge.from - 1;
        int to = edge.to - 1;
        double weight = edge.weight;

        setEdge(from, to, weight);
        if(!instance.directed){
            setEdge(to, from, weight);
        }
    }
}