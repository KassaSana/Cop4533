#include <iostream>
#include <limits>
#include <unordered_map>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <string>
using namespace std;

// Struct to represent cell in maze
struct Cell {
    int x{}, y{}, id{};
    string type; // "enter", "exit", "path", "wall"
};

// Priority‐q node for finding a way lowest cost
struct Node {
    int id;
    double cost;
    bool operator>(Node const& o) const { return cost > o.cost; }
};

void reverse(vector<int>::iterator iterator, vector<int>::iterator iterator1);

//Builds the graph
unordered_map<int, vector<pair<int,double>>>
graphBuild(vector<Cell> const& maze, int rows, int cols) {
    unordered_map<int, vector<pair<int,double>>> graph;
    static int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
    for (auto const& c : maze) {
        if (c.type == "wall") continue;
        //checks neighboring cells
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

//tracks performance
int nodesExpanded, edgesRelaxed;


vector<int> dijkstra(unordered_map<int, vector<pair<int,double>>> const& graph, int start, int end){
    //shortest distance to each node and reconstruct path
    unordered_map<int,double> dist;
    unordered_map<int,int> prev;
    priority_queue<Node, vector<Node>, greater<Node>> pq; //min heap

    for (auto const& kv : graph)
        dist[kv.first] = numeric_limits<double>::infinity();

    dist[start] = 0.0;
    pq.push({start, 0.0});

    //reset counter
    nodesExpanded = edgesRelaxed = 0;

    while (!pq.empty()) {
        int u = pq.top().id; pq.pop();
        ++nodesExpanded;
        if (u == end) break;
        for (auto const& [v, w] : graph.at(u)) {
            double nd = dist[u] + w;
            ++edgesRelaxed;
            if (nd < dist[v]) {
                dist[v] = nd;
                prev[v] = u;
                pq.push({v, nd});
            }
        }
    }

    //reconstruct path from end to start
    vector<int> path;
    if (!prev.count(end) && start != end)
        return path;

    for (int at = end; at != start; at = prev[at])
        path.push_back(at);
    path.push_back(start);
    reverse(path.begin(), path.end());
    return path;
}

void reverse(vector<int>::iterator iterator, vector<int>::iterator iterator1) {

}
//randomly generates a maze
vector<Cell> generateMaze(int rows, int cols) {
    vector<Cell> maze(rows*cols);
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j) {
            int id = i*cols + j;
            maze[id] = {i, j, id, (rand()%10 < 2 ? "wall" : "path")};
        }
    maze[0].type = "enter";//starts top lefft
    maze[rows*cols-1].type = "exit"; //bottom right is end goal
    return maze;
}

int main() {

    srand(1234); //over and over again
    cout << "size,elapsed_s,nodes_expanded,edges_relaxed,path_length\n";
    //loops of diffferent size mazes
    for (int size = 100; size <= 500; size += 100) {
        int rows = size, cols = size;
        int start = 0, end = rows*cols - 1;

        //create maze and build graph
        auto maze  = generateMaze(rows, cols);
        auto graph = graphBuild(maze, rows, cols);

        clock_t t0 = clock();
        auto path = dijkstra(graph, start, end);
        clock_t t1 = clock();

        double elapsed = double(t1 - t0) / CLOCKS_PER_SEC;
        int plen = path.empty() ? 0 : path.size();

        // Output perforamance metrics
        cout << rows << "x" << cols << ","
             << fixed << setprecision(5) << elapsed << ","
             << nodesExpanded << ","
             << edgesRelaxed << ","
             << plen << "\n";
    }


    return 0;
}
