#include "Parser.h"
#include <fstream>


template<class T>
std::unordered_map<unsigned int, Node<T>*> Parser::getNodes(std::string path) {
    std::unordered_map<unsigned int, Node<T>*> nodes;

    std::ifstream file(path);
    std::string line;

    ASSERT(!file.is_open(), "[getNodes] file does not exist!")

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

        // get the "from" node, if it doenst exist, create it
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

        // get the "to" node, if it doesn't exist, create it
        auto res = nodes.find(to_id);
        if (res != nodes.end()) {
            to = res->second;
        }
        else {
            to = new Node<T>();
            to->id = to_id;
            nodes[to_id] = to;
        }

        // append the "from" node to "to" node's children and increase the outbound connection count of "from" node
        to->links.push_back(from);
        from->outCount++;
    }

    file.close();
    return nodes;
}

template <class T>
MatrixData<T> Parser::getMatrix(std::string path) {
    MatrixData<T> ret;

    std::ifstream file(path);
    std::string line;

    ASSERT(!file.is_open(), "[getMatrix] file does not exist!")

    auto& matrix = ret.matrix; // the M matrix
    auto& idMap = ret.idMap; // map node ids <ID, arrayIndex> (used for indexing)
    auto& outbound = ret.outbound; // num of outbound connections

    // firstly we populate the idMap 
    int curr_id = 0, prev_id = -1;
    int counter = 0;
    while (std::getline(file, line)) {
        if (line[0] == '#') continue; // skip if comment

        curr_id = atoi(line.substr(0, line.find('\t')).c_str());
        if (prev_id != curr_id) {
            idMap[curr_id] = counter++;
        }

        prev_id = curr_id;
    }

    // get back to the beginning of the file
    file.clear();
    file.seekg(file.beg);

    // do the parsing
    unsigned int tabloc;
    unsigned int from_id, to_id;
    int yindex = 0;
    unsigned int num_ids = idMap.size();

    matrix = Matrix<T>(num_ids, num_ids);
    outbound.resize(num_ids);

    while (std::getline(file, line)) {
        if (line[0] == '#') continue;

        tabloc = line.find('\t');
        from_id = atoi(line.substr(0, tabloc).c_str());
        to_id = atoi(line.substr(tabloc + 1).c_str());

        yindex = idMap[to_id];

        matrix[{yindex, idMap[from_id]}] = 1.0;
        outbound[idMap[from_id]]++;
    }

    // set initial M matrix values
    T mul = 1.0;
    for (int i = 0; i < num_ids; i++) {
        mul = 1.0 / outbound[i];
        for (int j = 0; j < num_ids; j++) {
            matrix[{j,i}] *= mul;
        }
    }

    file.close();
    return ret;
}


template std::unordered_map<unsigned int, Node<float>*> Parser::getNodes(std::string path);
template std::unordered_map<unsigned int, Node<double>*> Parser::getNodes(std::string path);

template MatrixData<float> Parser::getMatrix(std::string path);
template MatrixData<double> Parser::getMatrix(std::string path);