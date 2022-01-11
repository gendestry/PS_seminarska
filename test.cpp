#include <math.h>
#include "Parser.h"


int main(int argc, char** argv) {
    auto nodes = Parser::getNodes("graph-test.txt");

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
    float sum_rank_diff = 1.0f;
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