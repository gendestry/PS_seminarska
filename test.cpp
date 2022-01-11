#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <math.h>

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

std::unordered_map<unsigned int, Node*> nodes;

bool parseFile(std::string path) {
    nodes.clear();
    
    std::ifstream file(path);
    if(!file.is_open()) {
        return false;
    }

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
        // if(counter++ > 30) break;
    }
    file.close();
    return true;
}

int main(int argc, char** argv) {
    parseFile("graph-test.txt");

    // print out    
    for(auto& [id, node] : nodes) {
        std::cout << node << std::endl; 
    }

    // initial setup
    const int N = nodes.size();
    const float initRank = 1.0f / N;
    const float d = 0.85f;
    const float offset = (1 - d) / N;

    for(auto& [id, node] : nodes) {
        node->rank = initRank;
        node->prev_rank = initRank;
    }

    // start iterating
    float temp;
    float sum_rank_diff;
    const float err = 1e-5;

    for(int counter = 0; sum_rank_diff > err; counter++) {
        sum_rank_diff = 0.0f;
        for(auto& [id, node] : nodes) {
            node->prev_rank = node->rank;
            temp = 0.0f;
            for(auto& child : node->links) {
                temp += child->prev_rank / child->out_count;
            }
            node->rank = offset + d * temp;
            sum_rank_diff += fabs(node->rank - node->prev_rank);
            std::cout << "[" << counter << "] " << node->id << ": " << node->rank << std::endl;
        }
    }

    return 0;
}