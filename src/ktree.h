#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include "segment.h"

struct Node
{
    std::vector<int> keys;
    std::vector<Segment *> values;
    std::vector<std::unique_ptr<Node>> children;

    bool is_leaf() const
    {
        return children.empty();
    }
};

template <int K>
class KTree
{
    std::unique_ptr<Node> root;
    std::unique_ptr<Node> build_recursive(const std::vector<std::pair<int, Segment *>> &entries, int left, int right);

public:
    void build(const std::vector<std::pair<int, Segment *>> &sorted_entries);
    Segment *search_tree(int key) const;
    int *lookup(int key) const
    {
        Segment *seg = search_tree(key);
        if (seg)
            return search_segment(seg, key);
        return nullptr;
    }
};

template <int K>
std::unique_ptr<Node> KTree<K>::build_recursive(const std::vector<std::pair<int, Segment *>> &entries, int left, int right)
{
    int n = right - left;
    if (n == 0)
        return nullptr;

    std::unique_ptr<Node> node = std::make_unique<Node>();
    if (n <= K)
    {
        for (int i = left; i < right; i++)
        {
            node->keys.push_back(entries[i].first);
            node->values.push_back(entries[i].second);
        }
        return node;
    }

    int chunk_size = (n + K) / (K + 1); // divide into K+1 children
    int index = left;

    for (int i = 0; i < K; i++)
    {
        int mid = std::min(index + chunk_size, right - (K - i - 1) * chunk_size);
        // mid - 1 is used as key and value
        if (mid - 1 >= left && mid - 1 < right)
        {
            node->keys.push_back(entries[mid - 1].first);
            node->values.push_back(entries[mid - 1].second);
        }

        node->children.push_back(build_recursive(entries, index, mid - 1));
        index = mid;
    }

    node->children.push_back(build_recursive(entries, index, right));
    return node;
}

template <int K>
void KTree<K>::build(const std::vector<std::pair<int, Segment *>> &sorted_entries)
{
    root = build_recursive(sorted_entries, 0, sorted_entries.size());
}

template <int K>
Segment *KTree<K>::search_tree(int key) const
{
    Node *node = root.get();
    Segment *best_val = nullptr;
    int best_key;
    int i;

    while (node)
    {
        i = 0;
        while (i < node->keys.size() && key > node->keys[i])
            i++;

        if (i < node->keys.size() && (!best_val || node->keys[i] < best_key))
        {
            best_val = node->values[i];
            best_key = node->keys[i];
        }
        if (node->is_leaf())
            break;

        node = node->children[i].get();
    }

    return best_val;
}
