#pragma once
#include <vector>
#include "Matrix.h"

template<class T>
struct SparseValue {
	int row;
	int col;
	T value;
};

template<class T>
class SparseMatrix {
	unsigned int m_Rows;
	unsigned int m_Cols;
	std::vector<SparseValue<T>> m_Data;

public:
	SparseMatrix() = delete;

	SparseMatrix(unsigned int rows, unsigned int cols) : m_Rows(rows), m_Cols(cols) {

	}

	void addElement(int row, int col, T value) {
		ASSERT(row >= m_Rows || col >= m_Cols, "SparseMatrix: pushing out of bounds")
		m_Data.emplace_back(row, col, value);
	}

	void addElement(SparseValue<T> val) {
		ASSERT(val.row >= m_Rows || val.col >= m_Cols, "SparseMatrix: pushing out of bounds")
		m_Data.push_back(val);
	}

	Matrix<T> operator* (const Matrix<T>& other) const {
		Matrix<T> ret(m_Rows, other.numCols());
		
		for(int i = 0; i < other.numCols(); i++) {
			for(int j = 0; j < m_Data.size(); j++) {
				const SparseValue<T>& val = m_Data[j];

				ret[{val.row, i}] += val.value * other[{val.col, i}];
			}
		}

		return ret;
	}
};
