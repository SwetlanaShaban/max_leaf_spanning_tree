#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <map>
#include <string>
#include "dsu.h"


using std::vector;
using std::map;
using std::for_each;


void AddEdgeToGraph(vector<vector<int>>* graph, int u, int v) {
    graph->at(u).push_back(v);
    graph->at(v).push_back(u);
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


int CalculateNumberOfLeaves(const vector<vector<int>>& spanning_tree) {
    return std::count_if(spanning_tree.begin(), spanning_tree.end(), [](const vector<int>& v) {return v.size() == 1; });
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


void AddEdgeToForest(vector<vector<int>>* forest, DisjointSetUnion* subtrees, int* parent_v, int parent_u, int u, int v) {
    AddEdgeToGraph(forest, u, v);
    subtrees->Union(*parent_v, parent_u);
    *parent_v = subtrees->FindParent(v);
}


bool MapContainsKey(const map<int, int>& m, int key) {
    return m.find(key) != m.end();
}


vector<vector<int>> FindMaximallyLeafyForest(const vector<vector<int>>& graph, DisjointSetUnion* subtrees) {
    vector<vector<int>> forest(graph.size());
    for (size_t v = 0; v < graph.size(); ++v) {
        map<int, int> vertex_and_parent_can_be_added;
        int number = 0;
        int parent_v = subtrees->FindParent(v);
        for (const int &u : graph[v]) {
            int parent_u = subtrees->FindParent(u);
            if (parent_u != parent_v && !MapContainsKey(vertex_and_parent_can_be_added, parent_u)) {
                ++number;
                vertex_and_parent_can_be_added.insert(std::make_pair(parent_u, u));
            }
            if (vertex_and_parent_can_be_added.size() >= 3) {
                for_each(vertex_and_parent_can_be_added.begin(), vertex_and_parent_can_be_added.end(), [&](const auto &pair) {
                    AddEdgeToForest(&forest, subtrees, &parent_v, pair.first, pair.second, v);
                });
                vertex_and_parent_can_be_added.clear();
            }
        }
        if (forest.at(v).size() + number >= 3) {
            for_each(vertex_and_parent_can_be_added.begin(), vertex_and_parent_can_be_added.end(), [&](const auto &pair) {
                AddEdgeToForest(&forest, subtrees, &parent_v, pair.first, pair.second, v);
            });
        }
    }
    return forest;
}


void MaxLeafForestToMaxLeafTree(const vector<vector<int>>& graph, vector<vector<int>>* forest,
    DisjointSetUnion& subtrees) {
    for (size_t v = 0; v < graph.size(); ++v) {
        int parent_v = subtrees.FindParent(v);
        for (const int &u : graph[v]) {
            int parent_u = subtrees.FindParent(u);
            if (parent_u != parent_v) {
                AddEdgeToForest(forest, &subtrees, &parent_v, parent_u, u, v);
            }
        }
    }
}


vector<vector<int>> FindMaximumLeafSpanningTree(const vector<vector<int>>& graph) {
    DisjointSetUnion subtrees = DisjointSetUnion(graph.size());
    vector<vector<int>> spanning_tree = FindMaximallyLeafyForest(graph, &subtrees);
    MaxLeafForestToMaxLeafTree(graph, &spanning_tree, subtrees);
    return spanning_tree;
}
