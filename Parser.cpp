#include "Parser.h"
#include <fstream>


template<class T>
std::unordered_map<unsigned int, Node<T>*> Parser::getNodes(std::string path) {
    std::unordered_map<unsigned int, Node<T>*> nodes;

    std::ifstream file(path);
    std::string line;

    unsigned int tabloc;
    unsigned int from_id, to_id;
    Node<T>* from = nullptr;
    Node<T>* to = nullptr;
    Node<T>* prevfrom = nullptr;

    while (std::getline(file, line)) {
        if (line[0] == '#') continue;

        tabloc = line.find('\t');
        from_id = atoi(line.substr(0, tabloc).c_str());
        to_id = atoi(line.substr(tabloc + 1).c_str());

        if (from == nullptr || from_id != from->id) {
            auto res = nodes.find(from_id);
            if (res != nodes.end()) {
                from = res->second;
            }
            else {
                from = new Node<T>();
                from->id = from_id;
                nodes[from_id] = from;
            }
        }

        auto res = nodes.find(to_id);
        if (res != nodes.end()) {
            to = res->second;
        }
        else {
            to = new Node<T>();
            to->id = to_id;
            nodes[to_id] = to;
        }

        to->links.push_back(from);
        from->out_count++;
    }

    file.close();
    return nodes;
}

template <class T>
MatrixData<T> Parser::getMatrix(std::string path) {
    MatrixData<T> ret;

    std::ifstream file(path);
    std::string line;

    // map ids <ID, arrayIndex>
    auto& matrix = ret.matrix;
    auto& id_map = ret.id_map;
    auto& outbound = ret.outbound;

    int curr_id = 0, prev_id = -1;
    int counter = 0;
    while (std::getline(file, line)) {
        if (line[0] == '#') continue;

        curr_id = atoi(line.substr(0, line.find('\t')).c_str());
        if (prev_id != curr_id) {
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
    int yindex = 0;
    unsigned int num_ids = id_map.size();

    matrix = Matrix<T>(num_ids, num_ids);
    // matrix.resize(num_ids);
    outbound.resize(num_ids);

    while (std::getline(file, line)) {
        if (line[0] == '#') continue;

        tabloc = line.find('\t');
        from_id = atoi(line.substr(0, tabloc).c_str());
        to_id = atoi(line.substr(tabloc + 1).c_str());

        yindex = id_map[to_id];

        matrix[{yindex, id_map[from_id]}] = 1.0;
        outbound[id_map[from_id]]++;
    }

    // set initial M matrix values
    T mul = 1.0;
    for (int i = 0; i < num_ids; i++) {
        mul = 1.0 / outbound[i];
        for (int j = 0; j < num_ids; j++) {
            matrix[{j,i}] *= mul;
        }
    }

    return ret;
}


template std::unordered_map<unsigned int, Node<float>*> Parser::getNodes(std::string path);
template std::unordered_map<unsigned int, Node<double>*> Parser::getNodes(std::string path);

template MatrixData<float> Parser::getMatrix(std::string path);
template MatrixData<double> Parser::getMatrix(std::string path);