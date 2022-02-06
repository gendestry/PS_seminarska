#pragma once
#include <vector>
#include <iostream>
#include "Matrix.h"

// Data storage for the sparse matrix
template<class T>
struct SparseValue {
	int row;
	int col;
	T value;
};

// Sparse matrix implementation, works with the Matrix class (well the multiplication does.. :)
template<class T>
class SparseMatrix {
	unsigned int m_Rows;
	unsigned int m_Cols;
	std::vector<SparseValue<T>> m_Data;

public:
	SparseMatrix() = default;

	SparseMatrix(unsigned int rows, unsigned int cols) : m_Rows(rows), m_Cols(cols) {

	}

	// Moves the data instead of copying it
	SparseMatrix(unsigned int rows, unsigned int cols, std::vector<SparseValue<T>>&& data) : m_Rows(rows), m_Cols(cols), m_Data(std::move(data)){

	}

	inline std::vector<SparseValue<T>>& getData() noexcept { return m_Data; }
	inline unsigned int numRows() const noexcept { return m_Rows; }
	inline unsigned int numCols() const noexcept { return m_Cols; }

	void addElement(SparseValue<T> val) {
		ASSERT(val.row >= m_Rows || val.col >= m_Cols, "Pushing out of bounds")
		m_Data.push_back(val);
	}

	// Basic arithmetic operators
	Matrix<T> operator* (const Matrix<T>& other) const {
		Matrix<T> ret(m_Rows, other.numCols());
		
		for(int i = 0; i < other.numCols(); i++) {
			#pragma omp parallel for
			for(int j = 0; j < m_Data.size(); j++) {
				const SparseValue<T>& val = m_Data[j];
				#pragma omp atomic
				ret[{val.row, i}] += val.value * other[{val.col, i}];
			}
		}

		return ret;
	}

	// Indexing operators
	SparseValue<T>& operator[] (int index) {
		ASSERT(index < 0 || index >= m_Data.size(), "Index out of bounds")
		return m_Data[index];
	}

	const SparseValue<T>& operator[] (int index) const {
		ASSERT(index < 0 || index >= m_Data.size(), "Index out of bounds")
		return m_Data[index];
	}

	// Printing
	friend std::ostream& operator<< (std::ostream& out, const SparseMatrix& matrix) {
		for(auto& s_value : matrix.m_Data) {
			out << "(" << s_value.row << "," << s_value.col << "," << s_value.value << ")  ";
		}
		out << std::endl;

		return out;
	}
};
