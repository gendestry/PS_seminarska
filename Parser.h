#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "Matrix.h"

template<class T>
struct Node {
    unsigned int id;
    unsigned int out_count;
    T rank = 0.0f;
    T prev_rank = 0.0f;

    std::vector<Node<T>*> links;

    friend std::ostream& operator<< (std::ostream& out, const Node* node) {
        out << node->id << "[" << node->out_count << "]\t";
        for (auto& el : node->links) {
            out << el->id << "  ";
        }
        return out;
    }
};

template<class T>
struct MatrixData {
    Matrix<T> matrix; // stores the M matrix
    std::unordered_map<unsigned int, int> id_map; // id mappings <ID, arrayIndex>
    std::vector<unsigned int> outbound; // num outbound connections
};


class Parser {
public:
    Parser() = delete;
    ~Parser() = delete;

    template<class T>
    static std::unordered_map<unsigned int, Node<T>*> getNodes(std::string path);

    template<class T>
    static MatrixData<T> getMatrix(std::string path);
};