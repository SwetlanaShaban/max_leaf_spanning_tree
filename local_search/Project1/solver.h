#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <queue>
#include <stack>
#include <algorithm>
#include <string>


using std::vector;
using std::stack;
using std::pair;
using std::find;


void AddEdgeToGraph(vector<vector<int>>* graph, int u, int v) {
    graph->at(u).push_back(v);
    graph->at(v).push_back(u);
}


int CalculateNumberOfLeaves(const vector<vector<int>>& spanning_tree) {
    return std::count_if(spanning_tree.begin(), spanning_tree.end(), [](const vector<int>& v) {return v.size() == 1; });
}


vector<vector<int>> Input() {
    std::ifstream fin("input.txt");
    if (!fin.is_open()) {
        throw "Error: can't open input file.";
    }
    size_t number_of_vertices, number_of_edges;
    fin >> number_of_vertices >> number_of_edges;
    vector<vector<int>> graph(number_of_vertices);
    for (size_t i = 0; i < number_of_edges; ++i) {
        int u, v;
        fin >> u >> v;
        if (u != v)
            AddEdgeToGraph(&graph, u, v);
    }
    fin.close();
    return graph;
}


void Output(const vector<vector<int>>& tree) {
    std::ofstream fout("output.txt");
    if (!fout.is_open()) {
        throw "Error: can't open output file.";
    }
    fout << "Maximum number of leaves = " << CalculateNumberOfLeaves(tree) << "\n";
    fout << "Maximum leafes spanning tree:\n";
    for (size_t i = 0; i < tree.size(); ++i) {
        for (size_t j = 0; j < tree[i].size(); ++j) {
            if (i < tree[i][j]) {
                fout << i << " - " << tree[i][j] << "\n";
            }
        }
    }
    fout.close();
}


//bfs
vector<vector<int>> BuildSpanningTree(const vector<vector<int>>& graph) {
    vector<vector<int>> spanning_tree(graph.size());
    std::queue<int> queue;
    vector<bool> used(graph.size(), false);
    for (size_t i = 0; i < used.size(); ++i) {
        if (used[i]) {
            continue;
        }
        queue.push(i);
        used[i] = true;
        while (!queue.empty()) {
            int u = queue.front();
            queue.pop();
            for (size_t j = 0; j < graph[u].size(); ++j) {
                int v = graph[u][j];
                if (used[v]) {
                    continue;
                }
                used[v] = true;
                queue.push(v);
                AddEdgeToGraph(&spanning_tree, u, v);
            }
        }
    }
    return spanning_tree;
}


bool TreeHasEdge(const vector<vector<int>>& tree, int u, int v) {
    return find(tree[v].begin(), tree[v].end(), u) != tree[v].end();
}


//dfs
void FindCycle(const vector<vector<int>>& grap_with_one_cycle, stack<int>* cycle, int first_vertex) {
    vector<int> numbers(grap_with_one_cycle.size(), 0);
    while (true) {
        int u = cycle->top();
        if (numbers[u] == grap_with_one_cycle[u].size()) {
            cycle->pop();
        }
        else {
            cycle->push(grap_with_one_cycle[u][numbers[u]]);
            if (grap_with_one_cycle[u][numbers[u]] == first_vertex) {
                return;
            }
            ++numbers[u];
        }
    }
}


struct EdgeToDelete {
    bool isEmpty = true;
    int u, v;
};


void FindEdgeToDeleteFromGraph(const vector<vector<int>>& grap_with_one_cycle,
    stack<int>& cycle, EdgeToDelete* edge, int* max_number_of_leaves, bool minus_one_leaf) {
    int max = *max_number_of_leaves;
    while (cycle.size() != 1) {
        int current_number_of_leaves = *max_number_of_leaves;
        if (minus_one_leaf) {
            current_number_of_leaves -= 1;
        }
        int u = cycle.top();
        cycle.pop();
        int v = cycle.top();
        if (grap_with_one_cycle[u].size() == 2) {
            current_number_of_leaves += 1;
        }
        if (grap_with_one_cycle[v].size() == 2) {
            current_number_of_leaves += 1;
        }
        if (current_number_of_leaves > max) {
            edge->u = u;
            edge->v = v;
            edge->isEmpty = false;
            max = current_number_of_leaves;
        }
    }
    if (max > *max_number_of_leaves) {
        *max_number_of_leaves = max;
    }
}


void DeleteEdgeFromGraph(vector<vector<int>>* grap_with_one_cycle, int u, int v) {
    grap_with_one_cycle->at(u).erase(find(grap_with_one_cycle->at(u).begin(), grap_with_one_cycle->at(u).end(), v));
    grap_with_one_cycle->at(v).erase(find(grap_with_one_cycle->at(v).begin(), grap_with_one_cycle->at(v).end(), u));
}


vector<vector<int>> FindMaximumLeafesSpanningTree(const vector<vector<int>>& graph) {
    vector<vector<int>> spanning_tree = BuildSpanningTree(graph);

    int number_of_leaves = CalculateNumberOfLeaves(spanning_tree);

    bool improvement = true;
    while (improvement) {
        improvement = false;
        for (size_t u = 0; u < graph.size(); ++u) {
            for (size_t i = 0; i < graph[u].size(); ++i) {
                int v = graph[u][i];
                if (TreeHasEdge(spanning_tree, u, v))
                    continue;

                if (spanning_tree.at(u).size() == 1 && spanning_tree.at(v).size() == 1)
                    continue;

                AddEdgeToGraph(&spanning_tree, u, v);

                stack<int> cycle;
                cycle.push(u);
                cycle.push(v);
                FindCycle(spanning_tree, &cycle, u);

                bool minus_one_leaf = (spanning_tree.at(u).size() == 2 || spanning_tree.at(v).size() == 2);
                EdgeToDelete edge;
                FindEdgeToDeleteFromGraph(spanning_tree, cycle, &edge, &number_of_leaves, minus_one_leaf);
                if (!edge.isEmpty) {
                    improvement = true;
                    DeleteEdgeFromGraph(&spanning_tree, edge.u, edge.v);
                }
                else {
                    DeleteEdgeFromGraph(&spanning_tree, u, v);
                }
            }
        }
    }
    return spanning_tree;
}