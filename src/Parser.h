#pragma once
#include <string>
#include <unordered_map>
#include "Node.h"
#include "SparseMatrix.h"

// Data that will be returned by the parser
template<class T>
struct NodesData {
	std::unordered_map<unsigned int, Node<T>*> nodes;
	std::vector<int> keys;
};

template<class T>
struct SparseMatrixData {
	SparseMatrix<T> matrix; // stores the M matrix
	std::unordered_map<unsigned int, int> idMap; // id mappings <ID, arrayIndex>
};

// Parser class used for reading and parsing the graph
class Parser {
public:
	Parser() = delete;
	~Parser() = delete;

	template<class T>
	static NodesData<T> getNodes(std::string path, int numVerts);

	template<class T>
	static SparseMatrixData<T> getSparseMatrix(std::string path, int numVerts);
};