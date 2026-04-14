#include <iostream>
#include <vector>
#include <list>
#include <fstream>
#include <random>
#include <iomanip>

using namespace std;

const int INF = 1e9;

class Graph {
private:
    int V; // Số đỉnh
    bool isMatrix; // Xác định kiểu lưu trữ

    // 1. Ma trận kề
    vector<vector<int>> adjMatrix;

    // 2. Danh sách kề (vector chứa các cặp {đỉnh kề, trọng số})
    vector<list<pair<int, int>>> adjList;

public:
    Graph(int vertices, bool useMatrix = true) : V(vertices), isMatrix(useMatrix) {
        if (isMatrix) {
            adjMatrix.assign(V, vector<int>(V, 0));
        } else {
            adjList.resize(V);
        }
    }

    // Thêm cạnh
    void addEdge(int u, int v, int w) {
        if (u >= V || v >= V) return;
        if (isMatrix) {
            adjMatrix[u][v] = w;
            adjMatrix[v][u] = w; // Đồ thị vô hướng (cho giao thông)
        } else {
            adjList[u].push_back({v, w});
            adjList[v].push_back({u, w});
        }
    }

    // Đọc dữ liệu từ file .txt
    static Graph* fromFile(string filename, bool useMatrix) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Không thể mở file!" << endl;
            return nullptr;
        }
        int v, e;
        file >> v >> e;
        Graph* g = new Graph(v, useMatrix);
        for (int i = 0; i < e; i++) {
            int u, v_node, w;
            file >> u >> v_node >> w;
            g->addEdge(u, v_node, w);
        }
        file.close();
        return g;
    }

    // Tạo đồ thị ngẫu nhiên
    void generateRandom(int E) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> disNode(0, V - 1);
        uniform_int_distribution<> disWeight(1, 100);

        for (int i = 0; i < E; i++) {
            addEdge(disNode(gen()), disNode(gen()), disWeight(gen()));
        }
    }

    // Hiển thị đồ thị để kiểm tra (Test dữ liệu)
    void display() {
        cout << "\n--- Cau truc do thi (" << (isMatrix ? "Ma tran ke" : "Danh sach ke") << ") ---" << endl;
        if (isMatrix) {
            for (int i = 0; i < V; i++) {
                for (int j = 0; j < V; j++) {
                    cout << setw(4) << adjMatrix[i][j];
                }
                cout << endl;
            }
        } else {
            for (int i = 0; i < V; i++) {
                cout << "Kho " << i << ": ";
                for (auto& edge : adjList[i]) {
                    cout << "-> (Kho " << edge.first << ", w:" << edge.second << ") ";
                }
                cout << endl;
            }
        }
    }
};

int main() {
    int numNodes = 5;
    int numEdges = 7;

    // Test 1: Ma trận kề + Random
    Graph gMatrix(numNodes, true);
    gMatrix.generateRandom(numEdges);
    cout << "Test 1: Random Matrix Graph";
    gMatrix.display();

    // Test 2: Danh sách kề + Read File
    // Giả sử bạn đã có file "data.txt", nếu không chương trình sẽ báo lỗi mở file.
    cout << "\nTest 2: Adjacency List from File";
    Graph* gList = Graph::fromFile("graph.txt", false);
    if (gList) {
        gList->display();
        delete gList;
    }

    return 0;
}