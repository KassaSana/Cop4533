#include <iostream>
#include <limits>
#include <unordered_map>
#include <cstdlib>
#include <vector>
#include <queue>
#include <cmath>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <string>
#include <functional>
using namespace std;

// Struct to represent a cell in the maze
struct Cell {
    int x, y, id;
    string type; // "enter", "exit", "path", "wall"
};

// Priority‐queue node for pathfinding
struct Node {
    int id;
    double cost;
    bool operator>(Node const& o) const { return cost > o.cost; }
};

unordered_map<int, vector<pair<int,double>>>
graphBuild(vector<Cell> const& maze, int rows, int cols) {
    unordered_map<int, vector<pair<int,double>>> graph;
    for (auto const& c : maze) {
        if (c.type == "wall") continue;
        static int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
        for (auto& d : dirs) {
            int nx = c.x + d[0], ny = c.y + d[1];
            if (nx>=0 && nx<rows && ny>=0 && ny<cols) {
                int nid = nx*cols + ny;
                if (maze[nid].type != "wall")
                    graph[c.id].emplace_back(nid, 1.0);
            }
        }
    }
    return graph;
}

vector<int> dijkstra(
    unordered_map<int, vector<pair<int,double>>> const& graph,
    int start, int end)
{
    unordered_map<int,double> dist;
    unordered_map<int,int> prev;
    priority_queue<Node, vector<Node>, greater<Node>> pq;

    for (auto const& kv : graph)
        dist[kv.first] = numeric_limits<double>::infinity();

    dist[start] = 0.0;
    pq.push({start, 0.0});

    while (!pq.empty()) {
        int u = pq.top().id; pq.pop();
        if (u == end) break;
        for (auto const& [v, w] : graph.at(u)) {
            double nd = dist[u] + w;
            if (nd < dist[v]) {
                dist[v] = nd;
                prev[v] = u;
                pq.push({v, nd});
            }
        }
    }

    // reconstruct path
    vector<int> path;
    if (!prev.count(end) && start!=end) {
        cout << "No path found.\n";
        return path;
    }
    for (int at = end; at != start; at = prev[at])
        path.push_back(at);
    path.push_back(start);
    reverse(path.begin(), path.end());
    return path;
}

// Display maze w/ path
void dispMaze(vector<Cell> const& maze, vector<int> const& path,
              int rows, int cols)
{
    vector<string> disp(rows*cols, ".");
    for (int id : path) disp[id] = "*";
    disp[0] = "S";
    disp[rows*cols-1] = "E";
    for (int i=0; i<rows; ++i) {
        for (int j=0; j<cols; ++j)
            cout << disp[i*cols+j] << ' ';
        cout << '\n';
    }
}

vector<Cell> generateMaze(int rows, int cols) {
    vector<Cell> maze(rows*cols);
    for (int i=0; i<rows; ++i) for (int j=0; j<cols; ++j) {
        int id = i*cols + j;
        maze[id] = {i, j, id,
            (rand()%10 < 2 ? "wall" : "path")};
    }
    maze[0].type = "enter";
    maze[rows*cols-1].type = "exit";
    return maze;
}

int main() {
    srand(1234);             // fixed seed for reproducibility
    int rows = 400, cols = 400;
    auto maze = generateMaze(rows, cols);
    auto graph = graphBuild(maze, rows, cols);
    int start = 0, end = rows*cols - 1;

    // Time Dijkstra
    auto t0 = clock();
    auto path = dijkstra(graph, start, end);
    auto t1 = clock();

    double elapsed = double(t1 - t0) / CLOCKS_PER_SEC;
    cout << "Dijkstra completed in "
         << fixed << setprecision(5)
         << elapsed << " seconds\n\n";

    dispMaze(maze, path, rows, cols);
    return 0;
}
