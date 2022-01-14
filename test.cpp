#include <math.h>
#include "Parser.h"
#include "Matrix.h"


template<typename T>
void graphIteration(std::string path);

template<typename T>
void matrixIteration(std::string path);

int main(int argc, char** argv) {
    //graphIteration<double>("graph-test.txt");
    matrixIteration<double>("graph-test.txt");
    // Matrix<float> id({ {1,0,0},{0,1,0}, {0,0,1} });
    // Matrix<float> col({ {1},{2},{3} });
    // std::cout << (id * col) << std::endl;
    /*Matrix<float> a ( {
        {1, 2},
        {2, 3},
        {3, 4}
    } );

    Matrix<float> b ({{1,0,0}, {0,1,0}});
    Matrix<float> id({{1,0,0},{0,1,0}, {0,0,1}});
    
    // auto x = id.mul(a);
    // id.scale(5.f);
    id += id2;
    id *= a;
    id *= -1.0f;
    std::cout << id;*/

    /*Matrix<float> d ({{1,0},{0,0}});
    Matrix<float> e ({{2,3},{4,5}});
    auto f = ((d + e) * d) * 3;
    std::cout << f; */

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

    Matrix<T> rank(N);
    Matrix<T> prev_rank(N);
    Matrix<T> ones(N);


    for (int i = 0; i < N; i++) {
        rank[i].push_back(initRank);
        prev_rank[i].push_back(initRank);
        ones[i].push_back(offset);
    }
    T rankDiff = 1.0;
    const T err = 1e-5;

    for (int counter = 0; rankDiff > err; counter++) {
        rankDiff = 1.0;
        prev_rank = rank;
        rank = ((M * prev_rank) * d) + ones;
        rankDiff = (rank - prev_rank).abs().sumElements();
        std::cout << "--- [" << counter << "] ---" << std::endl;
        std::cout << rank;
    }
}