#include <math.h>
#include "Parser.h"
#include "Matrix.h"


template<typename T>
void graphIteration(std::string path);

template<typename T>
void matrixIteration(std::string path);

int main(int argc, char** argv) {
    matrixIteration<double>("graph-test.txt");



    return 0;
}

template<typename T>
void graphIteration(std::string path) {
    auto nodes = Parser::getNodes<T>(path);

    for (auto& [id, node] : nodes) {
        std::cout << node << std::endl;
    }

    // initial setup
    const int N = nodes.size();
    const T initRank = 1.0 / N;
    const T d = 0.85;
    const T offset = (1 - d) / N;

    for (auto& [id, node] : nodes) {
        node->rank = initRank;
        node->prev_rank = initRank;
    }

    // start iterating
    T temp;
    T rankDiff = 1.0;
    const T err = 1e-5;

    for (int counter = 0; rankDiff > err; counter++) {
        rankDiff = 0.0;
        for (auto& [id, node] : nodes) {
            node->prev_rank = node->rank;
            temp = 0.0;
            for (auto& child : node->links) {
                temp += child->prev_rank / child->out_count;
            }
            node->rank = offset + d * temp;
            rankDiff += fabs(node->rank - node->prev_rank);
            std::cout << "[" << counter << "] " << node->id << ": " << node->rank << std::endl;
        }
    }
}

template<typename T>
void matrixIteration(std::string path) {
    auto matrixInfo = Parser::getMatrix<T>(path);
    auto& M = matrixInfo.matrix;

    const int N = matrixInfo.id_map.size();
    const T initRank = 1.0 / N;
    const T d = 0.85;
    const T offset = (1 - d) / N;

    Matrix<T> rank(N, 1, initRank);
    Matrix<T> prev_rank(N, 1, initRank);
    Matrix<T> ones(N, 1, offset);

    T rankDiff = 1.0;
    const T err = 1e-5;

    for (int counter = 0; rankDiff > err; counter++) {
        rankDiff = 1.0;
        prev_rank = rank;
        rank = ((M * prev_rank) * d) + ones;
        rankDiff = (rank - prev_rank).abs().sum();
        std::cout << "--- [" << counter << "] ---" << std::endl;
        std::cout << rank;
    }
}