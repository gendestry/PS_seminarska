#pragma once
#include <vector>
#include <iostream>
#include <math.h>

#define ASSERT(cond, msg) if(cond) {throw msg;}

struct MatrixIndex {
	int y;
	int x;
};

template<class T>
class Matrix {
private:
	unsigned int m_Rows;
	unsigned int m_Cols;
	std::vector<T> m_Matrix;

public:
	Matrix() = default;

	Matrix(unsigned int rows, unsigned int cols) : m_Rows(rows), m_Cols(cols) {
		m_Matrix.resize(rows * cols);
	}

	Matrix(unsigned int rows, unsigned int cols, const std::vector<T>& data) : m_Rows(rows), m_Cols(cols), m_Matrix(data) {
		
	}
	
	Matrix(unsigned int rows, unsigned int cols, T value) : m_Rows(rows), m_Cols(cols) {
		m_Matrix.resize(rows * cols);
		for(auto& el : m_Matrix)
			el = value;
	} 

	~Matrix() = default;

	inline unsigned int numRows() const noexcept { return m_Rows; }
	inline unsigned int numCols() const noexcept { return m_Cols; }
	inline unsigned int size() const noexcept { return m_Cols * m_Rows; }

	T sum() const {
		T ret = 0;
		for (auto& el : m_Matrix)
			ret += el;
		
		return ret;
	}

	Matrix& add(const Matrix& other) {
		ASSERT(m_Rows != other.numRows() || m_Cols != other.numCols(), "[add] incompatible sizes")
		for(int i = 0; i < size(); i++) {
			m_Matrix[i] += other[i];
		}

		return *this;
	}

	Matrix& subtract(const Matrix& other) {
		ASSERT(m_Rows != other.numRows() || m_Cols != other.numCols(), "[sub] incompatible sizes")
		for(int i = 0; i < size(); i++) {
			m_Matrix[i] -= other[i];
		}

		return *this;
	}

	Matrix matmul(const Matrix& other) const {
		ASSERT(m_Cols != other.numRows(), "[matmul] Incompatible matrixes")

		Matrix ret(m_Rows, other.numCols());

		for(int i = 0; i < m_Rows; i++) {
			for(int j = 0; j < other.numCols(); j++) {
				T product = 0;
				for(int k = 0; k < other.numRows(); k++) {
					product += operator[]({i, k}) * other[{k, j}];
				}
				ret[{i, j}] = product;
			}
		}

		return ret;
	}

	Matrix& abs() noexcept {
		for(auto& el : m_Matrix)
			el = fabs(el);

		return *this;
	}
	
	Matrix& scale(T scalar) noexcept {
		for(auto& el : m_Matrix)
			el *= scalar;
		
		return *this;
	}

	// ARITHMETIC OPERATORS

	Matrix operator+ (const Matrix& other) const {
		Matrix mat(m_Rows, m_Cols, m_Matrix);
		mat += other;
		return mat;
	}

	Matrix operator- (const Matrix& other) const {
		Matrix mat(m_Rows, m_Cols, m_Matrix);
		mat -= other;
		return mat;
	}

	Matrix operator* (const Matrix& other) const {
		return matmul(other);
	}

	Matrix operator* (T scalar) const noexcept {
		Matrix mat(m_Rows, m_Cols, m_Matrix);
		mat *= scalar;
		return mat;
	}

	Matrix& operator+= (const Matrix& other) {
		return add(other);
	}

	Matrix& operator-= (const Matrix& other) {
		return subtract(other);
	}

	Matrix operator*= (const Matrix& other) const {
		return matmul(other);
	}

	Matrix& operator*= (T scalar) noexcept {
		return scale(scalar);
	}

	// INDEXING OPERATORS
	T& operator[] (int index) {
		ASSERT(index < 0 || index >= m_Matrix.size(), "Index out of bounds")
		return m_Matrix[index];
	}

	const T& operator[] (int index) const {
		ASSERT(index < 0 || index >= m_Matrix.size(), "Index out of bounds")
		return m_Matrix[index];
	}

	T& operator[] (MatrixIndex i) {
		unsigned int index = i.x + i.y * m_Cols;
		ASSERT(index < 0 || index >= m_Matrix.size(), "Index out of bounds")
		return m_Matrix[index];
	}

	const T& operator[] (MatrixIndex i) const {
		unsigned int index = i.x + i.y * m_Cols;
		ASSERT(index < 0 || index >= m_Matrix.size(), "Index out of bounds")
		return m_Matrix[index];
	}

	// PRINTING
	friend std::ostream& operator<< (std::ostream& out, const Matrix& matrix) {
		for(int i = 0; i < matrix.numRows(); i++) {
			for(int j = 0; j < matrix.numCols(); j++)
				out << matrix[{i, j}] << "  ";
			out << std::endl;
		}

		return out;
	}
};