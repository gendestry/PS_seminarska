#pragma once
#include <vector>
#include <iostream>
#include <math.h>

#define S1(x) #x
#define S2(x) S1(x)
#define CONCAT(msg) (std::string(__FILE__) + "[" + S2(__LINE__) + "] in function: " + std::string(__PRETTY_FUNCTION__) + " => " + msg)
#define ASSERT(cond, msg) if(cond) { throw CONCAT(msg); }

// Used for indexing the matrix (swapping y and x would result in different matrix representation - column / row)
struct MatrixIndex {
	int y;
	int x;
};

// Matrix class used for matrix calculations
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
		#pragma omp parallel for schedule(guided)
		for(int i = 0; i < m_Matrix.size(); i++){
			m_Matrix[i] = value;
		}
	} 

	~Matrix() = default;

	// Some getters
	inline unsigned int numRows() const noexcept { return m_Rows; }
	inline unsigned int numCols() const noexcept { return m_Cols; }
	inline unsigned int size() const noexcept { return m_Cols * m_Rows; }

	// Basic arithmetic functions
	T sum() const {
		T ret = 0;
		#pragma omp parallel for schedule(guided) reduction(+:ret)
		for (int i = 0; i < m_Matrix.size(); i++){
			ret += m_Matrix[i];
		}
		
		return ret;
	}

	Matrix& add(const Matrix& other) {
		ASSERT(m_Rows != other.numRows() || m_Cols != other.numCols(), "[add] incompatible sizes")
		#pragma omp parallel for schedule(guided)
		for(int i = 0; i < m_Matrix.size(); i++) {
			m_Matrix[i] += other[i];
		}

		return *this;
	}

	Matrix& subtract(const Matrix& other) {
		ASSERT(m_Rows != other.numRows() || m_Cols != other.numCols(), "[sub] incompatible sizes")
		#pragma omp parallel for schedule(guided)
		for(int i = 0; i < m_Matrix.size(); i++) {
			m_Matrix[i] -= other[i];
		}

		return *this;
	}

	Matrix matmul(const Matrix& other) const {
		ASSERT(m_Cols != other.numRows(), "[matmul] Incompatible matrixes")

		Matrix ret(m_Rows, other.numCols());

		#pragma omp parallel for collapse(2) schedule(guided)
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
		#pragma omp parallel for schedule(guided)
		for(int i = 0; i < m_Matrix.size(); i++) {
			m_Matrix[i] = fabs(m_Matrix[i]);
		}

		return *this;
	}
	
	Matrix& scale(T scalar) noexcept {
		#pragma omp parallel for schedule(guided)
		for(int i = 0; i < m_Matrix.size(); i++) {
			m_Matrix[i] *= scalar;
		}
		
		return *this;
	}

	// Basic arithmetic operators
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

	// Indexing operators
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

	// Printing
	friend std::ostream& operator<< (std::ostream& out, const Matrix& matrix) {
		for(int i = 0; i < matrix.numRows(); i++) {
			for(int j = 0; j < matrix.numCols(); j++)
				out << matrix[{i, j}] << "  ";
			out << std::endl;
		}

		return out;
	}
};
