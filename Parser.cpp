#include "Parser.h"
#include <fstream>
#include <algorithm>


std::unordered_map<unsigned int, Node*> Parser::getNodes(std::string path) {
	std::unordered_map<unsigned int, Node*> nodes;

	std::ifstream file(path);
    std::string line;

    unsigned int tabloc;
    unsigned int from_id, to_id;
    Node* from = nullptr;
    Node* to = nullptr;
    Node* prevfrom = nullptr;

    while(std::getline(file, line)) {
        if(line[0] == '#') continue;

        tabloc = line.find('\t');
        from_id = atoi(line.substr(0, tabloc).c_str());
        to_id = atoi(line.substr(tabloc + 1).c_str());

        if(from == nullptr || from_id != from->id) {
            auto res = nodes.find(from_id);
            if(res != nodes.end()) {
                from = res->second;
            }
            else {
                from = new Node();
                from->id = from_id;
                nodes[from_id] = from;
            }
        }

        auto res = nodes.find(to_id);
        if(res != nodes.end()) {
            to = res->second;
        }
        else {
            to = new Node();
            to->id = to_id;
            nodes[to_id] = to;
        }

        to->links.push_back(from);
        from->out_count++;
    }

    file.close();
	return nodes;
}

// template <class T>
MatrixData Parser::getMatrix(std::string path) {
    MatrixData ret;

    std::ifstream file(path);
    std::string line;

    // map ids <ID, arrayIndex>
    auto& matrix = ret.matrix;
    auto& id_map = ret.id_map;
    auto& outbound = ret.outbound;

    int curr_id = 0, prev_id = -1;
    int counter = 0;
    while(std::getline(file, line)) {
        if(line[0] == '#') continue;

        curr_id = atoi(line.substr(0, line.find('\t')).c_str());
        if(prev_id != curr_id) {
            id_map[curr_id] = counter++;
        }

        prev_id = curr_id;
    }
    
    // get back to 0
    file.clear();
    file.seekg(file.beg);

    // do the parsing
    unsigned int tabloc;
    unsigned int from_id, to_id;
    unsigned int yindex = 0;
    unsigned int num_ids = id_map.size();

    matrix.resize(num_ids);
    outbound.resize(num_ids);

    while(std::getline(file, line)) {
        if(line[0] == '#') continue;

        tabloc = line.find('\t');
        from_id = atoi(line.substr(0, tabloc).c_str());
        to_id = atoi(line.substr(tabloc + 1).c_str());

        yindex = id_map[to_id];
        if(matrix[yindex].size() == 0) {
            matrix[yindex].resize(num_ids);
        }

        matrix[yindex][id_map[from_id]] = 1.0f;
        outbound[id_map[from_id]]++;
    }

    // 
    float mul = 1.0f;
    for(int i = 0; i < num_ids; i++) {
        mul = 1.0f / std::count_if(matrix[i].begin(), matrix[i].end(), [](int i) { return i > 0; });
        for(int j = 0; j < num_ids; j++) {
            matrix[i][j] *= mul;
        }
    }

    return ret;
}

// template std::vector<std::vector<float>> Parser::getMatrix(std::string path);
// template std::vector<std::vector<double>> Parser::getMatrix(std::string path);