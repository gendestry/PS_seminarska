#include "Parser.h"
#include "Timer.h"

template<typename T>
void graphIteration(std::string path, double error, int numVerts);

template<typename T>
void sparseMatrixIteration(std::string path, double error, int numVerts);

int main(int argc, char** argv) {
	double error = 1e-5;
	int datatype = 1; // 0 = float, 1 = double
	int itertype = 1; // 0 = graph, 1 = matrix
	int numVerts = -1;

	switch(argc){
		case 5: itertype = atoi(argv[4]);
		case 4: numVerts = atoi(argv[3]);
		case 3: datatype = atoi(argv[2]);
		case 2: error = atof(argv[1]);
	}
	if(numVerts == -1){
		numVerts = INT32_MAX;
	}

	if(datatype == 0) {
		itertype == 0 ? graphIteration<float>("graph-google.txt", error, numVerts) : sparseMatrixIteration<float>("graph-google.txt", error, numVerts);
	}
	else {
		itertype == 0 ? graphIteration<double>("graph-google.txt", error, numVerts) : sparseMatrixIteration<double>("graph-google.txt", error, numVerts);
	}
	

	return 0;
}

template<typename T>
void graphIteration(std::string path, double err, int numVerts) {
	NodesData<T> nodesData = Parser::getNodes<T>(path, numVerts);
	auto& nodes = nodesData.nodes;
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
void sparseMatrixIteration(std::string path, double err, int numVerts) {
	auto matrixInfo = Parser::getSparseMatrix<T>(path, numVerts);
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

	for (int counter = 0; rankDiff > err; counter++) {
		rankDiff = 0.0;
		prevRank = rank;
		rank = ((M * prevRank) * d) + ones;
		rankDiff = (rank - prevRank).abs().sum();
		//std::cout << "[" << counter << "] " << rankDiff << std::endl;
	}
}
