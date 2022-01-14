#include <math.h>
#include "Parser.h"
#include "Matrix.h"


template<typename T>
void graphIteration(std::string path);

template<typename T>
void matrixIteration(std::string path);

int main(int argc, char** argv) {
    // graphIteration<float>("graph-test.txt");
    // matrixIteration<float>("graph-test.txt");
    Matrix<float> a {
        {1, 2},
        {2, 3},
        {3, 4}
    };

    return 0;
}

template<typename T>
void graphIteration(std::string path) {
    auto nodes = Parser::getNodes<T>(path);

    for(auto& [id, node] : nodes) {
        std::cout << node << std::endl; 
    }

    // initial setup
    const int N = nodes.size();
    const T initRank = 1.0 / N;
    const T d = 0.85;
    const T offset = (1 - d) / N;

    for(auto& [id, node] : nodes) {
        node->rank = initRank;
        node->prev_rank = initRank;
    }

    // start iterating
    T temp;
    T sum_rank_diff = 1.0;
    const T err = 1e-5;

    for(int counter = 0; sum_rank_diff > err; counter++) {
        sum_rank_diff = 0.0;
        for(auto& [id, node] : nodes) {
            node->prev_rank = node->rank;
            temp = 0.0;
            for(auto& child : node->links) {
                temp += child->prev_rank / child->out_count;
            }
            node->rank = offset + d * temp;
            sum_rank_diff += fabs(node->rank - node->prev_rank);
            std::cout << "[" << counter << "] " << node->id << ": " << node->rank << std::endl;
        }
    }
}

template<typename T>
void matrixIteration(std::string path) {
    auto matrixInfo = Parser::getMatrix<T>(path);
    auto& M = matrixInfo.matrix;

    /*for(auto& vec : M) {
        for(auto& el : vec)
            std::cout << el << "  ";
        std::cout << std::endl;
    } */

    const int N = matrixInfo.id_map.size();
    const T initRank = 1.0 / N;
    const T d = 0.85;
    const T offset = (1 - d) / N;

    Matrix<T> rank(N);
    Matrix<T> prev_rank(N);
    Matrix<T> ones(N);

    for(int i = 0; i < N; i++) {
        rank[i].push_back(initRank);
        prev_rank[i].push_back(initRank);
        ones[i].push_back(offset);
    }

    /*T sum_rank_diff = 1.0;
    const T err = 1e-5;

    for(int counter = 0; sum_rank_diff > err; counter++) {
        sum_rank_diff = 0.0;
        prev_rank = rank;
        rank = matsum(matscale(matmul(M, prev_rank), d), ones);
        sum_rank_diff = fabs(matsumel(rank) - matsumel(prev_rank));
        std::cout << "[" << counter << "] ";
        for(auto& el : rank) {
            std::cout << el << "  ";
        }
        std::cout << std::endl;
    }*/
}