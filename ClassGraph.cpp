
#include <bits/stdc++.h>
using namespace std;

const int INF = 1e9;

// ================= DANH SÁCH KỀ =================
class GraphList {
public:
    int V;
    vector<vector<pair<int,int>>> adj;

    GraphList(int v) {
        V = v;
        adj.resize(V);
    }

    void addEdge(int u, int v, int w) {
        adj[u].push_back({v, w});
        // nếu vô hướng thì thêm dòng dưới
        // adj[v].push_back({u, w});
    }

    void dijkstra(int src) {
        vector<int> dist(V, INF);
        priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq;

        dist[src] = 0;
        pq.push({0, src});

        while (!pq.empty()) {
            int u = pq.top().second;
            pq.pop();

            for (auto x : adj[u]) {
                int v = x.first;
                int w = x.second;

                if (dist[v] > dist[u] + w) {
                    dist[v] = dist[u] + w;
                    pq.push({dist[v], v});
                }
            }
        }

        cout << "Dijkstra (Adj List):\n";
        for (int i = 0; i < V; i++)
            cout << "Node " << i << " = " << dist[i] << endl;
    }
};

// ================= MA TRẬN KỀ =================
class GraphMatrix {
public:
    int V;
    vector<vector<int>> matrix;

    GraphMatrix(int v) {
        V = v;
        matrix.assign(V, vector<int>(V, INF));
    }

    void addEdge(int u, int v, int w) {
        matrix[u][v] = w;
        // nếu vô hướng:
        // matrix[v][u] = w;
    }

    void dijkstra(int src) {
        vector<int> dist(V, INF);
        vector<bool> visited(V, false);

        dist[src] = 0;

        for (int i = 0; i < V - 1; i++) {
            int u = -1;

            for (int j = 0; j < V; j++)
                if (!visited[j] && (u == -1 || dist[j] < dist[u]))
                    u = j;

            visited[u] = true;

            for (int v = 0; v < V; v++) {
                if (matrix[u][v] != INF && dist[v] > dist[u] + matrix[u][v]) {
                    dist[v] = dist[u] + matrix[u][v];
                }
            }
        }

        cout << "\nDijkstra (Matrix):\n";
        for (int i = 0; i < V; i++)
            cout << "Node " << i << " = " << dist[i] << endl;
    }
};

// ================= ĐỌC FILE =================
// Format file:
// V E
// u v w
GraphList readFileList(string filename) {
    ifstream fin(filename);
    int V, E;
    fin >> V >> E;

    GraphList g(V);

    for (int i = 0; i < E; i++) {
        int u, v, w;
        fin >> u >> v >> w;
        g.addEdge(u, v, w);
    }

    fin.close();
    return g;
}

GraphMatrix readFileMatrix(string filename) {
    ifstream fin(filename);
    int V, E;
    fin >> V >> E;

    GraphMatrix g(V);

    for (int i = 0; i < E; i++) {
        int u, v, w;
        fin >> u >> v >> w;
        g.addEdge(u, v, w);
    }

    fin.close();
    return g;
}

// ================= RANDOM GRAPH =================
GraphList randomGraphList(int V, int E) {
    GraphList g(V);
    srand(time(0));

    for (int i = 0; i < E; i++) {
        int u = rand() % V;
        int v = rand() % V;
        int w = rand() % 100 + 1;
        g.addEdge(u, v, w);
    }

    return g;
}

// ================= MAIN TEST =================
int main() {
    // ===== TEST FILE =====
    GraphList g1 = readFileList("graph.txt");
    g1.dijkstra(0);

    GraphMatrix g2 = readFileMatrix("graph.txt");
    g2.dijkstra(0);

    // ===== TEST RANDOM =====
    GraphList g3 = randomGraphList(5, 10);
    cout << "\nRandom Graph Test:\n";
    g3.dijkstra(0);

    return 0;
}