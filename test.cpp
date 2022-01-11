#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <unordered_map>

struct Node {
    unsigned int id;
    unsigned int out_count;

    std::vector<Node*> links;

    std::string toString() {
        std::stringstream ss;
        ss << id << "[" << out_count << "]\t";
        for(auto& el : links) {
            ss << el->id << "  ";
        }

        return ss.str();
    }
};

int main(int argc, char** argv) {
    std::ifstream file("graph-test.txt");
    std::string line;
    std::unordered_map<unsigned int, Node*> nodes;

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
        // if(counter++ > 30) break;
    }

    for(auto& [id, node] : nodes) {
        std::cout << node->toString() << std::endl; 
    }

    file.close();
    return 0;
}