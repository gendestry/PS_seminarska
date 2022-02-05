#pragma once
#include <iostream>
#include <vector>

// Main class for Graph representation, i.e. one node in the graph
template<class T>
struct Node {
    unsigned int id;
    unsigned int outCount;
    T rank = 0.0f;
    T prevRank = 0.0f;

    std::vector<Node<T>*> links;

    friend std::ostream& operator<< (std::ostream& out, const Node* node) {
        out << node->id << "[" << node->outCount << "]\t";
        for (auto& el : node->links) {
            out << el->id << "  ";
        }
        return out;
    }
};