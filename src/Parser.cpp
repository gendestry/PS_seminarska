#include "Parser.h"
#include <fstream>


template<class T>
NodesData<T> Parser::getNodes(std::string path, int numVerts) {
	NodesData<T> data;
	auto& nodes = data.nodes;
	std::vector<int>& idMap = data.keys;
	std::ifstream file(path);
	std::string line;

	ASSERT(!file.is_open(), "File does not exist!")

	unsigned int tabloc;
	unsigned int from_id, to_id;
	Node<T>* from = nullptr;
	Node<T>* to = nullptr;
	Node<T>* prevfrom = nullptr;

	int vertCount = 0;
	while (std::getline(file, line)) {
		if (line[0] == '#') continue;

		tabloc = line.find('\t');
		from_id = atoi(line.substr(0, tabloc).c_str());
		to_id = atoi(line.substr(tabloc + 1).c_str());
		if(vertCount >= numVerts && (nodes.find(from_id) == nodes.end() || nodes.find(to_id) == nodes.end())) continue;

		// get the "from" node, if it doenst exist, create it
		if (from == nullptr || from_id != from->id) {
			auto res = nodes.find(from_id);
			if (res != nodes.end()) {
				from = res->second;
			}
			else {
				from = new Node<T>();
				from->id = from_id;
				nodes[from_id] = from;
				idMap.push_back(from_id);
				vertCount++;
			}
		}

		// get the "to" node, if it doesn't exist, create it
		auto res = nodes.find(to_id);
		if(vertCount >= numVerts && res == nodes.end()) continue;
		if (res != nodes.end()) {
			to = res->second;
		}
		else {
			to = new Node<T>();
			to->id = to_id;
			nodes[to_id] = to;
			idMap.push_back(to_id);
			vertCount++;
		}

		// append the "from" node to "to" node's children and increase the outbound connection count of "from" node
		to->links.push_back(from);
		from->outCount++;
	}

	file.close();
	return data;
}

template <class T>
SparseMatrixData<T> Parser::getSparseMatrix(std::string path, int numVerts) {
	SparseMatrixData<T> ret;

	std::ifstream file(path);
	std::string line;

	ASSERT(!file.is_open(), "File does not exist!")

	auto& matrix = ret.matrix; // the M matrix
	auto& idMap = ret.idMap; // map node ids <ID, arrayIndex> (used for indexing)

	// do the parsing
	unsigned int tabloc;
	unsigned int from_id, to_id;

	// idMap counter
	int id = 0;

	int counter = 0;
	int prev_id = -1;
	int prev_pos = 0;
	int i = 0;
	std::vector<SparseValue<T>> data;

	int vertCount = 0;
	while (std::getline(file, line)) {
		if (line[0] == '#') continue;

		// Parse line
		tabloc = line.find('\t');
		from_id = atoi(line.substr(0, tabloc).c_str());
		to_id = atoi(line.substr(tabloc + 1).c_str());
		if(vertCount >= numVerts && (idMap.find(from_id) == idMap.end() || idMap.find(to_id) == idMap.end())) continue;

		// Construct id_map
		if(from_id != prev_id){
			if(idMap.find(from_id) == idMap.end()){
				idMap[from_id] = id++;
				vertCount++;
			}
		}

		if(idMap.find(to_id) == idMap.end()){
			if(vertCount >= numVerts) continue;
			idMap[to_id] = id++;
			vertCount++;
		}

		// Add elements
		data.push_back({idMap[to_id], idMap[from_id], -1.0});
		if(from_id != prev_id){
			for(int j = i-1; j >= prev_pos; j--){
				data[j].value = 1.0/counter;
			}

			prev_pos = i;
			prev_id = from_id;
			counter = 0;
		}
		i++;
		counter++;
	}

	for(int j = i-1; j >= prev_pos; j--){
		data[j].value = 1.0/counter;
	}

	ret.matrix = SparseMatrix<T>(id, id, std::move(data));

	file.close();
	return ret;
}

template NodesData<float> Parser::getNodes(std::string path, int numVerts);
template NodesData<double> Parser::getNodes(std::string path, int numVerts);

template SparseMatrixData<float> Parser::getSparseMatrix(std::string path, int numVerts);
template SparseMatrixData<double> Parser::getSparseMatrix(std::string path, int numVerts);
