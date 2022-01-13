#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

struct Node {
    unsigned int id;
    unsigned int out_count;
    float rank = 0.0f;
    float prev_rank = 0.0f;

    std::vector<Node*> links;

    friend std::ostream& operator<< (std::ostream& out, const Node* node) {
        out << node->id << "[" << node->out_count << "]\t";
        for(auto& el : node->links) {
            out << el->id << "  ";
        }
        return out;
    }
};

struct MatrixData {
    std::vector<std::vector<float>> matrix; // stores the M matrix
    std::unordered_map<unsigned int, unsigned int> id_map; // id mappings <ID, arrayIndex>
    std::vector<unsigned int> outbound; // num outbound connections
};


class Parser {
public:
	Parser() = delete;
	~Parser() = delete;

	static std::unordered_map<unsigned int, Node*> getNodes(std::string path);
    static MatrixData getMatrix(std::string path);
};