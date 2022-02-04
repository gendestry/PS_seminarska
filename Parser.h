#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "SparseMatrix.h"

// node class containing it's id, num of outbound connections and it's links
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

template<class T>
struct SparseMatrixData {
    SparseMatrix<T> matrix; // stores the M matrix
    std::unordered_map<unsigned int, int> idMap; // id mappings <ID, arrayIndex>
};

class Parser {
public:
    Parser() = delete;
    ~Parser() = delete;

    template<class T>
    static std::unordered_map<unsigned int, Node<T>*> getNodes(std::string path);

    template<class T>
    static SparseMatrixData<T> getSparseMatrix(std::string path);
};