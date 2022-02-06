#include "Parser.h"
#include <fstream>


template<class T>
std::unordered_map<unsigned int, Node<T>*> Parser::getNodes(std::string path) {
	std::unordered_map<unsigned int, Node<T>*> nodes;

	std::ifstream file(path);
	std::string line;

	ASSERT(!file.is_open(), "File does not exist!")

	unsigned int tabloc;
	unsigned int from_id, to_id;
	Node<T>* from = nullptr;
	Node<T>* to = nullptr;
	Node<T>* prevfrom = nullptr;

	while (std::getline(file, line)) {
		if (line[0] == '#') continue;

		tabloc = line.find('\t');
		from_id = atoi(line.substr(0, tabloc).c_str());
		to_id = atoi(line.substr(tabloc + 1).c_str());

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
			}
		}

		// get the "to" node, if it doesn't exist, create it
		auto res = nodes.find(to_id);
		if (res != nodes.end()) {
			to = res->second;
		}
		else {
			to = new Node<T>();
			to->id = to_id;
			nodes[to_id] = to;
		}

		// append the "from" node to "to" node's children and increase the outbound connection count of "from" node
		to->links.push_back(from);
		from->outCount++;
	}

	file.close();
	return nodes;
}

template <class T>
SparseMatrixData<T> Parser::getSparseMatrix(std::string path) {
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

	while (std::getline(file, line)) {
		if (line[0] == '#') continue;

		// Parse line
		tabloc = line.find('\t');
		from_id = atoi(line.substr(0, tabloc).c_str());
		to_id = atoi(line.substr(tabloc + 1).c_str());

		// Construct id_map
		if(from_id != prev_id){
			if(idMap.find(from_id) == idMap.end()){
				idMap[from_id] = id++;
			}
		}

		if(idMap.find(to_id) == idMap.end()){
			idMap[to_id] = id++;
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

template <class T>
SparseMatrixGPUData<T> Parser::getSparseMatrixGPU(std::string path, int pkglen) {
	SparseMatrixGPUData<T> ret;
	std::vector<int>& offsets = ret.offsets;

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

	int prev_id = -1;
	int i = 0;
	std::vector<SparseValue<T>> data;
	std::vector<std::vector<SparseValue<T>>> rows;
	std::vector<int> counts;
	while (std::getline(file, line)) {
		if (line[0] == '#') continue;

		// Parse line
		tabloc = line.find('\t');
		from_id = atoi(line.substr(0, tabloc).c_str());
		to_id = atoi(line.substr(tabloc + 1).c_str());

		// Construct id_map
		if(from_id != prev_id){
			if(idMap.find(from_id) == idMap.end()){
				idMap[from_id] = id++;
				rows.push_back(std::vector<SparseValue<T>>());
				counts.push_back(0);
			}
		}
		if(idMap.find(to_id) == idMap.end()){
			idMap[to_id] = id++;
			rows.push_back(std::vector<SparseValue<T>>());
			counts.push_back(0);
		}

		// Add elements
		SparseValue<T> sv({idMap[to_id], idMap[from_id], -1.0});
		rows[idMap[to_id]].push_back(sv);
		i++;
		counts[idMap[from_id]]++;
	}
	data.reserve(i);
	offsets.push_back(0);
	int offset = 0;
	int lastOffset = 0;
	for(auto& row : rows){
		data.insert(data.end(), row.begin(), row.end());
		offset += row.size();
		if(offset > pkglen){
			offsets.push_back(lastOffset + offset);
			lastOffset += offset;
			offset = 0;
		}
	}
	if(offset > 0){
		offsets.push_back(lastOffset + offset);
	}
	for(auto& el : data){
		el.value = 1.0/counts[el.col];
	}
	ret.matrix = SparseMatrix<T>(id, id, std::move(data));
	file.close();
	return ret;
}

template std::unordered_map<unsigned int, Node<float>*> Parser::getNodes(std::string path);
template std::unordered_map<unsigned int, Node<double>*> Parser::getNodes(std::string path);

template SparseMatrixData<float> Parser::getSparseMatrix(std::string path);
template SparseMatrixData<double> Parser::getSparseMatrix(std::string path);

template SparseMatrixGPUData<float> Parser::getSparseMatrixGPU(std::string path, int pkglen);
template SparseMatrixGPUData<double> Parser::getSparseMatrixGPU(std::string path, int pkglen);
