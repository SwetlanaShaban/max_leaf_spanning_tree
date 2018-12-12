#include "solver.h"

int main() {
    vector<vector<int>> graph = Input();
    vector<vector<int>> maximum_leafes_spanning_tree = FindMaximumLeafSpanningTree(graph);
    Output(maximum_leafes_spanning_tree);
    return 0;
}
