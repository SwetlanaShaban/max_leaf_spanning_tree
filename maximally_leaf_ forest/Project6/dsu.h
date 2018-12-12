#pragma once
#include <vector>
#include <iostream>

using std::vector;


class DisjointSetUnion {
public:
    DisjointSetUnion(int size) {
        parent.resize(size);
        rank.resize(size);
        for (int i = 0; i < size; ++i) {
            MakeSet(i);
        }
    }

    void MakeSet(int element) {
        parent[element] = element;
        rank[element] = 0;
    }


    int FindParent(int element) {
        if (element == parent[element]) {
            return element;
        }
        return parent[element] = FindParent(parent[element]);
    }

    void Union(int x, int y) {
        if ((x = FindParent(x)) == (y = FindParent(y))) {
            return;
        }
        if (rank[x] < rank[y]) {
            parent[x] = y;
        } else {
            parent[y] = x;
            if (rank[x] == rank[y]) {
                ++rank[x];
            }
        }
    }

private:
    vector <int> parent, rank;
};
