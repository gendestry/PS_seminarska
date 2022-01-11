#include "Parser.h"
#include <fstream>

std::unordered_map<unsigned int, Node*> Parser::getNodes(std::string path) {
	std::unordered_map<unsigned int, Node*> nodes;

	std::ifstream file(path);
    std::string line;

    int counter = 0;
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