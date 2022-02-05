#include "Parser.h"
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

	for (auto& [id, node] : nodes) {
		node->rank = initRank;
		node->prevRank = initRank;
	}

	// start iterating
	T temp;
	T rankDiff = 1.0;
	const T err = 1e-5;

	for (int counter = 0; rankDiff > err; counter++) {
		for(auto& [id, node] : nodes)
			node->prevRank = node->rank;

		rankDiff = 0.0;
		for (auto& [id, node] : nodes) {
			temp = 0.0;
			for (auto& child : node->links) {
				temp += child->prevRank / child->outCount;
			}
			node->rank = offset + d * temp;
			rankDiff += fabs(node->rank - node->prevRank);
		}
		//std::cout << "[" << counter << "] " << rankDiff << std::endl;
	}

	t.stop();

	// cleanup
	for(auto& [id, node] : nodes) {
		delete node;
	}
}

template<typename T>
void sparseMatrixIteration(std::string path) {
	auto matrixInfo = Parser::getSparseMatrix<T>(path);
	Timer t("Matrix");

	auto& M = matrixInfo.matrix;
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