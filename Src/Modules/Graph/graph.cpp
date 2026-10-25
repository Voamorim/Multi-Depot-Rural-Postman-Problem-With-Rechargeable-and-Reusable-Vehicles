#include "graph.hpp"

Graph::Graph() : n(0) {}

Graph::Graph(const int num_vertices) : G(num_vertices, vector<double> (num_vertices, 0.0)), n(num_vertices) {}

void Graph::initGraph(const int num_vertices){
    n = num_vertices;
    G.resize(n, vector<double>(n, 0.0));
}

void Graph::resetGraph() {
    for (auto& row : G) {
        fill(row.begin(), row.end(), 0.0);
    }
}

bool Graph::hasEdge(const int src, const int dest) {
    if (src < n and dest < n and src >= 0 and dest >= 0) {
        return G[src][dest] != 0.0;
    }
    return false;
}

double Graph::getEdge(const int src, const int dest) { return G[src][dest]; }

void Graph::setEdge(const int src, const int dest, const double value) { G[src][dest] = value; }

int Graph::getNumVertices() const { return n; }

tuple<vector<double>, vector<int>> dijkstra(Graph &G, int src){
    const int n = G.getNumVertices();

    vector<double> dist(n, numeric_limits<double>::max());
    vector<bool> vis(n, false);
    vector<int> par(n, -1);

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

        for(int u = 0; u < n; ++u){
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
    dist_matrix.assign(n, vector<double>());
    par.assign(n, vector<int>());
    for(int v = 0; v < n; ++v){
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