#include "Parser.h"
#include "omp.h"
#include "Timer.h"

template<typename T>
void graphIteration(std::string path, double error);

template<typename T>
void sparseMatrixIteration(std::string path, double error);

int main(int argc, char** argv) {
	double error = 1e-5;
	int datatype = 1; // 0 = float, 1 = double
	int itertype = 1; // 0 = graph, 1 = matrix

	switch(argc){
		case 4: itertype = atoi(argv[3]);
		case 3: datatype = atoi(argv[2]);
		case 2: error = atof(argv[1]);
	}

	if(datatype == 0) {
		itertype == 0 ? graphIteration<float>("graph-google.txt", error) : sparseMatrixIteration<float>("graph-google.txt", error);
	}
	else {
		itertype == 0 ? graphIteration<double>("graph-google.txt", error) : sparseMatrixIteration<double>("graph-google.txt", error);
	}

	return 0;
}

template<typename T>
void graphIteration(std::string path, double err) {
	NodesData<T> nodesData = Parser::getNodes<T>(path);
	auto& nodes = nodesData.nodes;
	std::vector<int> keys = nodesData.keys;
	Timer t("Graph");

	// initial setup
	const int N = nodes.size();
	const T initRank = 1.0 / N;
	const T d = 0.85;
	const T offset = (1 - d) / N;

	int count = 0;
	#pragma omp parallel for
	for(int i = 0; i < keys.size(); i++) {
		nodes[keys[i]]->rank = initRank;
		nodes[keys[i]]->prevRank = initRank;
	}

	// start iterating
	T temp;
	T rankDiff = 1.0;

	for (int counter = 0; rankDiff > err; counter++) {
		#pragma omp parallel for
		for(int i = 0; i < keys.size(); i++){
			auto node = nodes[keys[i]];
			node->prevRank = node->rank;
		}

		rankDiff = 0.0;
		#pragma omp parallel for reduction(+:rankDiff) private(temp)
		for (int i = 0; i < keys.size(); i++) {
			Node<T>* node = nodes[keys[i]];
			temp = 0.0;
			for(int j = 0; j < node->links.size(); j++) {
				Node<T>* child = node->links[j];
				temp += child->prevRank / child->outCount;
			}
			node->rank = offset + d * temp;
			rankDiff += fabs(node->rank - node->prevRank);
			//std::cout << "[" << counter << "] " << node->id << ": " << node->rank << std::endl;
		}
		//std::cout << "[" << counter << "] " << rankDiff << std::endl;
	}

	t.stop();

	// cleanup
	for (auto& [id, node] : nodes) {
		delete node;
	}
}

template<typename T>
void sparseMatrixIteration(std::string path, double err) {
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

	for (int counter = 0; rankDiff > err; counter++) {
		rankDiff = 0.0;
		prevRank = rank;
		rank = ((M * prevRank) * d) + ones;
		rankDiff = (rank - prevRank).abs().sum();
		//std::cout << "[" << counter << "] " << rankDiff << std::endl;
	}
}