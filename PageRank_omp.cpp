#include "Parser.h"
#include "omp.h"
#include "Timer.h"

template<typename T>
void graphIteration(std::string path);

template<typename T>
void sparseMatrixIteration(std::string path);

int main(int argc, char** argv) {
    graphIteration<double>("graph-google.txt");
    sparseMatrixIteration<double>("graph-google.txt");

    return 0;
}

template<typename T>
void graphIteration(std::string path) {
    auto nodes = Parser::getNodes<T>(path);
    Timer t("Graph");

    // initial setup
    const int N = nodes.size();
    const T initRank = 1.0 / N;
    const T d = 0.85;
    const T offset = (1 - d) / N;

	std::vector<unsigned int> keys(nodes.size());
	int count = 0;
    for (auto& [id, node] : nodes) {
		keys[count++] = id;
        node->rank = initRank;
        node->prevRank = initRank;
    }
    // start iterating
    T temp;
    T rankDiff = 1.0;
    const T err = 1e-5;

    for (int counter = 0; rankDiff > err; counter++) {
        #pragma omp parallel for
		for(int i = 0; i < keys.size(); i++){
			auto node = nodes[keys[i]];
            node->prevRank = node->rank;
		}

        rankDiff = 0.0;
		#pragma omp parallel for reduction(+:rankDiff) private(temp)
        for (int i = 0; i < keys.size(); i++) {
			auto node = nodes[keys[i]];
            temp = 0.0;
            for (auto& child : node->links) {
                temp += child->prevRank / child->outCount;
            }
            node->rank = offset + d * temp;
            rankDiff += fabs(node->rank - node->prevRank);
            //std::cout << "[" << counter << "] " << node->id << ": " << node->rank << std::endl;
        }
        //std::cout << "[" << counter << "] " << rankDiff << std::endl;
    }
}

template<typename T>
void sparseMatrixIteration(std::string path) {
    auto matrixInfo = Parser::getSparseMatrix<T>(path);
    auto& M = matrixInfo.matrix;
    Timer t("Matrix");

    const int N = matrixInfo.idMap.size();
    const T initRank = 1.0 / N;
    const T d = 0.85;
    const T offset = (1 - d) / N;

    Matrix<T> rank(N, 1, initRank);
    Matrix<T> prevRank(N, 1, initRank);
    Matrix<T> ones(N, 1, offset);

    T rankDiff = 1.0;
    const T err = 1e-5;

    for (int counter = 0; rankDiff > err; counter++) {
        rankDiff = 0.0;
        prevRank = rank;
        rank = ((M * prevRank) * d) + ones;
        rankDiff = (rank - prevRank).abs().sum();
        //std::cout << "[" << counter << "] " << rankDiff << std::endl;
    }
}